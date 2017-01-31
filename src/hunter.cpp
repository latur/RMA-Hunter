#include <iostream>
#include <fstream>
#include <unistd.h>
#include "structs.h"

int main(int argc, char *argv[]){
    using namespace std;
    
    string key = argv[1];
    char * tpx = argv[2];
    string sdf = argv[3];
    string res = argv[4];

    container<vcfline> vcf_file("/tmp/" + key + ".xvcf");
    container<bedline> bed_file("/tmp/" + key + ".xbed");

    sdfline s;
    ifstream sdfs(sdf);

    ofstream ofs1(res + key + ".t1");
    ofstream ofs2(res + key + ".t2");
    ofstream ofs3(res + key + ".t3");

    bool bed_loaded = bed_file.total() > 0;
    
    while (sdfs >> s)
    {
        // Только выбранный класс интересует нас
        // tpx = 'A','C','AB','ABCD'
        if (s.chr == -1) continue;
        if (tpx[0] == '0' && s.e[20] != "A") continue;
        if (tpx[0] == '1' && s.e[20] != "C") continue;
        if (tpx[0] == '2' && !(s.e[20] == "A" || s.e[20] == "B")) continue;

        // Проверка, есть ли точка в интервалах BED
        if (bed_loaded)
        {
            bool in_bed = false;
            for (int p = 0; p < bed_file.chrx[s.chr].count; ++p)
            {
                if (s.pos < bed_file.chrx[s.chr].data[p].from) continue;
                if (s.pos > bed_file.chrx[s.chr].data[p].to) continue;
                in_bed = true;
                continue;
            }
            if (!in_bed) continue;
        }
        
        // Поиск в файле юзера
        // zygosity = ['X', '1/1', '0/1', '0/0', './1', './.'];
        short int zyg = -1;
        try
        {
            // Здесь можно ускорить
            for (int p = 0; p < vcf_file.chrx[s.chr].count; ++p)
            {
                if (vcf_file.chrx[s.chr].data[p].pos != s.pos) continue;
                if (vcf_file.chrx[s.chr].data[p].ref != s.e[2]) continue;
                if (vcf_file.chrx[s.chr].data[p].alt != s.e[3]) continue;
                zyg = vcf_file.chrx[s.chr].data[p].zyg;
                break;
            }
        } catch (...){}
        
        // 0.
        // Все, что находится в файле юзера (и в последней колонке генотип 1/1)
        // и не имеет в референсном файле флага "FALSE_SYNONYM" просто выкидывается
        if (s.e[21] != "FALSE_SYNONYM" && zyg == 1) continue;

        // 1.
        // Все из выбранного класса (в файле SDF_S2.csv),
        // что не находится в файле юзера сохраняется в список 1 (c генотипом ./.)
        if (s.e[21] != "FALSE_SYNONYM" && zyg == -1)
        {
            s.e[22] = "./.";
            ofs1 << s;
        }

        // 2.
        // Все из выбранного класса (в файле SDF_S2.csv),
        // что находится в файле юзера в зиготности 0/0
        // сохраняется в список 2 (дальше этому классу соответствует генотип 1/1).
        if (s.e[21] != "FALSE_SYNONYM" && zyg == 3)
        {
            s.e[22] = "0/0";
            s.e[23] = "*";
            ofs1 << s;
        }

        // 3.
        // Все, что находится в формате 0/1 (в файле юзера)
        // из выбранного класса (в нашем файле) сохраняется в список 3 (0/1).
        if (s.e[21] != "FALSE_SYNONYM" && zyg == 2)
        {
            s.e[22] = "0/1";
            ofs2 << s;
        }

        // 4.
        // Все, что нашлось в файле юзера из выбранного класса с флагом "FALSE_SYNONYM"
        // нужно сохранить в список 4 (зиготность из файла as-is).
        if (s.e[21] == "FALSE_SYNONYM" && zyg != -1)
        {
            ofs3 << s;
        }
    }
    
    ofs1.close();
    ofs2.close();
    ofs3.close();

    // while (true) sleep(10);
    return 0;
}

/**

g++ -Werror -Wall -std=c++11 src/hunter.cpp -o exec/hunter
valgrind ./exec/hunter EFYKSHHXTP7Y3Z0K9 0

*/
