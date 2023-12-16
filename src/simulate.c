#include "memory.h"
#include "assembly.h"
#include <stdio.h>
#include <simulate.h>
#include <stdint.h>

// We define all of the opcodes from the different instructions to implement
// by reading the opcode value in the tables p. 104 & 105 in the riscv spec.
// The values are in binary and we translate them to hex as they are shorter
// easier to read and use for comparisons.

// RV321 Base Instruction Set
// Opcodes
#define OPCODE_LUI 0x37
#define OPCODE_AUIPC 0x17
#define OPCODE_JAL 0x6F
#define OPCODE_JALR 0x67
#define OPCODE_BEQ_BNE_BLT_BGE_BLTU_BGEU 0x63
#define OPCODE_LB_LH_LW_LBU_LHU 0x3
#define OPCODE_SB_SH_SW 0x23
#define OPCODE_ADDI_SLTI_SLTIU_XORI_ORI_ANDI_SLLI_SRLI_SRAI 0x13
#define OPCODE_ADD_SUB_SLL_SLT_SLTU_XOR_SRL_SRA_OR_AND 0x33






// RV32M Standard Extension
// Opcodes
#define OPCODE_MUL_DIV_REM 0x33 // (They all share same value)

// Funct7
#define FUNCT7_MUL_DIV_REM 0x01 // (They all share same value)

// Funct3
#define FUNCT3_MUL     0x0
#define FUNCT3_MULH    0x1
#define FUNCT3_MULHSU  0x2
#define FUNCT3_MULHU   0x3
#define FUNCT3_DIV     0x4
#define FUNCT3_DIVU    0x5
#define FUNCT3_REM     0x6
#define FUNCT3_REMU    0x7

// 32 bit register as RISC-V is 32 bit
uint32_t registers[32];

// Writes a 32 bit value at specified index 
void write_register(int register_index, uint32_t value) {
    if (register_index != 0) { // // Register 0 is always 0.
        registers[register_index] = value;
    }
}

// Reads the register at specified index
uint32_t read_register(int register_index) {
    return registers[register_index];
}


long int simulate(struct memory *mem, struct assembly *as, int start_addr, FILE *log_file) {
    uint32_t pc = start_addr; // Program counter

    while (1) {
        int instruction = memory_rd_w(mem, pc);
        // Opcode is the last 7 bits in the instructions. See figure 2.2 & 2.4:
        // https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf
        int opcode = instruction & 0x7F;

       // RV321 Base Instruction Set
       // .....
       // Implement LUI
        if (opcode == OPCODE_LUI)
        {
            uint32_t imm32_12 = (instruction >> 12) & 0x7F; // Extract the funct7 field (bits 12-32)
            uint32_t rd = (instruction >> 7) & 0x1F;
            write_register(rd, imm32_12 << 12);
        }
        
        // Load Instructions
        if (opcode == OPCODE_LB_LH_LW_LBU_LHU)
        {
            // Retrive the addresse
            int funct3 = (instruction >> 12) & 0x7;
            int rs1 = (instruction >> 15) & 0x1F;
            int rd = (instruction >> 7) & 0x1F;
            uint32_t imm32_20 = (instruction >> 20) & 0x7F;// Extract the funct7 field (bits 20-32)
            uint32_t adr = read_register(imm32_20) + read_register(rs1);
            switch (funct3)
            {
            case 0x0:
                // load the byte from memory
                uint32_t byte = memory_rd_w(mem, adr);
                // get the sign of the byte 
                uint32_t sign = (byte <0x07)* 0xff;
                /// read_register the byte into memory
                write_register(rd,sign + byte);;
                break;
            case 0x1:
                // load the halfword from memory
                uint32_t halfword = memory_rd_w(mem, adr);
                // get the sign of the halfword 
                uint32_t sign = (halfword <0x0f)* 0xffff;
                /// read_register the halfword into memory
                write_register(rd, sign + halfword);
                break;
            case 0x2:
                // Load a Word from Memoery
                write_register(rd,memory_rd_w(mem, adr));
                break;
            case 0x4:
                // Load A Byte from memory Unsigned
                write_register(rd, memory_rd_w(mem, adr));
                break;
            case 0x5:
                // Load Unsigned Halfword from memory
                write_register(rd,memory_rd_w(mem, adr));
                break;
            default:
                break;
            }
        }
        


       // RV32M Standard Extension
        if (opcode == OPCODE_MUL_DIV_REM) {  
            // The instructions in this extension are all of R-type.
            uint32_t funct3 = (instruction >> 12) & 0x7;
            uint32_t funct7 = (instruction >> 25) & 0x7F;
            uint32_t rs1 = (instruction >> 15) & 0x1F;
            uint32_t rs2 = (instruction >> 20) & 0x1F;
            uint32_t rd = (instruction >> 7) & 0x1F;

            uint32_t rs1_value = read_register(rs1);
            uint32_t rs2_value = read_register(rs2);

            if (funct7 == FUNCT7_MUL_DIV_REM) {
                switch (funct3) {
                    // Following link has been used as help when implementing the instructions
                    // https://msyksphinz-self.github.io/riscv-isadoc/html/rvm.html
                    case FUNCT3_MUL: 
                        uint32_t mul = rs1_value * rs2_value;
                        write_register(rd, mul);
                        break;
                    case FUNCT3_MULH: 
                        int64_t mulh_rs1_value = (int64_t)(int32_t)rs1_value;
                        int64_t mulh_rs2_value = (int64_t)(int32_t)rs2_value;
                        int64_t mulh = mulh_rs1_value * mulh_rs2_value;
                        write_register(rd, (uint32_t)(mulh >> 32));
                        break;
                    case FUNCT3_MULHSU:
                        // Same as above but unsigned rs2
                        int64_t mulhsu_rs1_value = (int64_t)(int32_t)rs1_value; 
                        uint64_t mulhsu_rs2_value = (uint64_t)read_register(rs2_value);
                        int64_t mulhsu = mulhsu_rs1_value * (int64_t)mulhsu_rs2_value; 
                        uint32_t upper_bits_mulhsu = (uint32_t)(mulhsu >> 32); 
                        write_register(rd, upper_bits_mulhsu);
                        break;
                    case FUNCT3_DIV:
                        if (rs2_value == 0 ) {
                            printf("Division by 0");
                        }
                        else {
                           uint32_t div = rs1_value / rs2_value;
                           write_register(rd, div);
                        }
                        break;
                    case FUNCT3_DIVU:
                        uint32_t divu;
                        if (rs2_value == 0) {
                            divu = UINT32_MAX; // Division by zero gets largest unsigned value
                        } else {
                            divu = rs1_value / rs2_value; 
                        }
                        write_register(rd, divu);
                        break;
                    case FUNCT3_REM:
                        int32_t rem_rs1_value = (int32_t)rs1_value; 
                        int32_t rem_rs2_value = (int32_t)rs2_value; 

                        int32_t rem;
                        if (rem_rs2_value == 0) {
                            rem = rem_rs1_value; 
                        } else {
                            rem = rem_rs1_value % rem_rs2_value; 
                        }
                        write_register(rd, rem);
                        break;
                    case FUNCT3_REMU:
                        uint32_t remu;
                        if (rs2_value == 0) {
                            remu = rs1_value;
                        } else {
                            remu = rs1_value % rs2_value;
                        }
                        write_register(rd, remu);
                        break;
                }
            }
        }

        pc += 4; // Go to next instruction

    }

}
