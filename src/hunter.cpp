#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include "hunter.h"

int main(int argc, char *argv[])
{
    using namespace std;
    
    string key = argv[1]; // filename
    char * tpx = argv[2]; // coding/n
    string sdf = argv[3]; // supplementary data file
    string res = argv[4]; // base dir
    double afs = strtod(argv[5], NULL); // max ref afs

    ifstream sdfs(sdf);
    container<vcfline> vcf_file("/tmp/" + key + ".xvcf");
    container<bedline> bed_file("/tmp/" + key + ".xbed");
    
    bool bed_loaded = (bed_file.total() > 0);

    ofstream tbl1(res + key + ".t1");
    ofstream tbl2(res + key + ".t2");
    ofstream tbl3(res + key + ".t3");

    sdfline s;
    while (sdfs >> s)
    {
        // Только выбранный класс интересует нас (only coding? = 1)
        // tpx = '1','0'
        if (s.chr == -1) continue;
        if (tpx[0] == '1' && s.e[20] == "NO") continue;
        if (s.maxafs > afs) continue;

        // Проверка, есть ли точка в интервалах BED
        // Тут можно мою любимую корневую декомпозицию позже замутить для скорости
        // или или ещё что похитрее, дерево отрезков, например (слишком много памяти отожрёт)
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
        
        // 1.
        // Что не находится в файле юзера сохраняется в список 1 c генотипом ./.
        if (zyg == -1)
        {
            s.e[21] = "./.";
            tbl1 << s;
        }

        // 2.
        // Что находится в файле юзера в зиготности 0/0
        // сохраняется в список 1 (изменяем зиготность 1/1, помечаем)
        if (zyg == 3)
        {
            s.e[21] = "1/1";
            s.e[22] = "*";
            tbl1 << s;
        }

        // 3.
        // Что находится в файле юзера в зиготности 0/1
        // сохраняется в список 1 (оставляем зиготность 0/1)
        if (zyg == 2)
        {
            s.e[21] = "0/1";
            tbl1 << s;
        }

        // 4.
        // Работаем со всторым файлом ...
    }
    
    tbl1.close();
    tbl2.close();
    tbl3.close();

    return 0;
}

/*

g++ -Werror -Wall -std=c++11 src/hunter.cpp -o exec/hunter
valgrind ./exec/hunter EFYKSHHXTP7Y3Z0K9 0
g++ -g -std=c++11 src/hunter.cpp -o exec/hunter

*/
