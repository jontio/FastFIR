//---------------------------------------------------------------------------

//pure C++ version. untested.

#ifndef JFASTFIR_H
#define JFASTFIR_H

//for double real FIR
#define kiss_fft_scalar double
#define REAL_FASTFIR 1
#include "../kiss_fft130_jfork1.0.0/tools/kiss_fastfir.h"
#include "../kiss_fft130_jfork1.0.0/tools/kiss_fftr.h"

#include <memory>
#include <math.h>
#include <vector>

using namespace std;


//slow fir

class JSlowFIRFilter
{    
public:
    JSlowFIRFilter(vector<kffsamp_t> &imp_responce);
    int Update(kffsamp_t *data,int Size);
    int Update(vector<kffsamp_t> &data);
    void reset();
private:
    vector<kffsamp_t> Cof;
    vector<kffsamp_t> Buf;
    int BufPtr;
};

//fast fir

class JFastFIRFilter
{
public:
    JFastFIRFilter(vector<kffsamp_t> &imp_responce,size_t &nfft);
    JFastFIRFilter(vector<kffsamp_t> &imp_responce);
    int Update(kffsamp_t *data,int Size);
    int Update(vector<kffsamp_t> &data);
    void reset();
    ~JFastFIRFilter();
private:
    size_t nfft;
    kiss_fastfir_cfg cfg;
    size_t idx_inbuf;
    std::vector<kffsamp_t> inbuf;
    std::vector<kffsamp_t> outbuf;
    std::vector<kffsamp_t> remainder;
    int remainder_ptr;
};


#endif  //JFASTFIR
