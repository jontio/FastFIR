# FastFIR
1D real fast FIR filtering.
It's really fast compared to slow filtering!!!

##API usage with Qt

```C++
//create pointer
QJFastFIRFilter *fastfir;

//create fast FIR filter
fastfir = new QJFastFIRFilter(this);

//set the kernel of the filter, in this case a Low Pass filter at 800Hz
fastfir->setKernel(JFilterDesign::LowPassHanning(800,48000,1001));

//process data (data is QVector<kffsamp_t>, eg "QVector<kffsamp_t> data;")
fastfir->Update(data);
```

##Getting started

Clone/Download repository.

Demo project requires at least Qt5.
Open [FastFIR/Demo/Demo.pro](FastFIR/Demo/Demo.pro) with Qt creator and run.
Demo compares slow FIR and fast FIR times and low pass filter's noise at 800Hz and outputs this to the default soundcard device.

![Demo program output](FastFIR/Demo/screenshot.png)

Jonti 2015
http://jontio.zapto.org


