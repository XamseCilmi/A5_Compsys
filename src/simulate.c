
#include "memory.h"
#include "assembly.h"
#include <stdio.h>
#include <simulate.h>

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

long int simulate(struct memory *mem, struct assembly *as, int start_addr, FILE *log_file) {
    int pc = start_addr; // Program counter

    while (1) {
        int instruction = memory_rd_w(mem, pc);
        // Opcode is the last 7 bits in the instructions. See figure 2.2 & 2.4:
        // https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf
        int opcode = instruction & 0x7F;

       // RV321 Base Instruction Set
       // .....



       // RV32M Standard Extension
        if (opcode == OPCODE_MUL_DIV_REM) {  
            // The instructions in this extension are all of R-type.
            int funct3 = (instruction >> 12) & 0x7;
            int funct7 = (instruction >> 25) & 0x7F;
            int rs1 = (instruction >> 15) & 0x1F;
            int rs2 = (instruction >> 20) & 0x1F;
            int rd = (instruction >> 7) & 0x1F;
            
            if (funct7 == FUNCT7_MUL_DIV_REM) {
                switch (funct3) {
                    case FUNCT3_MUL:
                        write_register(rd, read_register(rs1) * read_register(rs2));
                        if (log_file) {
                            // fprintf();
                        }
                        break;
                    case FUNCT3_MULH:
                        
                        break;
                    case FUNCT3_MULHSU
                        break;
                    case FUNCT3_DIV
                        break;
                    case FUNCT3_DIVU
                        break;
                    case FUNCT3_REM
                        break;
                    case FUNCT3_REMU
                        break;
                }
            }
        }

        pc += 4; // Go to next instruction

    }

}
