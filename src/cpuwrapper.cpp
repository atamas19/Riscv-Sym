#include "cpuwrapper.h"
#include "instruction/Instruction.h"

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
#include <iostream>
void CpuWrapper::sendCommand(const QString &command)
{
    emit clearRegistersHighlights();
    InstructionOutput commandOutput;
    qDebug() << "Executing command:" << command;

    cpu.executeAsmCommand(command.toStdString(), commandOutput);

    emit logMessage(QString::fromStdString(commandOutput.consoleLog));
    commandOutput.exitCode = 0;
    if (commandOutput.exitCode == 0)
    {
        if (!commandOutput.modifiedRegisters.empty())
            for (const auto &reg : commandOutput.modifiedRegisters)
                emit registerChanged(reg);
        // if (!commandOutput.)
    }
}
