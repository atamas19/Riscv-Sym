#include "cpuwrapper.h"
#include <core/instruction/Instruction.h>

#include <numeric>

CpuWrapper::CpuWrapper(QObject *parent)
    : QObject{parent}, cpu(RiscvCpu::getInstance())
{
    cpu.resetGUI();
}

Q_INVOKABLE QVariant CpuWrapper::getRegister(int index)
{
    return QVariant(cpu.getRegister(index));
}

bool CpuWrapper::sendCommand(const QString &command)
{
    emit clearRegistersHighlights();
    InstructionOutput commandOutput;
    qDebug() << "Executing command:" << command;

    int executionResult = cpu.executeAsmCommand(command.toStdString(), commandOutput);
    if (!commandOutput.consoleLog.empty())
        emit logMessage(QString::fromStdString(commandOutput.consoleLog));
    if (commandOutput.exitCode == 0)
    {
        if (!commandOutput.modifiedRegisters.empty())
        {
            QList<int> qList(commandOutput.modifiedRegisters.begin(), commandOutput.modifiedRegisters.end());
            emit registersChanged(qList);
        }
        if (!commandOutput.modifiedRamAddresses.empty())
        {
            for (const auto& memoryCell : commandOutput.modifiedRamAddresses)
            {
                emit addMemoryEntry(memoryCell.address, memoryCell.value);
            }
        }
        emit pcChanged(cpu.getPc());
        return true;
    }
    else
        return false;
}

void CpuWrapper::reset()
{
    cpu.resetGUI();

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
