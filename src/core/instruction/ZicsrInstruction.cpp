#include <core/instruction/ZicsrInstruction.h>

#include <core/RiscvCpu.h>
#include <core/CsrUnit.h>

namespace Zicsr {

void Instruction::decode() {
    IType::Instruction::decode(); 

    csr_addr = static_cast<uint16_t>(imm & 0xFFF);
}

std::unique_ptr<Instruction> InstructionFactory::create(uint32_t encodedInstruction) {
    static const std::unordered_map<uint8_t, std::function<std::unique_ptr<Instruction>(uint32_t)>> instructionMap = {
        { CSRRW ::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRW> (ins); }},
        { CSRRS ::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRS> (ins); }},
        { CSRRC ::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRC> (ins); }},
        { CSRRWI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRWI>(ins); }},
        { CSRRSI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRSI>(ins); }},
        { CSRRCI::getInstructionDescriptor(), [](uint32_t ins) { return std::make_unique<CSRRCI>(ins); }}
    };

    uint8_t funct3 = getBits(encodedInstruction, 12, 14);

    auto it = instructionMap.find(funct3);
    if (it != instructionMap.end())
        return it->second(encodedInstruction);

    return nullptr;
}

void CSRRW::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t rs1_value = cpu.getRegister(rs1);

    if (rd != 0) {
        uint32_t old_value = csr.read(csr_addr);
        cpu.setRegister(rd, old_value);
    }

    csr.write(csr_addr, rs1_value);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRW: CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRS::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t old_val = csr.read(csr_addr);
    uint32_t rs1_val = cpu.getRegister(rs1);

    if (rs1 != 0) {
        csr.write(csr_addr, old_val | rs1_val);
    }
    
    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRS: Read/Set CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRC::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t old_val = csr.read(csr_addr);
    uint32_t rs1_val = cpu.getRegister(rs1);

    if (rs1 != 0) {
        csr.write(csr_addr, old_val & ~rs1_val);
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRC: Read/Clear CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRWI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t uimm = rs1;

    if (rd != 0) {
        uint32_t old_value = csr.read(csr_addr);
        cpu.setRegister(rd, old_value);
    }
    csr.write(csr_addr, uimm);

    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRWI: CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRSI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t old_val = csr.read(csr_addr);
    uint32_t uimm = rs1;

    if (uimm != 0) {
        csr.write(csr_addr, old_val | uimm);
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRSI: Read/Set CSR[0x" + std::to_string(csr_addr) + "]";
}

void CSRRCI::execute(RiscvCpu& cpu, InstructionOutput& instructionOutput) {
    CsrUnit& csr = cpu.getCsr();
    uint32_t old_val = csr.read(csr_addr);
    uint32_t uimm = rs1;

    if (uimm != 0) {
        csr.write(csr_addr, old_val & ~uimm);
    }

    cpu.setRegister(rd, old_val);
    cpu.setPc(cpu.getPc() + 4);

    instructionOutput.consoleLog = "CSRRCI: Read/Clear CSR[0x" + std::to_string(csr_addr) + "]";
}

} // namespace Zicsr
