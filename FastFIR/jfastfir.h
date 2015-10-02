//---------------------------------------------------------------------------

//pure C++ version. untested.

#ifndef JFASTFIR_H
#define JFASTFIR_H

//kiss_fft_scalar sets type of scaller used can be float or double
//kffsamp_t is the same thing as kiss_fft_scalar as we are doing real FIRs (REAL_FASTFIR==1)

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
    JSlowFIRFilter();
    void setKernel(vector<kffsamp_t> imp_responce);
    void Update(kffsamp_t *data,int Size);
    void Update(vector<kffsamp_t> &data);
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
    JFastFIRFilter();
    int setKernel(vector<kffsamp_t> imp_responce,int nfft);
    int setKernel(vector<kffsamp_t> imp_responce);
    void Update(kffsamp_t *data,int Size);
    void Update(vector<kffsamp_t> &data);
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

//filter design
//all designs are using the window method and derived from the low pass filter

class JFilterDesign
{
public:
    JFilterDesign(){}
    static vector<kffsamp_t> LowPassHanning(double FrequencyCutOff, double SampleRate, int Length);
    static vector<kffsamp_t> HighPassHanning(double FrequencyCutOff, double SampleRate, int Length);
    static vector<kffsamp_t> BandPassHanning(double LowFrequencyCutOff,double HighFrequencyCutOff, double SampleRate, int Length);
    static vector<kffsamp_t> BandStopHanning(double LowFrequencyCutOff,double HighFrequencyCutOff, double SampleRate, int Length);
private:
};

#endif  //JFASTFIR
