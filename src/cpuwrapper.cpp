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
    // Just for testing purposes
    // TODO: delete once the memory is linked with the rv32i commands
    static uint32_t address = 0;
    static uint32_t value = 1;

    emit addMemoryEntry(address++, value++);


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
    emit pcChanged(cpu.getPc());
}

void CpuWrapper::reset()
{
    cpu.reset();

    QVector<int> allIndices(32);
    std::iota(allIndices.begin(), allIndices.end(), 0);

    emit registersChanged(allIndices);
    emit pcChanged(cpu.getPc());
    emit clearMemory();
}

void CpuWrapper::resetPC()
{
    cpu.setPc(0);

    emit pcChanged(cpu.getPc());
}
