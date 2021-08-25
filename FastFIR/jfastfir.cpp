//---------------------------------------------------------------------------


#include "jfastfir.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//c files we need to compile
#include "../kiss_fft130_jfork1.0.0/kiss_fft.c"
#include "../kiss_fft130_jfork1.0.0/tools/kiss_fastfir.c"
#include "../kiss_fft130_jfork1.0.0/tools/kiss_fftr.c"

//---------------------------------------------------------------------------

//---filter design

double sinc_normalized(double val)
{
    if (val==0)return 1.0;
    return (sin(M_PI*val)/(M_PI*val));
}

vector<kffsamp_t> JFilterDesign::LowPassHanning(double FrequencyCutOff, double SampleRate, int Length)
{
    vector<kffsamp_t> h;
    if(Length<1)return h;
    if(!(Length%2))Length++;
    int j=1;
    for(int i=(-(Length-1)/2);i<=((Length-1)/2);i++)
    {
        double w=0.5*(1.0-cos(2.0*M_PI*((double)j)/((double)(Length))));
        h.push_back(w*(2.0*FrequencyCutOff/SampleRate)*sinc_normalized(2.0*FrequencyCutOff*((double)i)/SampleRate));
        j++;
    }

    return h;

/* in matlab this function is
idx = (-(Length-1)/2:(Length-1)/2);
hideal = (2*FrequencyCutOff/SampleRate)*sinc(2*FrequencyCutOff*idx/SampleRate);
h = hanning(Length)' .* hideal;
*/

}

vector<kffsamp_t> JFilterDesign::HighPassHanning(double FrequencyCutOff, double SampleRate, int Length)
{
    vector<kffsamp_t> h;
    if(Length<1)return h;
    if(!(Length%2))Length++;

    vector<kffsamp_t> h1;
    vector<kffsamp_t> h2;
    h2.assign(Length,0);
    h2[(Length-1)/2]=1.0;

    h1=LowPassHanning(FrequencyCutOff,SampleRate,Length);
    if((h1.size()==(size_t)Length)&&(h2.size()==(size_t)Length))
    {
        for(int i=0;i<Length;i++)h.push_back(h2[i]-h1[i]);
    }

    return h;
}

vector<kffsamp_t> JFilterDesign::BandPassHanning(double LowFrequencyCutOff,double HighFrequencyCutOff, double SampleRate, int Length)
{
    vector<kffsamp_t> h;
    if(Length<1)return h;
    if(!(Length%2))Length++;

    vector<kffsamp_t> h1;
    vector<kffsamp_t> h2;

    h2=LowPassHanning(HighFrequencyCutOff,SampleRate,Length);
    h1=LowPassHanning(LowFrequencyCutOff,SampleRate,Length);

    if((h1.size()==(size_t)Length)&&(h2.size()==(size_t)Length))
    {
        for(int i=0;i<Length;i++)h.push_back(h2[i]-h1[i]);
    }

    return h;
}

vector<kffsamp_t> JFilterDesign::BandStopHanning(double LowFrequencyCutOff,double HighFrequencyCutOff, double SampleRate, int Length)
{
    vector<kffsamp_t> h;
    if(Length<1)return h;
    if(!(Length%2))Length++;

    vector<kffsamp_t> h1;
    vector<kffsamp_t> h2;
    h2.assign(Length,0);
    h2[(Length-1)/2]=1.0;

    h1=BandPassHanning(LowFrequencyCutOff,HighFrequencyCutOff,SampleRate,Length);
    if((h1.size()==(size_t)Length)&&(h2.size()==(size_t)Length))
    {
        for(int i=0;i<Length;i++)h.push_back(h2[i]-h1[i]);
    }

    return h;
}


//---slow FIR

JSlowFIRFilter::JSlowFIRFilter()
{
    Cof.push_back(1.0);
    reset();
}

void JSlowFIRFilter::setKernel(vector<kffsamp_t> imp_responce)
{
    Cof=imp_responce;
    reset();
}

void JSlowFIRFilter::reset()
{
    Buf.assign(Cof.size(),0);
    BufPtr=0;
}

void JSlowFIRFilter::Update(vector<kffsamp_t> &data)
{
    Update(data.data(), data.size());
}

void JSlowFIRFilter::Update(kffsamp_t *data, int Size)
{

    if(Buf.size()!=Cof.size())
    {
        Buf.resize(Cof.size());
        BufPtr=0;
    }
    BufPtr%=Buf.size();
    unsigned int i;
    unsigned int COfsz=Cof.size();
    int Bufsz=Buf.size();

    for(int j=0;j<Size;j++)
    {
        Buf[BufPtr]=data[j];
        BufPtr++;if(BufPtr==Bufsz)BufPtr=0;
        data[j]=0;
        for(i=0;i<COfsz;i++)
        {
            data[j]+=Buf[BufPtr]*Cof[i];
            BufPtr++;if(BufPtr==Bufsz)BufPtr=0;
        }
    }

}


//---------------


//---fast FIR

JFastFIRFilter::JFastFIRFilter()
{
    vector<kffsamp_t> tvect;
    tvect.push_back(1.0);
    nfft=2;
    cfg=kiss_fastfir_alloc(tvect.data(),tvect.size(),&nfft,0,0);
    reset();
}

int JFastFIRFilter::setKernel(vector<kffsamp_t> imp_responce)
{
    int _nfft=imp_responce.size()*4;//rule of thumb
    _nfft=pow(2.0,(ceil(log2(_nfft))));
    return setKernel(imp_responce,_nfft);
}

int JFastFIRFilter::setKernel(vector<kffsamp_t> imp_responce, int _nfft)
{
    if(!imp_responce.size())return nfft;
    free(cfg);
    _nfft=pow(2.0,(ceil(log2(_nfft))));
    nfft=_nfft;
    cfg=kiss_fastfir_alloc(imp_responce.data(),imp_responce.size(),&nfft,0,0);
    reset();
    return nfft;
}

void JFastFIRFilter::reset()
{
    remainder.assign(nfft*2,0);
    idx_inbuf=0;
    remainder_ptr=nfft;
}

void JFastFIRFilter::Update(vector<kffsamp_t> &data)
{
    Update(data.data(), data.size());
}

void JFastFIRFilter::Update(kffsamp_t *data,int Size)
{

    //ensure enough storage
    if((inbuf.size()-idx_inbuf)<(size_t)Size)
    {
        inbuf.resize(Size+nfft);
        outbuf.resize(Size+nfft);
    }

    //add data to storage
    memcpy ( inbuf.data()+idx_inbuf, data, sizeof(kffsamp_t)*Size );
    size_t nread=Size;

    //fast fir of storage
    size_t nwrite=kiss_fastfir(cfg, inbuf.data(), outbuf.data(),nread,&idx_inbuf);

    int currentwantednum=Size;
    int numfromremainder=min(currentwantednum,remainder_ptr);

    //return as much as posible from remainder buffer
    if(numfromremainder>0)
    {
        memcpy ( data, remainder.data(), sizeof(kffsamp_t)*numfromremainder );

        currentwantednum-=numfromremainder;
        data+=numfromremainder;

        if(numfromremainder<remainder_ptr)
        {
            remainder_ptr-=numfromremainder;
            memcpy ( remainder.data(), remainder.data()+numfromremainder, sizeof(kffsamp_t)*remainder_ptr );
        } else remainder_ptr=0;
    }

    //then return stuff from output buffer
    int numfromoutbuf=std::min(currentwantednum,(int)nwrite);
    if(numfromoutbuf>0)
    {
        memcpy ( data, outbuf.data(), sizeof(kffsamp_t)*numfromoutbuf );
        currentwantednum-=numfromoutbuf;
        data+=numfromoutbuf;
    }

    //any left over is added to remainder buffer
    if(((size_t)numfromoutbuf<nwrite)&&(nwrite>0))
    {
        memcpy ( remainder.data()+remainder_ptr, outbuf.data()+numfromoutbuf, sizeof(kffsamp_t)*(nwrite-numfromoutbuf) );
        remainder_ptr+=(nwrite-numfromoutbuf);
    }

    //if currentwantednum>0 then some items were not changed, this should not happen
    //we should anyways have enough to return but if we dont this happens. this should be avoided else a discontinuity of frames occurs. set remainder to zero and set remainder_ptr to nfft before running to avoid this
    if(currentwantednum>0)
    {
        remainder_ptr+=currentwantednum;
    }

}

JFastFIRFilter::~JFastFIRFilter()
{
    free(cfg);
}

//-----------

