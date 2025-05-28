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
    void pcChanged(int newPc);
    void addMemoryEntry(quint32 address, quint32 value);
    void clearMemory();

public slots:
    Q_INVOKABLE QVariant getRegister(int index);
    Q_INVOKABLE void sendCommand(const QString& command);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void resetPC();

private:
    RiscvCpu& cpu;
};
