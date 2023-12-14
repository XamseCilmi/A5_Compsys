#include <stdio.h>

void identify_instruction(uint32_t instruction) {
    uint32_t opcode = instruction & 0x7F;   // Extract the opcode (lowest 7 bits)
    uint32_t funct3 = (instruction >> 12) & 0x7;  // Extract the funct3 field (bits 12-14)
    
    printf("Input Instruction: 0x%08X\n", instruction);

    switch (opcode) {
        case 0x37:
            printf("Opcode: LUI (Load Upper Immediate)\n");
            break;
        case 0x17:
            printf("Opcode: AUIPC (Add Upper Immediate to PC)\n");
            break;
        case 0x6F:
            printf("Opcode: JAL (Jump and Link)\n");
            break;
        case 0x67:
            printf("Opcode: JALR (Jump and Link Register)\n");
            break;
        case 0x63:
            switch (funct3) {
                case 0x0:
                    printf("Opcode: BEQ (Branch Equal)\n");
                    break;
                case 0x1:
                    printf("Opcode: BNE (Branch Not Equal)\n");
                    break;
                case 0x4:
                    printf("Opcode: BLT (Branch Less Than)\n");
                    break;
                case 0x5:
                    printf("Opcode: BGE (Branch Greater Than or Equal)\n");
                    break;
                case 0x6:
                    printf("Opcode: BLTU (Branch Less Than Unsigned)\n");
                    break;
                case 0x7:
                    printf("Opcode: BGEU (Branch Greater Than or Equal Unsigned)\n");
                    break;
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

int main() {
    uint32_t input_instruction = 0x00010513;  // Example instruction: add x1, x2, x3
    identify_instruction(input_instruction);
    return 0;
}
