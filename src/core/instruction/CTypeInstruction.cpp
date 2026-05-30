#include <core/instruction/CTypeInstruction.h>

#include <spdlog/spdlog.h>

namespace CType
{

inline uint32_t extractLen(uint16_t inst, int start, int length) {
    return (inst >> start) & ((1 << length) - 1);
}

inline uint32_t signExtend(uint32_t val, int bits) {
    if (val & (1 << (bits - 1))) {
        return val | (~0U << bits);
    }
    return val;
}

inline uint32_t buildR(uint32_t opcode, uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2) {
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}

inline uint32_t buildI(uint32_t opcode, uint32_t funct3, uint32_t rd, uint32_t rs1, uint32_t imm) {
    return ((imm & 0xFFF) << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}

inline uint32_t buildS(uint32_t opcode, uint32_t funct3, uint32_t rs1, uint32_t rs2, uint32_t imm) {
    uint32_t imm11_5 = (imm >> 5) & 0x7F;
    uint32_t imm4_0 = imm & 0x1F;
    return (imm11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm4_0 << 7) | opcode;
}

inline uint32_t buildB(uint32_t opcode, uint32_t funct3, uint32_t rs1, uint32_t rs2, uint32_t imm) {
    uint32_t bit12 = (imm >> 12) & 0x1;
    uint32_t bit11 = (imm >> 11) & 0x1;
    uint32_t bits10_5 = (imm >> 5) & 0x3F;
    uint32_t bits4_1 = (imm >> 1) & 0xF;
    return (bit12 << 31) | (bits10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (bits4_1 << 8) | (bit11 << 7) | opcode;
}

inline uint32_t buildJ(uint32_t opcode, uint32_t rd, uint32_t imm) {
    uint32_t bit20 = (imm >> 20) & 0x1;
    uint32_t bits19_12 = (imm >> 12) & 0xFF;
    uint32_t bit11 = (imm >> 11) & 0x1;
    uint32_t bits10_1 = (imm >> 1) & 0x3FF;
    return (bit20 << 31) | (bits10_1 << 21) | (bit11 << 20) | (bits19_12 << 12) | (rd << 7) | opcode;
}

inline uint32_t buildU(uint32_t opcode, uint32_t rd, uint32_t imm) {
    return (imm & 0xFFFFF000) | (rd << 7) | opcode;
}

uint32_t decompress(uint16_t inst) {
    if (inst == 0) return 0; // Illegal instruction

    uint8_t op = inst & 0x3; // Cadranul (0, 1 sau 2)
    uint8_t funct3 = extractLen(inst, 13, 3);

    auto getRvcRegister = [](uint16_t instruction, int startBit) -> uint32_t {
        return 8 + extractLen(instruction, startBit, 3);
    };

    switch (op) {
        // ==========================================
        // CADRAN 00 (Opcode 0x0)
        // ==========================================
        case 0x0: {
            switch (funct3) {
                case 0x0: { // C.ADDI4SPN -> ADDI rd', x2, imm
                    uint32_t rd_prime = getRvcRegister(inst, 2);
                    uint32_t imm = (extractLen(inst, 7, 4) << 6) | (extractLen(inst, 11, 2) << 4) | (extractLen(inst, 5, 1) << 3) | (extractLen(inst, 6, 1) << 2);
                    if (imm == 0) break;
                    return buildI(0x13, 0x0, rd_prime, 2, imm);
                }
                case 0x2: { // C.LW -> LW rd', imm(rs1')
                    uint32_t rd_prime = getRvcRegister(inst, 2);
                    uint32_t rs1_prime = getRvcRegister(inst, 7);
                    uint32_t imm = (extractLen(inst, 5, 1) << 6) | (extractLen(inst, 10, 3) << 3) | (extractLen(inst, 6, 1) << 2);
                    return buildI(0x03, 0x2, rd_prime, rs1_prime, imm);
                }
                case 0x6: { // C.SW -> SW rs2', imm(rs1')
                    uint32_t rs2_prime = getRvcRegister(inst, 2);
                    uint32_t rs1_prime = getRvcRegister(inst, 7);
                    uint32_t imm = (extractLen(inst, 5, 1) << 6) | (extractLen(inst, 10, 3) << 3) | (extractLen(inst, 6, 1) << 2);
                    return buildS(0x23, 0x2, rs1_prime, rs2_prime, imm);
                }
            }
            break;
        }

        // ==========================================
        // CADRAN 01 (Opcode 0x1)
        // ==========================================
        case 0x1: {
            switch (funct3) {
                case 0x0: { // C.ADDI / C.NOP -> ADDI rd, rd, imm
                    uint32_t rd = extractLen(inst, 7, 5);
                    uint32_t imm = signExtend((extractLen(inst, 12, 1) << 5) | extractLen(inst, 2, 5), 6);
                    if (rd == 0 && imm == 0) return buildI(0x13, 0x0, 0, 0, 0); // NOP curat
                    return buildI(0x13, 0x0, rd, rd, imm);
                }
                case 0x1: { // C.JAL -> JAL x1, imm
                    uint32_t imm = signExtend((extractLen(inst, 12, 1) << 11) | (extractLen(inst, 8, 1) << 10) |
                                              (extractLen(inst, 9, 2) << 8) | (extractLen(inst, 6, 1) << 7) |
                                              (extractLen(inst, 7, 1) << 6) | (extractLen(inst, 2, 1) << 5) |
                                              (extractLen(inst, 11, 1) << 4) | (extractLen(inst, 3, 3) << 1), 12);
                    return buildJ(0x6F, 1, imm);
                }
                case 0x2: { // C.LI -> ADDI rd, x0, imm
                    uint32_t rd = extractLen(inst, 7, 5);
                    if (rd == 0) break;
                    uint32_t imm = signExtend((extractLen(inst, 12, 1) << 5) | extractLen(inst, 2, 5), 6);
                    return buildI(0x13, 0x0, rd, 0, imm);
                }
                case 0x3: {
                    uint32_t rd = extractLen(inst, 7, 5);
                    uint32_t imm = signExtend((extractLen(inst, 12, 1) << 5) | extractLen(inst, 2, 5), 6);
                    if (rd == 2) { // C.ADDI16SP -> ADDI x2, x2, imm
                        uint32_t imm16 = signExtend((extractLen(inst, 12, 1) << 9) | (extractLen(inst, 3, 2) << 7) |
                                                    (extractLen(inst, 5, 1) << 6) | (extractLen(inst, 2, 1) << 5) |
                                                    (extractLen(inst, 6, 1) << 4), 10);
                        if (imm16 != 0) return buildI(0x13, 0x0, 2, 2, imm16);
                    } else if (rd != 0) { // C.LUI -> LUI rd, imm
                        if (imm != 0) return buildU(0x37, rd, imm << 12);
                    }
                    break;
                }
                case 0x4: { // Misc ALU
                    uint32_t funct2 = extractLen(inst, 10, 2);
                    uint32_t rd_prime = getRvcRegister(inst, 7);

                    if (funct2 == 0x3) { // C.AND / C.OR / C.XOR / C.SUB
                        uint32_t rs2_prime = getRvcRegister(inst, 2);
                        uint32_t funct2_sub = extractLen(inst, 5, 2);

                        if (funct2_sub == 0x0) return buildR(0x33, 0x0, 0x20, rd_prime, rd_prime, rs2_prime); // SUB
                        if (funct2_sub == 0x1) return buildR(0x33, 0x4, 0x00, rd_prime, rd_prime, rs2_prime); // XOR
                        if (funct2_sub == 0x2) return buildR(0x33, 0x6, 0x00, rd_prime, rd_prime, rs2_prime); // OR
                        if (funct2_sub == 0x3) return buildR(0x33, 0x7, 0x00, rd_prime, rd_prime, rs2_prime); // AND
                    } else { // C.SRLI / C.SRAI / C.ANDI
                        uint32_t imm = (extractLen(inst, 12, 1) << 5) | extractLen(inst, 2, 5);
                        if (funct2 == 0x0) return buildI(0x13, 0x5, rd_prime, rd_prime, imm);        // SRLI
                        if (funct2 == 0x1) return buildI(0x13, 0x5, rd_prime, rd_prime, imm | 0x400); // SRAI
                        if (funct2 == 0x2) return buildI(0x13, 0x7, rd_prime, rd_prime, signExtend(imm, 6)); // ANDI
                    }
                    break;
                }
                case 0x5: { // C.J -> JAL x0, imm
                    uint32_t imm = signExtend((extractLen(inst, 12, 1) << 11) | (extractLen(inst, 8, 1) << 10) |
                                              (extractLen(inst, 9, 2) << 8) | (extractLen(inst, 6, 1) << 7) |
                                              (extractLen(inst, 7, 1) << 6) | (extractLen(inst, 2, 1) << 5) |
                                              (extractLen(inst, 11, 1) << 4) | (extractLen(inst, 3, 3) << 1), 12);
                    return buildJ(0x6F, 0, imm);
                }
                case 0x6:   // C.BEQZ -> BEQ rs1', x0, imm
                case 0x7: { // C.BNEZ -> BNE rs1', x0, imm
                    uint32_t rs1_prime = getRvcRegister(inst, 7);
                    uint32_t imm = signExtend((extractLen(inst, 12, 1) << 8) | (extractLen(inst, 5, 2) << 6) |
                                              (extractLen(inst, 2, 1) << 5) | (extractLen(inst, 10, 2) << 3) |
                                              (extractLen(inst, 3, 2) << 1), 9);
                    uint32_t funct3_branch = (funct3 == 0x6) ? 0x0 : 0x1;
                    return buildB(0x63, funct3_branch, rs1_prime, 0, imm);
                }
            }
            break;
        }

        // ==========================================
        // CADRAN 10 (Opcode 0x2)
        // ==========================================
        case 0x2: {
            switch (funct3) {
                case 0x0: { // C.SLLI -> SLLI rd, rd, shamt
                    uint32_t rd = extractLen(inst, 7, 5);
                    uint32_t shamt = (extractLen(inst, 12, 1) << 5) | extractLen(inst, 2, 5);
                    if (rd != 0 && shamt != 0) return buildI(0x13, 0x1, rd, rd, shamt);
                    break;
                }
                case 0x2: { // C.LWSP -> LW rd, imm(x2)
                    uint32_t rd = extractLen(inst, 7, 5);
                    uint32_t imm = (extractLen(inst, 2, 2) << 6) | (extractLen(inst, 12, 1) << 5) | (extractLen(inst, 4, 3) << 2);
                    if (rd != 0) return buildI(0x03, 0x2, rd, 2, imm);
                    break;
                }
                case 0x4: { // C.JR / C.MV / C.EBREAK / C.JALR / C.ADD
                    uint32_t bit12 = extractLen(inst, 12, 1);
                    uint32_t rs1 = extractLen(inst, 7, 5);
                    uint32_t rs2 = extractLen(inst, 2, 5);

                    if (bit12 == 0) {
                        if (rs2 == 0 && rs1 != 0) return buildI(0x67, 0x0, 0, rs1, 0); // C.JR -> JALR x0, rs1, 0
                        if (rs2 != 0 && rs1 != 0) return buildR(0x33, 0x0, 0x0, rs1, 0, rs2); // C.MV -> ADD rd, x0, rs2
                    } else {
                        if (rs1 == 0 && rs2 == 0) return 0x00100073; // C.EBREAK -> EBREAK
                        if (rs2 == 0 && rs1 != 0) return buildI(0x67, 0x0, 1, rs1, 0); // C.JALR -> JALR x1, rs1, 0
                        if (rs2 != 0 && rs1 != 0) return buildR(0x33, 0x0, 0x0, rs1, rs1, rs2); // C.ADD -> ADD rd, rd, rs2
                    }
                    break;
                }
                case 0x6: { // C.SWSP -> SW rs2, imm(x2)
                    uint32_t rs2 = extractLen(inst, 2, 5);
                    uint32_t imm = (extractLen(inst, 7, 2) << 6) | (extractLen(inst, 9, 4) << 2);
                    return buildS(0x23, 0x2, 2, rs2, imm);
                }
            }
            break;
        }
    }

    spdlog::warn("Unimplemented or Invalid Compressed Instruction: 0x{:04X}", inst);
    return 0;
}

} // namespace CType
