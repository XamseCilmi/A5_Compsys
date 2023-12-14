#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
void identify_instruction(int instruction) {
    int opcode = instruction & 0x7F;   // Extract the opcode (lowest 7 bits)
    int funct3 = (instruction >> 12) & 0x7;  // Extract the funct3 field (bits 12-14)
    int funct7 = (instruction >> 25) & 0x7F; // Extract the funct7 field (bits 25-32)
    printf("opcode : 0x%08X\n",opcode);
    printf("funct3: 0x%08X\n",funct3);
    printf("funct3: 0x%08X\n",funct7);
    printf("Input Instruction: 0x%08X\n", instruction);

    switch (opcode) {
        // R-Type M-extention
        case 0x33:
            printf("Opcode: R-Type\n");
            switch (funct3) {
            case 0x0:
                switch (funct7) {
                    case 0x00:
                        printf("Opcode: ADD (Addition)\n");
                        break;
                    case 0x01:
                        printf("Opcode: MUL (Multiply)\n");
                        break;
                    case 0x20:
                        printf("Opcode: SUB (Subtraction)\n");
                        break;
                    default:
                        printf("Unknown funct7 for MUL/ADD/SUB\n");
                        break;
                }
                break;
            case 0x1:
                switch (funct7) {
                    case 0x00:
                        printf("Opcode: SLL (Shift Left Logical)\n");
                        break;
                    case 0x01:
                        printf("Opcode: MULH (Multiply High)\n");
                        break;
                    default:
                        printf("Unknown funct7 for SLL/MULH\n");
                        break;
                }
                break;
            case 0x2:
                switch (funct7) {
                    case 0x00:
                        printf("Opcode: SLT (Set Less Than)\n");
                        break;
                    case 0x01:
                        printf("Opcode: MULHSU (Multiply High Signed Unsigned)\n");
                        break;
                    default:
                        printf("Unknown funct7 for SLT/MULHSU\n");
                        break;
                }
                break;
            case 0x3:
                switch (funct7){
                    case 0x00:
                        printf("Opcode: SLTU (Set Less Than Unsigned)\n");
                        break;
            
                    case 0x01:
                        printf("Opcode: MULHU (Multiply High Signed Unsigned)\n");
                        break;
            
                default:
                    printf("Unknown funct7 for SLTU/MULHU\n");
                    break;
                }
                break;
                
            case 0x4:
                switch (funct7) {
                    case 0x00:
                        printf("Opcode: XOR (Bitwise XOR)\n");
                        break;
                    case 0x01:
                        printf("Opcode: DIV (Divide)\n");
                        break;
                    default:
                        printf("Unknown funct7 for XOR/DIVIDE\n");
                        break;
                }
                break;
            case 0x5:
                switch (funct7) {
                    case 0x00:
                        printf("Opcode: SRL (Shift Right Logical)\n");
                        break;
                    case 0x01:
                        printf("Opcode: DIVU (Divide Unsigned)\n");
                        break;
                    case 0x20:
                        printf("Opcode: SRA (Shift Right Arithmetic)\n");
                        break;
                    default:
                        printf("Unknown funct7 for SRL/SRA\n");
                        break;
                }
                break;
            case 0x6:
                switch (funct7) {
                    case 0x00:
                        printf("Opcode: OR (Bitwise OR)\n");
                        break;
                    case 0x01:
                        printf("Opcode: REM (Remainder)\n");
                        break;
                    default:
                        printf("Unknown funct7 for OR\n");
                        break;
                }
                break;
            case 0x7:
                switch (funct7) {
                    case 0x00:
                        printf("Opcode: AND (Bitwise AND)\n");
                        break;
                    case 0x01:
                        printf("Opcode: REMU (Remainder Unsigned)\n");
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
            printf("Opcode: LUI (Load Upper Immediate)\n");
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
            switch (funct3) {
                case 0x0:
                    printf("Opcode: BEQ (Branch Equal)\n");
                    break;
                // Add cases for other I-type instructions as needed
                default:
                    printf("Unknown funct3 for opcode 0x63\n");
                    break;
            }
            break;
        default:
            printf("Unknown opcode\n");
            break;
    }
}
// init
int generate_instruction(int opcode, int funct3, int funct7) {
    // Ensure that the opcode, funct3, and funct7 values are within the expected ranges
    if (opcode > 0x7F || funct3 > 0x7 || funct7 > 0x7F) {
        fprintf(stderr, "Error: Invalid opcode, funct3, or funct7 value\n");
        return 0;
    }

    // Construct the 32-bit instruction
    int instruction = (funct7 << 25) | (funct3 << 12) | opcode;

    return instruction;
}


int main(int argc, char *argv[]){
    if (argc != 1) {
        fprintf(stderr, "Usage: %s <instruction>\n", argv[0]);
        return 1;
    }
    int opcode = 0x33;
    int funct3 = 0;
    int i = 0;
    int instructions;
    for ( funct3 = 0; funct3 < 8; funct3++)
    {
        /* code */
        for ( i = 0; i < 3; i++)
        {
            if (i == 0){
                instructions = generate_instruction(opcode,funct3,0x00);
            }
            if (i == 1){
                instructions = generate_instruction(opcode,funct3,0x01);
            }
            if (i == 2){
                int funct7 = 1<<5;
                instructions = generate_instruction(opcode,funct3,funct7);
            }
            identify_instruction(instructions);
        }
        
    }
    return 1;
    
}