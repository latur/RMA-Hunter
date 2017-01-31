#pragma once
#include <stdlib.h>
#include <iomanip>

using namespace std;

struct bedline
{
    bedline()
    {}

    bedline(short int chr, int from, int to) 
    : chr(chr), from(from), to(to) 
    {}

    bedline& operator=(const bedline& other)
    {
        chr = other.chr;
        from = other.from;
        to = other.to;
        return *this;
    }
    
    friend istream& operator >> ( istream& is, bedline& e )
    {
        short int chr;
        int from, to;
        is >> chr >> from >> to;
        e = bedline(chr, from, to);
        return is;
    }

    friend ostream& operator <<( ostream& out, const bedline& e )
    {
        out << "chr" << (e.chr + 1) << " " << e.from << "-" << e.to << "\n";
        return out;
    }

    short int chr;
    int from, to;
};

struct vcfline 
{
    vcfline()
    {}

    vcfline(short int chr, int pos, string ref, string alt, short int zyg)
    : chr(chr), pos(pos), ref(ref), alt(alt), zyg(zyg) 
    {}

    ~vcfline()
    {}

    vcfline& operator=(const vcfline& other)
    {
        chr = other.chr;
        pos = other.pos;
        ref = other.ref;
        alt = other.alt;
        zyg = other.zyg;
        return *this;
    }

    friend istream& operator >> ( istream& is, vcfline& e )
    {
        short int chr, zyg;
        string ref, alt;
        int pos;
        is >> chr >> pos >> ref >> alt >> zyg;
        e = vcfline(chr, pos, ref, alt, zyg);
        return is;
    }

    friend ostream& operator << ( ostream& out, const vcfline& e )
    {
        out << e.chr << "\t" << e.pos << "\t" << e.ref << "\t" 
            << e.alt << "\t" << e.pos << "\t" << e.zyg << "\n";
        return out;
    }

    short int chr;
    int pos;
    string ref;
    string alt;
    short int zyg; 
};

template <typename T>
struct container 
{
    struct box 
    {
        box()
        : size(4)
        , count(0)
        {
            data = new T[size];
        }
        
        ~box()
        {
            delete [] data;
        }
        
        void resize()
        {
            T * t = new T[size * 2];
            for (int k = 0; k < size; ++k) t[k] = data[k];
            delete [] data;
            data = t;
            size *= 2;
        }
    
        void append(T item)
        {
            if (item.chr == 0) return ;
            if (size - 1 == count) resize();
            data[++count] = item;
        }
        
        void echo()
        {
            for (int k = 0; k < count; ++k) cout << data[k];
        }
    
        T * data;
        int size;
        int count;
    };

    container(string fname)
    {
        T tmp;
        ifstream raw(fname);
        chrx = new box [24];
        while (raw >> tmp)
        {
            chrx[tmp.chr - 1].append(tmp);
            ++elements;
        }
    }
    
    ~container()
    {
        delete [] chrx;
    }
    
    long int total()
    {
        long int sum = 0;
        for (int chr = 0; chr < 24; ++chr) sum += (chrx[chr].count);
        return sum;
    }

    box * chrx;
    int elements = 0;
};

struct sdfline
{
    sdfline()
    {}
    
    sdfline(string raw)
    : raw(raw)
    {
        chr = -1;
        unsigned short int k = 0;
        e[k] = "";
        for (unsigned short int x = 0; x < raw.length(); ++x)
        {
            if (raw[x] == ',')
            {
                e[++k] = "";
                continue;
            }
            e[k] += raw[x];
        }
        try {
            if (e[0] == "X") chr = 22;
            if (e[0] == "Y") chr = 23;
            if (chr == -1) chr = stoi(e[0]) - 1;
            
            pos = stoi(e[1]);
        }
        catch (...) {}
    }

    ~sdfline()
    {}
    
    friend istream& operator >> ( istream& is, sdfline& e )
    {
        string raw;
        is >> raw;
        e = sdfline(raw);
        return is;
    }
    
    friend ostream& operator << ( ostream& out, const sdfline& e )
    {
        double x  = atof(e.e[9].c_str());
        double t1 = atof(e.e[10].c_str());
        if (t1 > x) x = t1;
        double t2 = atof(e.e[11].c_str());
        if (t2 > x) x = t2;

        out.precision(6);
        out << fixed << (9 - x);
        out << string(2 - e.e[0].length(), '0') << e.e[0];
        out << string(9 - e.e[1].length(), '0') << e.e[1];
        out << " ";
        
        // Chromosome,Position,Ref,Alt
        out << e.e[0] << "," << e.e[1] << "," << e.e[2] << "," << e.e[3] << ",";
        // ID,Gene,Substitution,UniProt_ID
        out << e.e[4] << "," << e.e[5] << "," << e.e[7] << "," << e.e[8] << ",";
        // PROVEAN_prediction, Polyphen_prediction, SIFT_prediction
        out << e.e[15] << "," << e.e[17] << "," << e.e[19] << ",";
        // PROVEAN_score, Polyphen_score, SIFT_score
        out << e.e[14] << "," << e.e[16] << "," << e.e[18] << ",";
        // 1000G_AF, ExAC_AF, ESP_AF
        out << e.e[9] << "," << e.e[10] << "," << e.e[11] << ",";
        // zyg
        out << e.e[22] << "," << e.e[23] << "\n";
        return out;
    }
    
    short int chr;
    int pos;
    string raw;
    string e[25];
};






