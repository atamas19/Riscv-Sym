#include "instruction/AssemblyCompiler.h"

#include "instruction/RTypeInstruction.h"
#include "instruction/UTypeInstruction.h"
#include "instruction/ITypeInstruction.h"
#include "instruction/STypeInstruction.h"
#include "instruction/BTypeInstruction.h"
#include "instruction/JTypeInstruction.h"

#include <unordered_map>
#include <cassert>

AssemblyCompiler& AssemblyCompiler::getInstance()
{
    static AssemblyCompiler instance;

    return instance;
}

uint32_t AssemblyCompiler::compile(const std::string& asmCode)
{
    std::istringstream stream(asmCode);
    auto& assemblyCompiler = AssemblyCompiler::getInstance();

    uint32_t encodedInstruction = assemblyCompiler.getInstruction(stream);

    if (encodedInstruction != 0)
    {
        // means stuff is right
        return encodedInstruction;
    }
    else
    {
        // means stuff is not right
        return 0;
    }
}
 // add x1, x2, x3 -> 01101110111011
 // addi x1, x2, 191
 // add x1, x2, 191(x3)
 // lw x1, 1
uint32_t AssemblyCompiler::getInstruction(std::istringstream &instruction)
{
    static const std::unordered_map<std::string, std::function<uint32_t(std::istringstream&)>> instructionMap = {
        {"add", [this](std::istringstream& ins) { return assembleAdd(ins); }}
    };
    std::string instructionName;
    instruction >> instructionName;

    auto it = instructionMap.find(instructionName);
    if (it != instructionMap.end())
        return it->second(instruction);
    else
        return 0;
}

uint32_t AssemblyCompiler::encodeRType(uint8_t funct7, uint8_t rs2, uint8_t rs1, uint8_t funct3, uint8_t rd, uint8_t opcode)
{
    assert(funct7 <= 0x7F && "funct7 must be 7 bits");
    assert(rs2    <= 0x1F && "rs2 must be 5 bits");
    assert(rs1    <= 0x1F && "rs1 must be 5 bits");
    assert(funct3 <= 0x07 && "funct3 must be 3 bits");
    assert(rd     <= 0x1F && "rd must be 5 bits");
    assert(opcode <= 0x7F && "opcode must be 7 bits");

    uint32_t encoded = 0;
    encoded |= (static_cast<uint32_t>(funct7) << 25);
    encoded |= (static_cast<uint32_t>(rs2)    << 20);
    encoded |= (static_cast<uint32_t>(rs1)    << 15);
    encoded |= (static_cast<uint32_t>(funct3) << 12);
    encoded |= (static_cast<uint32_t>(rd)     << 7);
    encoded |= static_cast<uint32_t>(opcode);

    return encoded;
}

uint32_t AssemblyCompiler::assembleAdd(std::istringstream& instruction)
{
    // TODO
    // Check if istringstream& is really the right type here, I should decode xy, xz, xw into real register numbers then pass everything into encodeRType
    // something like this
    // uint8_t rs1 = something();
    // uint8_t rs2 = something();
    // uint8_t rd = something();

    // return encodeRType(0x0,rs2,rs1,0x0,rd,0x33);
    // this is the way all functions should be handled
    return 0;
}