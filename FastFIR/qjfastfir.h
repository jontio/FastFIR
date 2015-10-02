//---------------------------------------------------------------------------

#ifndef QJFASTFIR_H
#define QJFASTFIR_H

//kiss_fft_scalar sets type of scaller used can be float or double
//kffsamp_t is the same thing as kiss_fft_scalar as we are doing real FIRs (REAL_FASTFIR==1)

//for double real FIR
#define kiss_fft_scalar double
#define REAL_FASTFIR 1
#include "../kiss_fft130_jfork1.0.0/tools/kiss_fastfir.h"
#include "../kiss_fft130_jfork1.0.0/tools/kiss_fftr.h"

#include <QVector>
#include <QObject>
#include <QDebug>

//slow fir

class QJSlowFIRFilter : public QObject
{
    Q_OBJECT
public:
    QJSlowFIRFilter(QObject *parent = 0);
    void setKernel(QVector<kffsamp_t> imp_responce);
    void Update(kffsamp_t *data,int Size);
    void Update(QVector<kffsamp_t> &data);
    void reset();
private:
    QVector<kffsamp_t> Cof;
    QVector<kffsamp_t> Buf;
    int BufPtr;
};

//fast fir

class QJFastFIRFilter : public QObject
{
    Q_OBJECT
public:
    QJFastFIRFilter(QObject *parent = 0);
    int setKernel(QVector<kffsamp_t> imp_responce,int nfft);
    int setKernel(QVector<kffsamp_t> imp_responce);
    void Update(kffsamp_t *data,int Size);
    void Update(QVector<kffsamp_t> &data);
    void reset();
    ~QJFastFIRFilter();
private:
    size_t nfft;
    kiss_fastfir_cfg cfg;
    size_t idx_inbuf;
    QVector<kffsamp_t> inbuf;
    QVector<kffsamp_t> outbuf;
    QVector<kffsamp_t> remainder;
    int remainder_ptr;
};

//filter design
//all designs are using the window method and derived from the low pass filter

class QJFilterDesign
{
public:
    QJFilterDesign(){}
    static QVector<kffsamp_t> LowPassHanning(double FrequencyCutOff, double SampleRate, int Length);
    static QVector<kffsamp_t> HighPassHanning(double FrequencyCutOff, double SampleRate, int Length);
    static QVector<kffsamp_t> BandPassHanning(double LowFrequencyCutOff,double HighFrequencyCutOff, double SampleRate, int Length);
    static QVector<kffsamp_t> BandStopHanning(double LowFrequencyCutOff,double HighFrequencyCutOff, double SampleRate, int Length);
private:
};


#endif  //QJFASTFIR
