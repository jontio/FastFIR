# FastFIR
1D real fast FIR filtering.

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

Jonti 2015
http://jontio.zapto.org


