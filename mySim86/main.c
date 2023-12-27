#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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

u32 proc_registers[9] = {-1,0,0,0,0,0,0,0, 0};
u32 proc_inst_ptr = 0;

u8 proc_memory[65536];

void gen_flags_str(char* _out, u16 flags);
u16 get_data(instruction inst);
void execute_instruction(instruction inst);

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
    FILE *dump_file = NULL;

    // try to find file provided as an argument
    if (argc > 1) {
        if (isprint(argv[1][0]) && !isspace(argv[1][0])) {
                bin_file = fopen(argv[1], "rb");
        }
        if (argc > 3 && strcmp(argv[2], "-d") == 0) {
            dump_file = fopen(argv[3], "wb");
        }
    }
    // if no valid file provided, get binary from stdin
    if (bin_file == NULL) {
        bin_file = stdin;
    }

    // zero 8086 simulated memory
    memset(&proc_memory, 0, 65536);

    // Load instructions into proc memory from file
    long instruction_byte_count = 0;
    size_t result = 0;

    // Obtain the file size
    fseek(bin_file, 0, SEEK_END);
    instruction_byte_count = ftell(bin_file);
    rewind(bin_file);

    result = fread(proc_memory, 1, instruction_byte_count, bin_file);
    if (result != instruction_byte_count) {
        return 3;
    }
    // Close file
    if (bin_file != stdin && bin_file != NULL) {
        fclose(bin_file);
    }

    // Decode the instructions
    while (proc_inst_ptr < instruction_byte_count) {
        instruction Decoded;
        Sim86_Decode8086Instruction(instruction_byte_count - proc_inst_ptr, proc_memory + proc_inst_ptr, &Decoded);
        if (Decoded.Op) {
            proc_inst_ptr += Decoded.Size;
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

    // print values of 'regular' registers
    printf("\n=====================================\nREGISTERS AT END:\n");
    register_access dummy_reg = {0, 0, 2};
    for (int i = 1; i <= 8; ++i) {
        if (proc_registers[i] != 0) {
            dummy_reg.Index = i;
            printf("\t%s: 0x%.4x (%d)\n", Sim86_RegisterNameFromOperand(&dummy_reg), proc_registers[i], proc_registers[i]);
        }
    }

    // print instruction pointer value
    printf("    ip: 0x%.4x (%d)\n", proc_inst_ptr, proc_inst_ptr);

    // print active flags in flag "register"
    char flags_out_str[16] = "";
    gen_flags_str(flags_out_str, proc_flags);
    printf("    flags: %s\n", flags_out_str);

    if (dump_file != NULL) {
        fwrite(&proc_memory[64*4], sizeof(char), 16384, dump_file);
        fclose(dump_file);
    }
}

void gen_flags_str(char* _out, u16 flags) {
    sprintf(_out, "%s%s",
        flags & FLAG_BIT_SIGN ? "S" : "",
        flags & FLAG_BIT_ZERO ? "Z" : ""
    );
}

u16 get_mem_index(instruction inst, u8 operand_index) {
    assert(inst.Operands[operand_index].Type == Operand_Memory);
    effective_address_expression addr = inst.Operands[operand_index].Address;

    u16 mem_index = addr.Displacement;

    u8 term0 = addr.Terms[0].Register.Index;
    if (term0 >= 1 && term0 <= 8) {
        mem_index += proc_registers[addr.Terms[0].Register.Index];
    }

    u8 term1 = addr.Terms[1].Register.Index;
    if (term1 >= 1 && term1 <= 8) {
        mem_index += proc_registers[addr.Terms[1].Register.Index];
    }

    assert(mem_index < sizeof(proc_memory) - 1);
    return mem_index;
}

u16 get_data(instruction inst) {
    switch (inst.Operands[1].Type) {
        case Operand_Immediate:
            return inst.Operands[1].Immediate.Value;
            break;
        case Operand_Register:
            return proc_registers[inst.Operands[1].Register.Index];
            break;
        case Operand_Memory:
            return proc_memory[get_mem_index(inst, 1)];;
            break;
        case Operand_None:
        default:
            return 0;
    }
}

void exec_mov(instruction inst) {
    if (inst.Operands[0].Type == Operand_Register) {
        assert(inst.Operands[0].Register.Index >= 1 && inst.Operands[0].Register.Index <= 8);
        proc_registers[inst.Operands[0].Register.Index] = get_data(inst);
    } else if (inst.Operands[0].Type == Operand_Memory) {
        u16 mem_index = get_mem_index(inst, 0);
        proc_memory[mem_index] = get_data(inst);
    }
}

void set_flags(u16 result) {
    proc_flags = 0;

    proc_flags |= FLAG_BIT_ZERO * (result == 0);
    proc_flags |= FLAG_BIT_SIGN * !!(result & 0x8000);
}

void exec_arith(instruction inst) {
    u32 *lhs_reg = &proc_registers[inst.Operands[0].Register.Index];
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
    u32 init_inst_ptr = proc_inst_ptr;
    u16 init_flags = proc_flags;

    u32 *lhs_reg = &proc_registers[0];
    u32 init_reg = 0;
    if (inst.Operands[0].Type == Operand_Register) {
        lhs_reg = &proc_registers[inst.Operands[0].Register.Index];
        init_reg = *lhs_reg;
    }

    switch (inst.Op) {
        case Op_jne:
            if (!(proc_flags & FLAG_BIT_ZERO)) {
                proc_inst_ptr += inst.Operands[0].Immediate.Value;
            }
            break;
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

    { // print out global effects of executing this instruction
        char reg_out_str[32];
        if (inst.Operands[0].Type == Operand_Register) {
            sprintf(reg_out_str, " ; %s:0x%x->0x%x",
                Sim86_RegisterNameFromOperand(&inst.Operands[0].Register),
                init_reg, *lhs_reg);
        }

        char flags_out_str[32] = "";
        if (proc_flags != init_flags) {
            char flags_1[16] = "";
            char flags_2[16] = "";
            gen_flags_str(flags_1, init_flags);
            gen_flags_str(flags_2, proc_flags);
            sprintf(flags_out_str, "flags:%s->%s", flags_1, flags_2);
        }

        printf("%s ip:0x%x->0x%x %s", reg_out_str, init_inst_ptr, proc_inst_ptr, flags_out_str);
    }
}
