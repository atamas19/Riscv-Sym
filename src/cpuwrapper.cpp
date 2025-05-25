#include "cpuwrapper.h"
#include "instruction/Instruction.h"

#include <numeric>

CpuWrapper::CpuWrapper(QObject *parent)
    : QObject{parent}, cpu(RiscvCpu::getInstance())
{
    cpu.setRegister(1, 10);
    cpu.setRegister(2, 20);
    cpu.setRegister(3, 30);
}

Q_INVOKABLE QVariant CpuWrapper::getRegister(int index)
{
    return QVariant(cpu.getRegister(index));
}

void CpuWrapper::sendCommand(const QString &command)
{
    emit clearRegistersHighlights();
    InstructionOutput commandOutput;
    qDebug() << "Executing command:" << command;

    int executionResult = cpu.executeAsmCommand(command.toStdString(), commandOutput);
    if (executionResult == -1)
    {

    }
    if (!commandOutput.consoleLog.empty())
        emit logMessage(QString::fromStdString(commandOutput.consoleLog));
    if (commandOutput.exitCode == 0)
    {
        if (!commandOutput.modifiedRegisters.empty())
        {
            QList<int> qList(commandOutput.modifiedRegisters.begin(), commandOutput.modifiedRegisters.end());
            emit registersChanged(qList);
        }
    }
}

void CpuWrapper::reset()
{
    cpu.reset();

    QVector<int> allIndices(32);
    std::iota(allIndices.begin(), allIndices.end(), 0);

    emit registersChanged(allIndices);
}
