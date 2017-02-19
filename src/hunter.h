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
        // SRC:
        // Chromosome, Position, Ref, Alt, ID,
        // Gene, Type, Substitution, UniProt_ID, 1000G_AF,
        // ExAC_AF, ESP_AF, COSMIC_count, ClinVar, PROVEAN_score,
        // PROVEAN_prediction, Polyphen_score, Polyphen_prediction, SIFT_score, SIFT_prediction,
        // Coding
        
        chr = -1; // from 0 to 21 (22-X 23-Y)
        maxafs = 999;
        unsigned short int k = 0;
        e[0] = "";
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
            maxafs = atof(e[9].c_str());
            maxafs = max(maxafs, atof(e[10].c_str()));
            maxafs = max(maxafs, atof(e[11].c_str()));
        } catch (...) {}
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
    
    friend ostream& operator << ( ostream& out, const sdfline& h )
    {
        // first column (sorting)
        out << fixed << h.maxafs;
        out << string(2 - h.e[0].length(), '0') << h.e[0];
        out << string(9 - h.e[1].length(), '0') << h.e[1];
        out << " ";
        
        // OUTPUT:
        for (unsigned short int i = 0; i < 23; ++i) out << h.e[i] << (i == 22 ? "\n" : ",");
        return out;
    }
    
    short int chr;
    double maxafs;
    int pos;
    string raw;
    string e[23];
};






