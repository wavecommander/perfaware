#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "../computer_enhance/perfaware/sim86/shared/sim86_shared.h"
#pragma comment (lib, "sim86_shared_debug.lib")

// DEBUG MACROS
// #define DEBUG 1

#ifdef DEBUG
#define IF_DEBUG if (1)

#else
#define IF_DEBUG if (0)

#endif // ifdef DEBUG
// END DEBUG MACROS

u16 proc_flags = 0;
#define FLAG_BIT_ZERO 0x1
#define FLAG_BIT_SIGN 0x2

void execute_instruction(instruction inst);

u32 proc_registers[8] = {0,0,0,0,0,0,0,0};

int main(int argc, char **argv)
{
    { // Version checking, error out on mismatch
        u32 Version = Sim86_GetVersion();
        // printf("Sim86 Version: %u (expected %u)\n", Version, SIM86_VERSION);
        if(Version != SIM86_VERSION)
        {
            printf("ERROR: Header file version doesn't match DLL.\n");
            return -1;
        }
    }

    // Get the instruction table
    instruction_table Table;
    Sim86_Get8086InstructionTable(&Table);

    // Get the 8086 binary file
    FILE *bin_file = NULL;

    // try to find file provided as an argument
    if (argc > 1) {
        if (isprint(argv[1][0]) && !isspace(argv[1][0])) {
            bin_file = fopen(argv[1], "rb");
        }
    }
    // if no valid file provided, get binary from stdin
    if (bin_file == NULL) {
        bin_file = stdin;
    }

    // Load instructions into array from file
    u8 instruction_bytes[1024 * 64];
    u32 instruction_byte_count = 0;
    char c = EOF;

    if ((c = getc(bin_file)) != EOF) {
        do {
            instruction_bytes[instruction_byte_count++] = c;
        } while ((c = getc(bin_file)) != EOF);
    }

    // Decode the instructions
    u32 Offset = 0;
    while (Offset < instruction_byte_count) {
        instruction Decoded;
        Sim86_Decode8086Instruction(instruction_byte_count - Offset, instruction_bytes + Offset, &Decoded);
        if (Decoded.Op) {
            Offset += Decoded.Size;
            Sim86_PrintInstruction(Decoded, stdout);

            IF_DEBUG {
                printf("\t\tSize:%u Op:%s Flags:0x%x\n", Decoded.Size, Sim86_MnemonicFromOperationType(Decoded.Op), Decoded.Flags);
            }

            execute_instruction(Decoded);

            printf("\n");
        } else {
            printf("Unrecognized instruction\n");
            break;
        }
    }

    printf("\n=====================================\nREGISTERS AT END:\n");
    register_access dummy_reg = {0, 0, 2};

    for (int i = 1; i <= 8; ++i) {
        dummy_reg.Index = i;
        printf("\t%s: 0x%.4x (%d)\n", Sim86_RegisterNameFromOperand(&dummy_reg), proc_registers[i-1], proc_registers[i-1]);
    }

    // Close files
    if (bin_file != stdin && bin_file != NULL) {
        fclose(bin_file);
    }
}

void exec_mov(instruction mov_inst) {
    u16 data = mov_inst.Operands[1].Type == Operand_Immediate
        ? mov_inst.Operands[1].Immediate.Value
        : proc_registers[mov_inst.Operands[1].Register.Index - 1];

    assert(mov_inst.Operands[0].Register.Index > 0 &&
            mov_inst.Operands[0].Register.Index <= 8);

    proc_registers[mov_inst.Operands[0].Register.Index - 1] = data;
}

void exec_sub(instruction sub_inst) {

}

void execute_instruction(instruction inst) {
    if (inst.Operands[0].Type == Operand_Register) {
        // print initial value of register
        printf(" ; %s:0x%x->",
            Sim86_RegisterNameFromOperand(&inst.Operands[0].Register),
            proc_registers[inst.Operands[0].Register.Index - 1]);
    }

    switch (inst.Op) {
        case Op_mov:
            exec_mov(inst);
            break;
        case Op_sub:
        case Op_cmp:
        default:
        break;
    }

    if (inst.Operands[0].Type == Operand_Register) {
        // print resulting value of register
            printf("0x%x", proc_registers[inst.Operands[0].Register.Index - 1]);
    }
}
