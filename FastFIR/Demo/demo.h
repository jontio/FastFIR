#ifndef DEMO_H
#define DEMO_H

#include <QMainWindow>

#include <QIODevice>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QTime>

#include "../qjfastfir.h"

namespace Ui {
class Demo;
}

class Generator : public QIODevice
{
    Q_OBJECT
public:
    Generator(QAudioFormat format,QObject *parent);
    ~Generator(){}
    void start(){open(QIODevice::ReadOnly);}
    void stop(){close();}
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len)
    {
        Q_UNUSED(data);
        Q_UNUSED(len);
        return 0;
    }
private:
    QVector<double> buf;
    QJFastFIRFilter *fastfir;
};

class Demo : public QMainWindow
{
    Q_OBJECT

public:
    explicit Demo(QWidget *parent = 0);
    ~Demo();

private:
    Ui::Demo *ui;
    Generator *generator;
    QTime timer;
    int nMilliseconds;
private slots:
};

#endif // DEMO_H
