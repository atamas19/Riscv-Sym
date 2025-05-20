#ifndef CPUWRAPPER_H
#define CPUWRAPPER_H

#include <QObject>
#include <QVariant>
#include <QDebug>

#include "RiscvCpu.h"

class CpuWrapper : public QObject
{
    Q_OBJECT
public:
    explicit CpuWrapper(QObject *parent = nullptr);

    int test1();

signals:
    void registerEmitted(QVariant reg);

public slots:
    Q_INVOKABLE QVariant getRegister(int index);

private:
    RiscvCpu& cpu;
};

#endif // CPUWRAPPER_H
