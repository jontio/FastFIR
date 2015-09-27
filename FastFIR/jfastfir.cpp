//---------------------------------------------------------------------------


#include "jfastfir.h"

//---------------------------------------------------------------------------


//---slow FIR

JSlowFIRFilter::JSlowFIRFilter(vector<kffsamp_t> &imp_responce)
{
    Cof=imp_responce;
    reset();
}

void JSlowFIRFilter::reset()
{
    Buf.assign(Cof.size(),0);
    BufPtr=0;
}

int JSlowFIRFilter::Update(vector<kffsamp_t> &data)
{
    return Update(data.data(), data.size());
}

int JSlowFIRFilter::Update(kffsamp_t *data, int Size)
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

    return Size;

}



//---------------


//---fast FIR


JFastFIRFilter::JFastFIRFilter(vector<kffsamp_t> &imp_responce,size_t &_nfft)
{
    _nfft=pow(2.0,(ceil(log2(_nfft))));
    cfg=kiss_fastfir_alloc(imp_responce.data(),imp_responce.size(),&_nfft,0,0);
    nfft=_nfft;
    reset();
}

JFastFIRFilter::JFastFIRFilter(vector<kffsamp_t> &imp_responce)
{
    nfft=4*imp_responce.size();
    nfft=pow(2.0,(ceil(log2(nfft))));
    cfg=kiss_fastfir_alloc(imp_responce.data(),imp_responce.size(),&nfft,0,0);
    reset();
}

void JFastFIRFilter::reset()
{
    remainder.assign(nfft*2,0);
    idx_inbuf=0;
    remainder_ptr=nfft;
}

int JFastFIRFilter::Update(vector<kffsamp_t> &data)
{
    return Update(data.data(), data.size());
}

int JFastFIRFilter::Update(kffsamp_t *data,int Size)
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


    //we should anyways have enough to return but if we dont this happens. this should be avoided else a discontinuity of frames occurs. set remainder to zero and set remainder_ptr to nfft before running to avoid this
    if(currentwantednum>0)
    {
        remainder_ptr+=currentwantednum;
    }

    //return how many items we changed
    return Size-currentwantednum;

}

JFastFIRFilter::~JFastFIRFilter()
{
    free(cfg);
}

//-----------

