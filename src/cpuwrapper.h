#pragma once

#include <QObject>
#include <QVariant>
#include <QDebug>

#include "RiscvCpu.h"
#include "AssemblyCompiler.h"

class CpuWrapper : public QObject
{
    Q_OBJECT
public:
    explicit CpuWrapper(QObject *parent = nullptr);

    int test1();

signals:
    void registerEmitted(QVariant reg);
    void registersUpdated();

public slots:
    Q_INVOKABLE QVariant getRegister(int index);
    Q_INVOKABLE void sendCommand(const QString& command);

private:
    RiscvCpu& cpu;
};
