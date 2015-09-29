#include "demo.h"
#include "ui_demo.h"

Demo::Demo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Demo)
{
    ui->setupUi(this);

    //create pointers
    QJFastFIRFilter *fastfir;
    QJSlowFIRFilter *slowfir;

    //create fast fir LPF
    fastfir = new QJFastFIRFilter(this);
    fastfir->setKernel(QJFilterDesign::LowPassHanning(800,48000,1001));

    //create slow fir LPF
    slowfir = new QJSlowFIRFilter(this);
    slowfir->setKernel(QJFilterDesign::LowPassHanning(800,48000,1001));

    //make some data
    QVector<kffsamp_t> buf;
    buf.resize(48000*10);

    //time slow fir
    timer.start();
    slowfir->Update(buf);
    nMilliseconds = timer.elapsed();
    ui->plainTextEdit->appendPlainText(((QString)"Slow FIR took %1 ms for %2 double samples with a kernel of 1001 samples").arg(nMilliseconds).arg(buf.size()));

    //time fast fir
    timer.start();
    fastfir->Update(buf);
    nMilliseconds = timer.elapsed();
    ui->plainTextEdit->appendPlainText(((QString)"Fast FIR took %1 ms for %2 double samples with a kernel of 1001 samples").arg(nMilliseconds).arg(buf.size()));

    //use the fast fir to LPF some random samples and output them to the sound card
    ui->plainTextEdit->appendPlainText("Outputting 800Hz low pass filtered random samples to the soundcard using the fast fir");
    QAudioFormat format;
    format.setChannelCount(1);
    format.setCodec("audio/pcm");
    format.setSampleRate(48000);
    format.setSampleSize(16);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    generator = new Generator(format, this);
    QAudioOutput *audiooutput = new QAudioOutput(format, this);
    generator->start();
    audiooutput->start(generator);
}


Demo::~Demo()
{
    delete ui;
}
#include <vector>

Generator::Generator(QAudioFormat format, QObject *parent) : QIODevice(parent)
{

    //create fast FIR LPF
    fastfir = new QJFastFIRFilter(this);
    fastfir->setKernel(QJFilterDesign::LowPassHanning(800,format.sampleRate(),2111));

}

qint64 Generator::readData(char *data, qint64 len)
{

    //checks
    if(len==0)return 0;
    int nsamps=len/sizeof(short);
    if(buf.size()<nsamps)buf.resize(nsamps);

    //make noise
    for(int i=0;i<nsamps;i++)buf[i]=0.5*(2.0*((((double)(qrand()%32767))/32767.0)-0.5));

    //fast fir filter
    fastfir->Update(buf.data(),nsamps);

    //return shorts
    short *ptr = reinterpret_cast<short *>(data);
    for(int i=0;i<nsamps;i++)
    {
        (*ptr)=(buf[i]*32768.0);
        ptr++;
    }
    return len;
}

