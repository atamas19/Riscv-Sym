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

signals:
    void registersChanged(const QList<int>& regIndices);
    void registersUpdated();
    void logMessage(const QString &message);
    void clearRegistersHighlights();

public slots:
    Q_INVOKABLE QVariant getRegister(int index);
    Q_INVOKABLE void sendCommand(const QString& command);
    Q_INVOKABLE void reset();

private:
    RiscvCpu& cpu;
};
