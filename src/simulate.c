#include "memory.h"
#include "assembly.h"
#include <stdio.h>
#include "simulate.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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
    long int instructions = 0;
    (void)as; 

    while (1) {
        int instruction = memory_rd_w(mem, pc);
        // Opcode is the last 7 bits in the instructions. See figure 2.2 & 2.4:
        // https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf
        int opcode = instruction & 0x7F;

        // ecall
        if (opcode == 0x73) {
            int a7 = read_register(17); 
            switch(a7) {
                case 1:
                    ;
                    int input_char = getchar();
                    write_register(10, input_char);
                    break;
                case 2: 
                    ;
                    int output_char = read_register(10);
                    putchar(output_char);
                    break;
                case 3:
                    printf("Quitting simulation. Ran %ld instructions\n ", instructions);
                    exit(0);
                    break;
                case 93:
                    printf("Quitting simulation. Ran %ld instructions\n ", instructions);
                    exit(0);
                    break;
                default:
                    printf("Problem with system call A7 = %d \n", a7);
                    exit(-1);
            }
        }
       
        // Implement LUI
        if (opcode == OPCODE_LUI)
        {
            uint32_t imm32_12 = (instruction >> 12) & 0xFFFFF; // Extract the imm field (bits 12-32)
            uint32_t rd = (instruction >> 7) & 0x1F;
            write_register(rd, imm32_12 << 12);
        }

        if (opcode == OPCODE_AUIPC)
        {
            uint32_t imm32_12 = (instruction >> 12) & 0xFFFFF; // Extract the imm field (bits 12-32)
            uint32_t rd = (instruction >> 7) & 0x1F;
            /// Store offset + pc to rd
            write_register(rd,(imm32_12 << 12)+pc);
        }
        

        // JAL
        if (opcode == OPCODE_JAL) {
            uint32_t rd = (instruction >> 7) & 0x1F;
            int32_t imm = ((instruction & 0x80000000) >> 11) | // imm[20]
                        ((instruction & 0x7FE00000) >> 20) | // imm[10:1]
                        ((instruction & 0x00100000) >> 9) |  // imm[11]
                        (instruction & 0x000FF000);          // imm[19:12]
            if (imm & 0x100000) { // sign extend 
                imm |= 0xFFE00000;
            }
            write_register(rd, pc + 4);
            pc += imm;
            continue; // Skip the normal increment
        }
        // JALR
        if (opcode == OPCODE_JALR) {
            uint32_t rd = (instruction >> 7) & 0x1F;
            uint32_t rs1 = (instruction >> 15) & 0x1F;
            int32_t imm = (instruction >> 20) & 0xFFF;
            if (imm & 0x800) { // sign extend
                imm |= 0xFFFFF000;
            }
            write_register(rd, pc + 4);
            pc = (read_register(rs1) + imm) & ~1U; // Clear the least significant bit
            continue; // Skip the normal increment
        }


        // BEQ
        if (opcode == OPCODE_BEQ_BNE_BLT_BGE_BLTU_BGEU) {
            uint32_t funct3 = (instruction >> 12) & 0x7;
            uint32_t rs1 = (instruction >> 15) & 0x1F;
            uint32_t rs2 = (instruction >> 20) & 0x1F;
            int32_t imm = ((instruction & 0x80000000) >> 19) | // imm[12]
                        ((instruction & 0x7E000000) >> 20) | // imm[10:5]
                        ((instruction & 0x00000F00) >> 7) |  // imm[4:1]
                        ((instruction & 0x00000080) << 4);   // imm[11]
            if (imm & 0x1000) { // Sign extend 
                imm |= 0xFFFFE000;
            }
            bool take_branch = false;
            switch (funct3) {
                case 0x0: // BEQ
                    take_branch = read_register(rs1) == read_register(rs2);
                    break;
                case 0x1: // BNE
                    take_branch = read_register(rs1) != read_register(rs2);
                    break;
                case 0x4: // BLT
                    take_branch = (int32_t)read_register(rs1) < (int32_t)read_register(rs2);
                    break;
                case 0x5: // BGE
                    take_branch = (int32_t)read_register(rs1) >= (int32_t)read_register(rs2);
                    break;
                case 0x6: // BLTU
                    take_branch = read_register(rs1) < read_register(rs2);
                    break;
                case 0x7: // BGEU
                    take_branch = read_register(rs1) >= read_register(rs2);
                    break;
            }
            if (take_branch) {
                pc += imm;
                continue; // Skip normal PC increment
            }
        }

        // Load Instructions
        if (opcode == OPCODE_LB_LH_LW_LBU_LHU)
        {
            // Retrive the addresse
            int funct3 = (instruction >> 12) & 0x7;
            int rs1 = (instruction >> 15) & 0x1F;
            int rd = (instruction >> 7) & 0x1F;
            uint32_t imm32_20 = (instruction >> 20) & 0x7F;// Extract the funct7 field (bits 20-32)
            uint32_t adr = imm32_20 + read_register(rs1);
            switch (funct3) {
                case 0x0: // LB
                    ;
                    // load the byte from memory
                    uint32_t byte = memory_rd_w(mem, adr);
                    // get the sign of the byte 
                    uint32_t sign_byte = (byte <0x07)* 0xff;
                    /// read_register the byte into memory
                    write_register(rd,sign_byte + byte);;
                    break;
                case 0x1: // LH
                    ;
                    // load the halfword from memory
                    uint32_t halfword = memory_rd_w(mem, adr);
                    // get the sign of the halfword 
                    uint32_t sign_halfword = (halfword <0x0f)* 0xffff;
                    /// read_register the halfword into memory
                    write_register(rd, sign_halfword + halfword);
                    break;
                case 0x2: // LW
                    // Load a Word from Memoery
                    write_register(rd,memory_rd_w(mem, adr));
                    break;
                case 0x4: // LBU
                    // Load A Byte from memory Unsigned
                    write_register(rd, memory_rd_w(mem, adr));
                    break;
                case 0x5: // LHU
                    // Load Unsigned Halfword from memory
                    write_register(rd,memory_rd_w(mem, adr));
                    break;
                default:
                    break;
                }
        }

        // Store Instructions
        if (opcode == OPCODE_SB_SH_SW)
        {
            uint32_t imm4_0 = (instruction >> 7) & 0x1F; // Extract the imm4_0 field (bits 7-11)
            uint32_t imm32_25 = (instruction >> 25) & 0x7F; // Extract the funct7 field (bits 25-32)
            uint32_t funct3 = (instruction >> 12) & 0x7;     // Extract the funct3 field (bits 12-14)
            uint32_t rs1 = (instruction >> 15) & 0x1F;       // Extract bits 19:15
            uint32_t rs2 = (instruction >> 20) & 0x1F; // Extract bits 24:20
            uint32_t adr = read_register(rs1) +imm32_25 +imm4_0; // get memory location and add offset
            switch (funct3) {
            case 0x0: // SB
                memory_wr_b(mem,adr, read_register(rs2));
                // printf("Opcode: SB \n");
                break;
            case 0x1: // SH
                memory_wr_h(mem,adr, read_register(rs2));
                // printf("Opcode: SH \n");
                break;
            case 0x2: // SW
                memory_wr_w(mem,adr, read_register(rs2));
                // printf("Opcode: SW \n");
                break;
            default:
                break;
            }
            break;
        }
       
    

        if (opcode == OPCODE_ADDI_SLTI_SLTIU_XORI_ORI_ANDI_SLLI_SRLI_SRAI) {
            uint32_t funct3 = (instruction >> 12) & 0x7;
            uint32_t rs1 = (instruction >> 15) & 0x1F;
            uint32_t rd = (instruction >> 7) & 0x1F;
            int32_t imm = ((instruction >> 20) & 0xFFF);
            if (imm & 0x800) { // sign extend 
                imm |= 0xFFFFF000;
            }

            switch (funct3) {
                case 0x0: // ADDI
                    write_register(rd, read_register(rs1) + imm);
                    break;
                case 0x2: // SLTI
                    write_register(rd, (int32_t)read_register(rs1) < imm ? 1 : 0);
                    break;
                case 0x3: // SLTIU
                    write_register(rd, read_register(rs1) < (uint32_t)imm ? 1 : 0);
                    break;
                case 0x4: // XORI
                    write_register(rd, read_register(rs1) ^ imm);
                    break;
                case 0x6: // ORI
                    write_register(rd, read_register(rs1) | imm);
                    break;
                case 0x7: // ANDI
                    write_register(rd, read_register(rs1) & imm);
                    break;
                case 0x1: // SLLI
                    write_register(rd, read_register(rs1) << (imm & 0x1F));
                    break;
                case 0x5: // SRLI / SRAI
                    if ((instruction >> 30) & 0x1) { // SRAI
                        write_register(rd, (int32_t)read_register(rs1) >> (imm & 0x1F));
                    } else { // SRLI
                        write_register(rd, read_register(rs1) >> (imm & 0x1F));
                    }
                    break;
            }

        if (opcode == OPCODE_ADD_SUB_SLL_SLT_SLTU_XOR_SRL_SRA_OR_AND) {
            uint32_t funct3 = (instruction >> 12) & 0x7;
            uint32_t funct7 = (instruction >> 25) & 0x7F;
            uint32_t rs1 = (instruction >> 15) & 0x1F;
            uint32_t rs2 = (instruction >> 20) & 0x1F;
            uint32_t rd = (instruction >> 7) & 0x1F;

            switch (funct3) {
                case 0x0: // ADD / SUB
                    if (funct7 == 0x20) {
                        // SUB
                        int64_t sub = (int64_t)read_register(rs1) - (int64_t)read_register(rs2);
                        write_register(rd, (uint32_t)sub);

                    } else {
                        // ADD
                        int64_t add = (int64_t)read_register(rs1) + (int64_t)read_register(rs2);
                        write_register(rd, (uint32_t)add);
                    }
                    break;
                case 0x1: // SLL
                    ;
                    int64_t sll = (int64_t)read_register(rs1) << (read_register(rs2) & 0x1F);
                    write_register(rd, (uint32_t)sll);
                    break;
                case 0x2: // SLT
                    ;
                    int32_t slt = ((int32_t)read_register(rs1) < (int32_t)read_register(rs2)) ? 1 : 0;
                    write_register(rd, (uint32_t)slt);
                    break;
                case 0x3: // SLTU 
                    ;
                    uint32_t sltu = (read_register(rs1) < read_register(rs2)) ? 1 : 0;
                    write_register(rd, sltu);
                    break;
                case 0x4: // XOR 
                    ;
                    uint32_t xor = read_register(rs1) ^ read_register(rs2);
                    write_register(rd, xor);
                    break;
                case 0x5: // SRL / SRA 
                    if (funct7 == 0x20) {
                        // SRA
                        int32_t sra = (int32_t)read_register(rs1) >> (read_register(rs2) & 0x1F);
                        write_register(rd, (uint32_t)sra);
                    } else {
                        // SRL
                        uint32_t srl = read_register(rs1) >> (read_register(rs2) & 0x1F);
                        write_register(rd, srl);
                    }
                    break;
                case 0x6: // OR 
                    ;
                    uint32_t or = read_register(rs1) | read_register(rs2);
                    write_register(rd, or);
                    break;
                case 0x7: // AND 
                    ;
                    uint32_t and = read_register(rs1) & read_register(rs2);
                    write_register(rd, and);
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
                    case 0x0: // MUL
                        ;
                        uint32_t mul = rs1_value * rs2_value;
                        write_register(rd, mul);
                        break;
                    case 0x1: // MULH
                        ;
                        int64_t mulh_rs1_value = (int64_t)(int32_t)rs1_value;
                        int64_t mulh_rs2_value = (int64_t)(int32_t)rs2_value;
                        int64_t mulh = mulh_rs1_value * mulh_rs2_value;
                        write_register(rd, (uint32_t)(mulh >> 32));
                        break;
                    case 0x2: // MULHSU
                        ;
                        // Same as above but unsigned rs2
                        int64_t mulhsu_rs1_value = (int64_t)(int32_t)rs1_value; 
                        uint64_t mulhsu_rs2_value = (uint64_t)read_register(rs2_value);
                        int64_t mulhsu = mulhsu_rs1_value * (int64_t)mulhsu_rs2_value; 
                        uint32_t upper_bits_mulhsu = (uint32_t)(mulhsu >> 32); 
                        write_register(rd, upper_bits_mulhsu);
                        break;
                    case 0x3: // MULHU
                        ;
                        uint64_t mulhu_rs1_value = (uint64_t)read_register(rs1);
                        uint64_t mulhu_rs2_value = (uint64_t)read_register(rs2);
                        uint64_t mulhu = mulhu_rs1_value * mulhu_rs2_value;
                        write_register(rd, (uint32_t)(mulhu >> 32));
                        break;
                    case 0x4: // DIV
                        if (rs2_value == 0 ) {
                            printf("Division by 0");
                        }
                        else {
                           uint32_t div = rs1_value / rs2_value;
                           write_register(rd, div);
                        }
                        break;
                    case 0x5: // DIVU
                        ;
                        uint32_t divu;
                        if (rs2_value == 0) {
                            divu = UINT32_MAX; // Division by zero gets largest unsigned value
                        } else {
                            divu = rs1_value / rs2_value; 
                        }
                        write_register(rd, divu);
                        break;
                    case 0x6: // REM
                        ;
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
                    case 0x7: // REMU
                        ;
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
    return 0;
}
