#include "../memory.h"
#include "../assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>

// implement register 0x0 - 0x031
#define REGISTER_COUNT 32
// init register
uint32_t registers_data[REGISTER_COUNT] = {0}; 
uint32_t rd, r1, r2;

void decode_Rtype_instruction(uint32_t instruction, uint32_t *rd, uint32_t *r1, uint32_t *r2) {
    *rd = (instruction >> 7) & 0x1F;       // Extract bits 11:7 for rd
    *r1 = (instruction >> 15) & 0x1F;      // Extract bits 19:15 for r1
    *r2 = (instruction >> 20) & 0x1F;      // Extract bits 24:20 for r2
    //*imm = (instruction >> 20) & 0xFFF;    // Extract bits 31:20 for imm[11:0]
}
void identify_instruction(uint32_t instruction,struct memory *mem ) {
    uint32_t opcode = instruction & 0x7F;   // Extract the opcode (lowest 7 bits)
    uint32_t funct3 = (instruction >> 12) & 0x7;  // Extract the funct3 field (bits 12-14)
    uint32_t funct7 = (instruction >> 25) & 0x7F; // Extract the funct7 field (bits 25-32)
    uint32_t imm32_12 = (instruction >> 12) & 0x7F; // Extract the funct7 field (bits 12-32)
    rd = (instruction >> 7) & 0x1F;       // Extract bits 11:7 for rd
    r1 = (instruction >> 15) & 0x1F;      // Extract bits 19:15 for r1
    r2 = (instruction >> 20) & 0x1F;      // Extract bits 24:20 for r2
    printf("opcode : 0x%08X\n",opcode);
    printf("funct3: 0x%08X\n",funct3);
    printf("funct3: 0x%08X\n",funct7);
    printf("Input Instruction: 0x%08X\n", instruction);

    switch (opcode) {
        // R-Type M-extention
        case 0x03:
            // Retrive the addresse
            uint32_t imm32_20 = (instruction >> 20) & 0x7F;// Extract the funct7 field (bits 20-32)
            uint32_t adr = registers_data[imm32_20] + registers_data[r1];
            printf("Opcode: I-Type\n");
            switch (funct3)
            {
            case 0x0:
                // load the byte from memory
                uint32_t byte = memory_rd_w(mem, adr);
                // get the sign of the byte 
                uint32_t sign = (byte <0x07)* 0xff;
                /// write the byte into memory
                registers_data[rd] = sign + byte;
                printf("opcode:LB \n");
                printf("Result: %d\n", registers_data[rd]);
                break;
            case 0x1:
                // load the halfword from memory
                uint32_t halfword = memory_rd_w(mem, adr);
                // get the sign of the halfword 
                uint32_t sign = (halfword <0x0f)* 0xffff;
                /// write the halfword into memory
                registers_data[rd] = sign + halfword;
                printf("opcode: LH\n");
                printf("Result: %d\n", registers_data[rd]);
                break;
            case 0x2:
                // Load a Word from Memoery
                registers_data[rd] = memory_rd_w(mem, adr);
                printf("opcode: LW\n");
                printf("Result: %d\n", registers_data[rd]);
                break;
            case 0x4:
                // Load A Byte from memory Unsigned
                registers_data[rd] = memory_rd_w(mem, adr);
                printf("opcode:LBU \n");
                printf("Result: %d\n", registers_data[rd]);
                break;
            case 0x5:
                // Load Unsigned Halfword from memory
                registers_data[rd] = memory_rd_w(mem, adr);
                printf("opcode:LHU \n");
                printf("Result: %d\n", registers_data[rd]);
                break;
            default:
                printf("Unknown funct7 for L I-type\n");
                break;
            }
            break;
        case 0x13:
            printf("Opcode: I-Type \n");
            switch (funct3){
                case 0x0:
                    printf("opcode:ADDI \n");
                    break;
                case 0x1:
                    printf("opcode:SLLI \n");
                    break;
                case 0x2:
                    printf("opcode: SLTI\n");
                    break;
                case 0x3:
                    printf("opcode: SLTIU \n");
                    break;
                case 0x4:
                    printf("opcode:XORI \n");
                    break;
                case 0x5:
                    switch (funct7){
                    case 0x0:
                        printf("opcode:SRLI \n");
                        break;
                    case 0x20:
                        printf("opcode:SRAI \n");
                        break;
                    default:
                        printf("Unknown funct7 for SRLI/SRAI I-type\n");
                     break;
                    }
                    break;

                case 0x06:
                    printf("opcode:ORI \n");
                    break;
                case 0x7:
                    printf("opcode:ANDI \n");
                    break;
                default:
                    printf("Unknown funct7 for L I-type\n");
                    break;
            }
            break;
        case 0x23:
             printf("Opcode: S-Type\n");
            switch (funct3)
            {
            case 0x0:
                 printf("Opcode: SB \n");
                break;
            case 0x1:
                 printf("Opcode: SH \n");
                break;
            case 0x2:
                 printf("Opcode: SW \n");
                break;
            default:
                break;
            }
            break;
        case 0x33:
            printf("Opcode: R-Type\n");
            decode_Rtype_instruction(instruction,rd,r1,r2);
            switch (funct3) {
            case 0x0:
                switch (funct7) {
                    case 0x00:
                        registers_data[rd] = registers_data[r1] + registers_data[r2];
                        printf("Opcode: ADD (Addition)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x01:
                    registers_data[rd] = registers_data[r1] * registers_data[r2];
                        printf("Opcode: MUL (Multiply)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x20:
                        registers_data[rd] = registers_data[r1] - registers_data[r2];
                        printf("Opcode: SUB (Subtraction)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    default:
                        printf("Unknown funct7 for MUL/ADD/SUB\n");
                        break;
                }
                break;
            case 0x1:
                switch (funct7) {
                    case 0x00:
                        registers_data[rd] = registers_data[r1] << registers_data[r2];
                        printf("Opcode: SLL (Shift Left Logical)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x01:
                        registers_data[rd] = (registers_data[r1]*registers_data[r2]) >> 32;
                        printf("Opcode: MULH (Multiply High)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    default:
                        printf("Unknown funct7 for SLL/MULH\n");
                        break;
                }
                break;
            case 0x2:
                switch (funct7) {
                    case 0x00:
                        registers_data[rd] = (registers_data[r1] < registers_data[r2]) ? 1: 0;
                        printf("Opcode: SLT (Set Less Than)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x01:
                        registers_data[rd] = (registers_data[r1]*registers_data[r2]) >> 32;
                        printf("Opcode: MULHSU (Multiply High Signed Unsigned)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    default:
                        printf("Unknown funct7 for SLT/MULHSU\n");
                        break;
                }
                break;
            case 0x3:
                switch (funct7){
                    case 0x00:
                        registers_data[rd] = (registers_data[r1] < registers_data[r2]) ? 1: 0;
                        printf("Opcode: SLTU (Set Less Than Unsigned)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
            
                    case 0x01:
                        registers_data[rd] = (registers_data[r1]*registers_data[r2]) >> 32;
                        printf("Opcode: MULHU (Multiply High Signed Unsigned)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
            
                default:
                    printf("Unknown funct7 for SLTU/MULHU\n");
                    break;
                }
                break;
                
            case 0x4:
                switch (funct7) {
                    case 0x00:
                        registers_data[rd] = registers_data[r1] ^ registers_data[r2];
                        printf("Opcode: XOR (Bitwise XOR)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x01:
                        registers_data[rd] = registers_data[r1]/registers_data[r2];
                        printf("Opcode: DIV (Divide)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    default:
                        printf("Unknown funct7 for XOR/DIVIDE\n");
                        break;
                }
                break;
            case 0x5:
                switch (funct7) {
                    case 0x00:
                        registers_data[rd] = registers_data[r1] >> registers_data[r2];
                        printf("Opcode: SRL (Shift Right Logical)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x01:
                        registers_data[rd] = registers_data[r1]/registers_data[r2];
                        printf("Opcode: DIVU (Divide Unsigned)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x20:
                        registers_data[rd] = registers_data[r1] >> registers_data[r2];
                        printf("Opcode: SRA (Shift Right Arithmetic)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    default:
                        printf("Unknown funct7 for SRL/SRA\n");
                        break;
                }
                break;
            case 0x6:
                switch (funct7) {
                    case 0x00:
                        registers_data[rd] = registers_data[r1] | registers_data[r2];
                        printf("Opcode: OR (Bitwise OR)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    case 0x01:
                        registers_data[rd] = registers_data[r1]%registers_data[r2];
                        printf("Opcode: REM (Remainder)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;
                    default:
                        printf("Unknown funct7 for OR\n");
                        break;
                }
                break;
            case 0x7:
                switch (funct7) {
                    case 0x00:
                        registers_data[rd] = registers_data[r1] & registers_data[r2];
                        printf("Opcode: AND (Bitwise AND)\n");
                        break;
                    case 0x01:
                        registers_data[rd] = registers_data[r1]%registers_data[r2];
                        printf("Opcode: REMU (Remainder Unsigned)\n");
                        printf("Result: %d\n", registers_data[rd]);
                        break;    
                    default:
                        printf("Unknown funct7 for AND\n");
                        break;
                }
                break;
            // Add cases for other R-type instructions as needed
            default:
                printf("Unknown funct3 for R-type instruction\n");
                break;
        }
            break;
        // Add cases for other opcodes as needed
        case 0x37:
            // Store value in the 20 bits by shifting 12
            registers_data[rd] = imm32_12 << 12; 
            printf("Opcode: LUI (Load Upper Immediate)\n");
            printf("Result: %d\n", registers_data[rd]);
            break;
        // I - Type
        case 0x17:
            printf("Opcode: AUIPC (Add Upper Immediate to PC)\n");
            break;
        // S - Type
        case 0x6F:
            printf("Opcode: JAL (Jump and Link)\n");
            break;
        // U - Type    
        case 0x67:
            printf("Opcode: JALR (Jump and Link Register)\n");
            break;
        case 0x63:
            printf("Opcode: B-Type\n");
            switch (funct3) {
                case 0x0:
                    printf("Opcode: BEQ  \n");
                    break;
                case 0x1:
                    printf("Opcode: BNE \n");
                    break;
                case 0x4:
                    printf("Opcode: BLT \n");
                    break;
                case 0x5:
                    printf("Opcode: BGE \n");
                    break;
                case 0x6:
                    printf("Opcode: BLTU \n");
                    break;
                case 0x7:
                    printf("Opcode: BGEU \n");
                    break;
                default:
                    printf("Unknown funct3 for opcode B-Type\n");
                    break;
            }
            break;
        default:
            printf("Unknown opcode\n");
            break;
    }
}
// init
uint32_t generate_instruction(uint32_t opcode, uint32_t funct3, uint32_t funct7) {
    // Ensure that the opcode, funct3, and funct7 values are within the expected ranges
    if (opcode > 0x7F || funct3 > 0x7 || funct7 > 0x7F) {
        fprintf(stderr, "Error: Invalid opcode, funct3, or funct7 value\n");
        return 0;
    }

    // Construct the 32-bit instruction
    uint32_t instruction = (funct7 << 25) | (funct3 << 12) | opcode;

    return instruction;
}


uint32_t main(uint32_t argc, char *argv[]){
    if (argc != 1) {
        fprintf(stderr, "Usage: %s <instruction>\n", argv[0]);
        return 1;
    }
    uint32_t opcode = 0x13;
    uint32_t funct3 = 0;
    uint32_t instructions;
    for ( funct3 = 0; funct3 < 8; funct3++)
    {
        /* code */
        instructions = generate_instruction(opcode,funct3,0x0);
        identify_instruction(instructions);
        if (funct3 == 0x3)
        {
            uint32_t funct7 = 1 << 5;
            instructions = generate_instruction(opcode,funct3,funct7);
            identify_instruction(instructions);
        }
       
        
        
    }
    return 1;
    
}