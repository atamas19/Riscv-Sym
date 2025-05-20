#pragma once

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
    Q_INVOKABLE void sendCommand(const QString& command);

private:
    RiscvCpu& cpu;
};
