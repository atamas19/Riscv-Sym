#include "cpuwrapper.h"

CpuWrapper::CpuWrapper(QObject *parent)
    : QObject{parent}, cpu(RiscvCpu::getInstance())
{
    cpu.setRegister(1, 10);
    cpu.setRegister(2, 20);
    cpu.setRegister(3, 30);
}

int CpuWrapper::test1()
{
    return 10;
}

Q_INVOKABLE QVariant CpuWrapper::getRegister(int index)
{
    return QVariant(cpu.getRegister(index));
}
