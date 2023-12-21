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

    // TODO: omit registers that were not touched
    printf("\n=====================================\nREGISTERS AT END:\n");
    register_access dummy_reg = {0, 0, 2};

    for (int i = 1; i <= 8; ++i) {
        dummy_reg.Index = i;
        printf("\t%s: 0x%.4x (%d)\n", Sim86_RegisterNameFromOperand(&dummy_reg), proc_registers[i-1], proc_registers[i-1]);
    }

    printf("\tflags: %s%s\n",
        proc_flags & FLAG_BIT_SIGN ? "S" : "",
        proc_flags & FLAG_BIT_ZERO ? "Z" : ""
    );

    // Close files
    if (bin_file != stdin && bin_file != NULL) {
        fclose(bin_file);
    }
}

u32 *get_lhs_reg(instruction inst) {
    return &proc_registers[inst.Operands[0].Register.Index - 1];
}

u16 get_data(instruction inst) {
    return inst.Operands[1].Type == Operand_Immediate
        ? inst.Operands[1].Immediate.Value
        : proc_registers[inst.Operands[1].Register.Index - 1];
}

void exec_mov(instruction inst) {
    assert(inst.Operands[0].Register.Index > 0 &&
            inst.Operands[0].Register.Index <= 8);

    proc_registers[inst.Operands[0].Register.Index - 1] = get_data(inst);
}

void set_flags(u16 result) {
    proc_flags = 0;

    proc_flags |= (FLAG_BIT_ZERO && (result == 0));
    proc_flags |= (FLAG_BIT_SIGN && (result & 0x8000));
}

void exec_arith(instruction inst) {
    u32 *lhs_reg = get_lhs_reg(inst);
    u16 data = get_data(inst);
    u16 result = 0;

    switch (inst.Op) {
        case Op_add:
            result = *lhs_reg + data;
            *lhs_reg = result;
        break;
        case Op_sub:
            result = *lhs_reg - data;
            *lhs_reg = result;
        break;
        case Op_cmp:
            result = *lhs_reg - data;
        break;
        default:
            return;
        break;
    }

    set_flags(result);
}

void execute_instruction(instruction inst) {
    u32 *lhs_reg = get_lhs_reg(inst);

    if (inst.Operands[0].Type == Operand_Register) {
        // print initial value of register
        printf(" ; %s:0x%x->",
            Sim86_RegisterNameFromOperand(&inst.Operands[0].Register),
            *lhs_reg);
    }

    switch (inst.Op) {
        case Op_mov:
            exec_mov(inst);
            break;
        case Op_add:
        case Op_sub:
        case Op_cmp:
            exec_arith(inst);
        default:
        break;
    }

    if (inst.Operands[0].Type == Operand_Register) {
        // print resulting value of register
        printf("0x%x", *lhs_reg);
    }
}
