import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#121212"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 32
            spacing: 16

            Label {
                text: "RISC-V Instructions"
                font.pixelSize: 32
                font.bold: true
                color: "#ffffff"
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12
                model: ListModel {
                    // R-Type Instructions
                    ListElement { name: "ADD"; fullName: "Add"; format: "add rd, rs1, rs2"; description: "Adds the values of rs1 and rs2, stores the result in rd." }
                    ListElement { name: "SUB"; fullName: "Subtract"; format: "sub rd, rs1, rs2"; description: "Subtracts the value of rs2 from rs1, stores the result in rd." }
                    ListElement { name: "SLL"; fullName: "Shift Left Logical"; format: "sll rd, rs1, rs2"; description: "Shifts the value in rs1 left by the value in the lower 5 bits of rs2, stores the result in rd." }
                    ListElement { name: "SLT"; fullName: "Set Less Than"; format: "slt rd, rs1, rs2"; description: "Sets rd to 1 if the value in rs1 is less than the value in rs2, else 0." }
                    ListElement { name: "SLTU"; fullName: "Set Less Than Unsigned"; format: "sltu rd, rs1, rs2"; description: "Sets rd to 1 if the value in rs1, treated as unsigned, is less than the value in rs2, else 0." }
                    ListElement { name: "XOR"; fullName: "Exclusive OR"; format: "xor rd, rs1, rs2"; description: "Bitwise XOR of the values in rs1 and rs2, stores the result in rd." }
                    ListElement { name: "SRL"; fullName: "Shift Right Logical"; format: "srl rd, rs1, rs2"; description: "Shifts the value in rs1 right logically by the value in the lower 5 bits of rs2, stores the result in rd." }
                    ListElement { name: "SRA"; fullName: "Shift Right Arithmetic"; format: "sra rd, rs1, rs2"; description: "Shifts the value in rs1 right arithmetically by the value in the lower 5 bits of rs2, stores the result in rd." }
                    ListElement { name: "OR"; fullName: "OR"; format: "or rd, rs1, rs2"; description: "Bitwise OR of the values in rs1 and rs2, stores the result in rd." }
                    ListElement { name: "AND"; fullName: "AND"; format: "and rd, rs1, rs2"; description: "Bitwise AND of the values in rs1 and rs2, stores the result in rd." }

                    // I-Type Instructions
                    ListElement { name: "ADDI"; fullName: "Add Immediate"; format: "addi rd, rs1, imm"; description: "Adds the immediate value to the value of rs1, stores the result in rd." }
                    ListElement { name: "SLTI"; fullName: "Set Less Than Immediate"; format: "slti rd, rs1, imm"; description: "Sets rd to 1 if the value in rs1 is less than the immediate value, else 0." }
                    ListElement { name: "SLTIU"; fullName: "Set Less Than Immediate Unsigned"; format: "sltiu rd, rs1, imm"; description: "Sets rd to 1 if the value in rs1 (unsigned) is less than the immediate value (unsigned), else 0." }
                    ListElement { name: "XORI"; fullName: "XOR Immediate"; format: "xori rd, rs1, imm"; description: "Bitwise XOR of the value in rs1 and the immediate value, stores the result in rd." }
                    ListElement { name: "ORI"; fullName: "OR Immediate"; format: "ori rd, rs1, imm"; description: "Bitwise OR of the value in rs1 and the immediate, stores the result in rd." }
                    ListElement { name: "ANDI"; fullName: "AND Immediate"; format: "andi rd, rs1, imm"; description: "Bitwise AND of the value in rs1 and the immediate value, stores the result in rd." }
                    ListElement { name: "SLLI"; fullName: "Shift Left Logical Immediate"; format: "slli rd, rs1, shamt"; description: "Shifts the value in rs1 left by the shamt amount (lower 5 bits), stores the result in rd." }
                    ListElement { name: "SRLI"; fullName: "Shift Right Logical Immediate"; format: "srli rd, rs1, shamt"; description: "Shifts the value in rs1 right logically by the shamt amount (lower 5 bits), stores the result in rd." }
                    ListElement { name: "SRAI"; fullName: "Shift Right Arithmetic Immediate"; format: "srai rd, rs1, shamt"; description: "Shifts the value in rs1 right arithmetically by the shamt amount (lower 5 bits), stores the result in rd." }
                    ListElement { name: "LB"; fullName: "Load Byte"; format: "lb rd, imm(rs1)"; description: "Loads a byte from memory at rs1 value + immediate value, sign-extends it to 32 bits, and stores it in rd." }
                    ListElement { name: "LH"; fullName: "Load Halfword"; format: "lh rd, imm(rs1)"; description: "Loads 2 bytes from memory at rs1 value + immediate value, sign-extends to 32 bits, and stores in rd." }
                    ListElement { name: "LW"; fullName: "Load Word"; format: "lw rd, imm(rs1)"; description: "Loads 4 bytes (a word) from memory at rs1 value + immediate value and stores it in rd." }
                    ListElement { name: "LBU"; fullName: "Load Byte Unsigned"; format: "lbu rd, imm(rs1)"; description: "Loads a byte from memory at rs1 value + immediate value, zero-extends it to 32 bits, and stores it in rd." }
                    ListElement { name: "LHU"; fullName: "Load Halfword Unsigned"; format: "lhu rd, imm(rs1)"; description: "Loads 2 bytes from memory at rs1 value + immediate value, zero-extends to 32 bits, and stores in rd." }
                    ListElement { name: "JALR"; fullName: "Jump and Link Register"; format: "jalr rd, imm(rs1)"; description: "Jumps to the address rs1 value + immediate value (with LSB cleared), stores the return address (PC + 4) in rd." }

                    // S-Type Instructions
                    ListElement { name: "SB"; fullName: "Store Byte"; format: "sb rs2, imm(rs1)"; description: "Stores the least significant byte of rs2 into memory at address rs1 value + immediate value." }
                    ListElement { name: "SH"; fullName: "Store Halfword"; format: "sh rs2, imm(rs1)"; description: "Stores the least significant halfword (2 bytes) of rs2 into memory at rs1 value + immediate value." }
                    ListElement { name: "SW"; fullName: "Store Word"; format: "sw rs2, imm(rs1)"; description: "Stores all 4 bytes of rs2 into memory at address rs1 value + immediate value." }

                    // B-Type Instructions
                    ListElement { name: "BEQ"; fullName: "Branch if Equal"; format: "beq rs1, rs2, imm"; description: "Branches to PC + immediate value if the values in rs1 and rs2 are equal." }
                    ListElement { name: "BNE"; fullName: "Branch if Not Equal"; format: "bne rs1, rs2, imm"; description: "Branches to PC + immediate value if the values in rs1 and rs2 are not equal." }
                    ListElement { name: "BLT"; fullName: "Branch if Less Than"; format: "blt rs1, rs2, imm"; description: "Branches to PC + immediate value if the value in rs1 is less than the value in rs2." }
                    ListElement { name: "BGE"; fullName: "Branch if Greater or Equal"; format: "bge rs1, rs2, imm"; description: "Branches to PC + immediate value if the value in rs1 is greater than or equal to the value inrs2." }
                    ListElement { name: "BLTU"; fullName: "Branch if Less Than Unsigned"; format: "bltu rs1, rs2, imm"; description: "Branches to PC + immediate value if the value in rs1 is less than rs2 (unsigned comparison)." }
                    ListElement { name: "BGEU"; fullName: "Branch if Greater or Equal Unsigned"; format: "bgeu rs1, rs2, imm"; description: "Branches to PC + immediate value if the value in rs1 is greater than or equal to rs2 (unsigned comparison)." }

                    // U-Type Instructions
                    ListElement { name: "LUI"; fullName: "Load Upper Immediate"; format: "lui rd, imm"; description: "Loads the 20-bit immediate into the upper 20 bits of rd. The lower 12 bits are set to 0." }
                    ListElement { name: "AUIPC"; fullName: "Add Upper Immediate to PC"; format: "auipc rd, imm"; description: "Adds the 20-bit immediate (shifted left by 12) to the current PC, stores the result in rd." }

                    // J-Type Instruction
                    ListElement { name: "JAL"; fullName: "Jump and Link"; format: "jal rd, imm"; description: "Jumps to PC + immediate value and stores the return address (PC + 4) in rd." }
                }

                delegate: InstructionCard {
                    instructionName: name
                    instructionFullName: fullName
                    instructionFormat: format
                    instructionDescription: description
                }
            }
        }
    }
}
