typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef s32 b32;
static u32 const SIM86_VERSION = 3;

enum operation_type : u32
{
    Op_None,
    Op_mov,
    Op_push,
    Op_pop,
    Op_xchg,
    Op_in,
    Op_out,
    Op_xlat,
    Op_lea,
    Op_lds,
    Op_les,
    Op_lahf,
    Op_sahf,
    Op_pushf,
    Op_popf,
    Op_add,
    Op_adc,
    Op_inc,
    Op_aaa,
    Op_daa,
    Op_sub,
    Op_sbb,
    Op_dec,
    Op_neg,
    Op_cmp,
    Op_aas,
    Op_das,
    Op_mul,
    Op_imul,
    Op_aam,
    Op_div,
    Op_idiv,
    Op_aad,
    Op_cbw,
    Op_cwd,
    Op_not,
    Op_shl,
    Op_shr,
    Op_sar,
    Op_rol,
    Op_ror,
    Op_rcl,
    Op_rcr,
    Op_and,
    Op_test,
    Op_or,
    Op_xor,
    Op_rep,
    Op_movs,
    Op_cmps,
    Op_scas,
    Op_lods,
    Op_stos,
    Op_call,
    Op_jmp,
    Op_ret,
    Op_retf,
    Op_je,
    Op_jl,
    Op_jle,
    Op_jb,
    Op_jbe,
    Op_jp,
    Op_jo,
    Op_js,
    Op_jne,
    Op_jnl,
    Op_jg,
    Op_jnb,
    Op_ja,
    Op_jnp,
    Op_jno,
    Op_jns,
    Op_loop,
    Op_loopz,
    Op_loopnz,
    Op_jcxz,
    Op_int,
    Op_int3,
    Op_into,
    Op_iret,
    Op_clc,
    Op_cmc,
    Op_stc,
    Op_cld,
    Op_std,
    Op_cli,
    Op_sti,
    Op_hlt,
    Op_wait,
    Op_esc,
    Op_lock,
    Op_segment,

    Op_Count,
};
enum instruction_bits_usage : u8
{
    Bits_End,
    Bits_Literal,
    Bits_D,
    Bits_S,
    Bits_W,
    Bits_V,
    Bits_Z,
    Bits_MOD,
    Bits_REG,
    Bits_RM,
    Bits_SR,
    Bits_Disp,
    Bits_Data,
    Bits_DispAlwaysW,
    Bits_WMakesDataW,
    Bits_RMRegAlwaysW,
    Bits_RelJMPDisp,
    Bits_Far,
    Bits_Count,
};
struct instruction_bits
{
    instruction_bits_usage Usage;
    u8 BitCount;
    u8 Shift;
    u8 Value;
};
struct instruction_encoding
{
    operation_type Op;
    instruction_bits Bits[16];
};
struct instruction_table
{
    instruction_encoding *Encodings;
    u32 EncodingCount;
    u32 MaxInstructionByteCount;
};

instruction_encoding InstructionTable8086[] = {
    {Op_mov,
     {{Bits_Literal, sizeof("100010") - 1, 0, 0b100010},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_mov,
     {{Bits_Literal, sizeof("1100011") - 1, 0, 0b1100011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_D, 0, 0, 0}}},
    {Op_mov,
     {{Bits_Literal, sizeof("1011") - 1, 0, 0b1011},
      {Bits_W, 1},
      {Bits_REG, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_D, 0, 0, 1}}},
    {Op_mov,
     {{Bits_Literal, sizeof("1010000") - 1, 0, 0b1010000},
      {Bits_W, 1},
      {Bits_Disp, 0, 0, 0},
      {Bits_DispAlwaysW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_MOD, 0, 0, 0},
      {Bits_RM, 0, 0, 0b110},
      {Bits_D, 0, 0, 1}}},
    {Op_mov,
     {{Bits_Literal, sizeof("1010001") - 1, 0, 0b1010001},
      {Bits_W, 1},
      {Bits_Disp, 0, 0, 0},
      {Bits_DispAlwaysW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_MOD, 0, 0, 0},
      {Bits_RM, 0, 0, 0b110},
      {Bits_D, 0, 0, 0}}},
    {Op_mov,
     {{Bits_Literal, sizeof("100011") - 1, 0, 0b100011},
      {Bits_D, 1},
      {Bits_Literal, sizeof("0") - 1, 0, 0b0},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("0") - 1, 0, 0b0},
      {Bits_SR, 2},
      {Bits_RM, 3},
      {Bits_W, 0, 0, 1}}},
    {Op_push,
     {{Bits_Literal, sizeof("11111111") - 1, 0, 0b11111111},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("110") - 1, 0, 0b110},
      {Bits_RM, 3},
      {Bits_W, 0, 0, 1}}},
    {Op_push, {{Bits_Literal, sizeof("01010") - 1, 0, 0b01010}, {Bits_REG, 3}, {Bits_W, 0, 0, 1}}},
    {Op_push,
     {{Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_SR, 2},
      {Bits_Literal, sizeof("110") - 1, 0, 0b110},
      {Bits_W, 0, 0, 1}}},
    {Op_pop,
     {{Bits_Literal, sizeof("10001111") - 1, 0, 0b10001111},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_RM, 3},
      {Bits_W, 0, 0, 1}}},
    {Op_pop, {{Bits_Literal, sizeof("01011") - 1, 0, 0b01011}, {Bits_REG, 3}, {Bits_W, 0, 0, 1}}},
    {Op_pop,
     {{Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_SR, 2},
      {Bits_Literal, sizeof("111") - 1, 0, 0b111},
      {Bits_W, 0, 0, 1}}},
    {Op_xchg,
     {{Bits_Literal, sizeof("1000011") - 1, 0, 0b1000011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3},
      {Bits_D, 0, 0, 1}}},
    {Op_xchg,
     {{Bits_Literal, sizeof("10010") - 1, 0, 0b10010},
      {Bits_REG, 3},
      {Bits_MOD, 0, 0, 0b11},
      {Bits_W, 0, 0, 1},
      {Bits_RM, 0, 0, 0}}},
    {Op_in,
     {{Bits_Literal, sizeof("1110010") - 1, 0, 0b1110010},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_in,
     {{Bits_Literal, sizeof("1110110") - 1, 0, 0b1110110},
      {Bits_W, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1},
      {Bits_MOD, 0, 0, 0b11},
      {Bits_RM, 0, 0, 2},
      {Bits_RMRegAlwaysW, 0, 0, 1}}},
    {Op_out,
     {{Bits_Literal, sizeof("1110011") - 1, 0, 0b1110011},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 0}}},
    {Op_out,
     {{Bits_Literal, sizeof("1110111") - 1, 0, 0b1110111},
      {Bits_W, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 0},
      {Bits_MOD, 0, 0, 0b11},
      {Bits_RM, 0, 0, 2},
      {Bits_RMRegAlwaysW, 0, 0, 1}}},
    {
        Op_xlat,
        {{Bits_Literal, sizeof("11010111") - 1, 0, 0b11010111}},
    },
    {Op_lea,
     {{Bits_Literal, sizeof("10001101") - 1, 0, 0b10001101},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3},
      {Bits_D, 0, 0, 1},
      {Bits_W, 0, 0, 1}}},
    {Op_lds,
     {{Bits_Literal, sizeof("11000101") - 1, 0, 0b11000101},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3},
      {Bits_D, 0, 0, 1},
      {Bits_W, 0, 0, 1}}},
    {Op_les,
     {{Bits_Literal, sizeof("11000100") - 1, 0, 0b11000100},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3},
      {Bits_D, 0, 0, 1},
      {Bits_W, 0, 0, 1}}},
    {
        Op_lahf,
        {{Bits_Literal, sizeof("10011111") - 1, 0, 0b10011111}},
    },
    {
        Op_sahf,
        {{Bits_Literal, sizeof("10011110") - 1, 0, 0b10011110}},
    },
    {
        Op_pushf,
        {{Bits_Literal, sizeof("10011100") - 1, 0, 0b10011100}},
    },
    {
        Op_popf,
        {{Bits_Literal, sizeof("10011101") - 1, 0, 0b10011101}},
    },
    {Op_add,
     {{Bits_Literal, sizeof("000000") - 1, 0, 0b000000},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_add,
     {{Bits_Literal, sizeof("100000") - 1, 0, 0b100000},
      {Bits_S, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_add,
     {{Bits_Literal, sizeof("0000010") - 1, 0, 0b0000010},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_adc,
     {{Bits_Literal, sizeof("000100") - 1, 0, 0b000100},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_adc,
     {{Bits_Literal, sizeof("100000") - 1, 0, 0b100000},
      {Bits_S, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("010") - 1, 0, 0b010},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_adc,
     {{Bits_Literal, sizeof("0001010") - 1, 0, 0b0001010},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_inc,
     {{Bits_Literal, sizeof("1111111") - 1, 0, 0b1111111},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_RM, 3}}},
    {Op_inc, {{Bits_Literal, sizeof("01000") - 1, 0, 0b01000}, {Bits_REG, 3}, {Bits_W, 0, 0, 1}}},
    {
        Op_aaa,
        {{Bits_Literal, sizeof("00110111") - 1, 0, 0b00110111}},
    },
    {
        Op_daa,
        {{Bits_Literal, sizeof("00100111") - 1, 0, 0b00100111}},
    },
    {Op_sub,
     {{Bits_Literal, sizeof("001010") - 1, 0, 0b001010},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_sub,
     {{Bits_Literal, sizeof("100000") - 1, 0, 0b100000},
      {Bits_S, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("101") - 1, 0, 0b101},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_sub,
     {{Bits_Literal, sizeof("0010110") - 1, 0, 0b0010110},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_sbb,
     {{Bits_Literal, sizeof("000110") - 1, 0, 0b000110},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_sbb,
     {{Bits_Literal, sizeof("100000") - 1, 0, 0b100000},
      {Bits_S, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("011") - 1, 0, 0b011},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_sbb,
     {{Bits_Literal, sizeof("0001110") - 1, 0, 0b0001110},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_dec,
     {{Bits_Literal, sizeof("1111111") - 1, 0, 0b1111111},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("001") - 1, 0, 0b001},
      {Bits_RM, 3}}},
    {Op_dec, {{Bits_Literal, sizeof("01001") - 1, 0, 0b01001}, {Bits_REG, 3}, {Bits_W, 0, 0, 1}}},
    {Op_neg,
     {{Bits_Literal, sizeof("1111011") - 1, 0, 0b1111011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("011") - 1, 0, 0b011},
      {Bits_RM, 3}}},
    {Op_cmp,
     {{Bits_Literal, sizeof("001110") - 1, 0, 0b001110},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_cmp,
     {{Bits_Literal, sizeof("100000") - 1, 0, 0b100000},
      {Bits_S, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("111") - 1, 0, 0b111},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_cmp,
     {{Bits_Literal, sizeof("0011110") - 1, 0, 0b0011110},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {
        Op_aas,
        {{Bits_Literal, sizeof("00111111") - 1, 0, 0b00111111}},
    },
    {
        Op_das,
        {{Bits_Literal, sizeof("00101111") - 1, 0, 0b00101111}},
    },
    {Op_mul,
     {{Bits_Literal, sizeof("1111011") - 1, 0, 0b1111011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("100") - 1, 0, 0b100},
      {Bits_RM, 3},
      {Bits_S, 0, 0, 0}}},
    {Op_imul,
     {{Bits_Literal, sizeof("1111011") - 1, 0, 0b1111011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("101") - 1, 0, 0b101},
      {Bits_RM, 3},
      {Bits_S, 0, 0, 1}}},
    {Op_aam,
     {{Bits_Literal, sizeof("11010100") - 1, 0, 0b11010100}, {Bits_Literal, sizeof("00001010") - 1, 0, 0b00001010}}},
    {Op_div,
     {{Bits_Literal, sizeof("1111011") - 1, 0, 0b1111011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("110") - 1, 0, 0b110},
      {Bits_RM, 3},
      {Bits_S, 0, 0, 0}}},
    {Op_idiv,
     {{Bits_Literal, sizeof("1111011") - 1, 0, 0b1111011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("111") - 1, 0, 0b111},
      {Bits_RM, 3},
      {Bits_S, 0, 0, 1}}},
    {Op_aad,
     {{Bits_Literal, sizeof("11010101") - 1, 0, 0b11010101}, {Bits_Literal, sizeof("00001010") - 1, 0, 0b00001010}}},
    {
        Op_cbw,
        {{Bits_Literal, sizeof("10011000") - 1, 0, 0b10011000}},
    },
    {
        Op_cwd,
        {{Bits_Literal, sizeof("10011001") - 1, 0, 0b10011001}},
    },
    {Op_not,
     {{Bits_Literal, sizeof("1111011") - 1, 0, 0b1111011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("010") - 1, 0, 0b010},
      {Bits_RM, 3}}},
    {Op_shl,
     {{Bits_Literal, sizeof("110100") - 1, 0, 0b110100},
      {Bits_V, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("100") - 1, 0, 0b100},
      {Bits_RM, 3}}},
    {Op_shr,
     {{Bits_Literal, sizeof("110100") - 1, 0, 0b110100},
      {Bits_V, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("101") - 1, 0, 0b101},
      {Bits_RM, 3}}},
    {Op_sar,
     {{Bits_Literal, sizeof("110100") - 1, 0, 0b110100},
      {Bits_V, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("111") - 1, 0, 0b111},
      {Bits_RM, 3}}},
    {Op_rol,
     {{Bits_Literal, sizeof("110100") - 1, 0, 0b110100},
      {Bits_V, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_RM, 3}}},
    {Op_ror,
     {{Bits_Literal, sizeof("110100") - 1, 0, 0b110100},
      {Bits_V, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("001") - 1, 0, 0b001},
      {Bits_RM, 3}}},
    {Op_rcl,
     {{Bits_Literal, sizeof("110100") - 1, 0, 0b110100},
      {Bits_V, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("010") - 1, 0, 0b010},
      {Bits_RM, 3}}},
    {Op_rcr,
     {{Bits_Literal, sizeof("110100") - 1, 0, 0b110100},
      {Bits_V, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("011") - 1, 0, 0b011},
      {Bits_RM, 3}}},
    {Op_and,
     {{Bits_Literal, sizeof("001000") - 1, 0, 0b001000},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_and,
     {{Bits_Literal, sizeof("1000000") - 1, 0, 0b1000000},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("100") - 1, 0, 0b100},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_and,
     {{Bits_Literal, sizeof("0010010") - 1, 0, 0b0010010},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_test,
     {{Bits_Literal, sizeof("1000010") - 1, 0, 0b1000010}, {Bits_W, 1}, {Bits_MOD, 2}, {Bits_REG, 3}, {Bits_RM, 3}}},
    {Op_test,
     {{Bits_Literal, sizeof("1111011") - 1, 0, 0b1111011},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("000") - 1, 0, 0b000},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_test,
     {{Bits_Literal, sizeof("1010100") - 1, 0, 0b1010100},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_or,
     {{Bits_Literal, sizeof("000010") - 1, 0, 0b000010},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_or,
     {{Bits_Literal, sizeof("1000000") - 1, 0, 0b1000000},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("001") - 1, 0, 0b001},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_or,
     {{Bits_Literal, sizeof("0000110") - 1, 0, 0b0000110},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_xor,
     {{Bits_Literal, sizeof("001100") - 1, 0, 0b001100},
      {Bits_D, 1},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_REG, 3},
      {Bits_RM, 3}}},
    {Op_xor,
     {{Bits_Literal, sizeof("1000000") - 1, 0, 0b1000000},
      {Bits_W, 1},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("110") - 1, 0, 0b110},
      {Bits_RM, 3},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1}}},
    {Op_xor,
     {{Bits_Literal, sizeof("0011010") - 1, 0, 0b0011010},
      {Bits_W, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_REG, 0, 0, 0},
      {Bits_D, 0, 0, 1}}},
    {Op_rep, {{Bits_Literal, sizeof("1111001") - 1, 0, 0b1111001}, {Bits_Z, 1}}},
    {Op_movs, {{Bits_Literal, sizeof("1010010") - 1, 0, 0b1010010}, {Bits_W, 1}}},
    {Op_cmps, {{Bits_Literal, sizeof("1010011") - 1, 0, 0b1010011}, {Bits_W, 1}}},
    {Op_scas, {{Bits_Literal, sizeof("1010111") - 1, 0, 0b1010111}, {Bits_W, 1}}},
    {Op_lods, {{Bits_Literal, sizeof("1010110") - 1, 0, 0b1010110}, {Bits_W, 1}}},
    {Op_stos, {{Bits_Literal, sizeof("1010101") - 1, 0, 0b1010101}, {Bits_W, 1}}},
    {Op_call,
     {{Bits_Literal, sizeof("11101000") - 1, 0, 0b11101000},
      {Bits_Disp, 0, 0, 0},
      {Bits_DispAlwaysW, 0, 0, 1},
      {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_call,
     {{Bits_Literal, sizeof("11111111") - 1, 0, 0b11111111},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("010") - 1, 0, 0b010},
      {Bits_RM, 3},
      {Bits_W, 0, 0, 1}}},
    {Op_call,
     {{Bits_Literal, sizeof("10011010") - 1, 0, 0b10011010},
      {Bits_Disp, 0, 0, 0},
      {Bits_DispAlwaysW, 0, 0, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_W, 0, 0, 1}}},
    {Op_call,
     {{Bits_Literal, sizeof("11111111") - 1, 0, 0b11111111},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("011") - 1, 0, 0b011},
      {Bits_RM, 3},
      {Bits_W, 0, 0, 1},
      {Bits_Far, 0, 0, 1}}},
    {Op_jmp,
     {{Bits_Literal, sizeof("11101001") - 1, 0, 0b11101001},
      {Bits_Disp, 0, 0, 0},
      {Bits_DispAlwaysW, 0, 0, 1},
      {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jmp, {{Bits_Literal, sizeof("11101011") - 1, 0, 0b11101011}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jmp,
     {{Bits_Literal, sizeof("11111111") - 1, 0, 0b11111111},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("100") - 1, 0, 0b100},
      {Bits_RM, 3},
      {Bits_W, 0, 0, 1}}},
    {Op_jmp,
     {{Bits_Literal, sizeof("11101010") - 1, 0, 0b11101010},
      {Bits_Disp, 0, 0, 0},
      {Bits_DispAlwaysW, 0, 0, 1},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_W, 0, 0, 1}}},
    {Op_jmp,
     {{Bits_Literal, sizeof("11111111") - 1, 0, 0b11111111},
      {Bits_MOD, 2},
      {Bits_Literal, sizeof("101") - 1, 0, 0b101},
      {Bits_RM, 3},
      {Bits_W, 0, 0, 1},
      {Bits_Far, 0, 0, 1}}},
    {
        Op_ret,
        {{Bits_Literal, sizeof("11000011") - 1, 0, 0b11000011}},
    },
    {Op_ret,
     {{Bits_Literal, sizeof("11000010") - 1, 0, 0b11000010},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_W, 0, 0, 1}}},
    {
        Op_retf,
        {{Bits_Literal, sizeof("11001011") - 1, 0, 0b11001011}},
    },
    {Op_retf,
     {{Bits_Literal, sizeof("11001010") - 1, 0, 0b11001010},
      {Bits_Data, 0, 0, 0},
      {Bits_WMakesDataW, 0, 0, 1},
      {Bits_W, 0, 0, 1}}},
    {Op_je, {{Bits_Literal, sizeof("01110100") - 1, 0, 0b01110100}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jl, {{Bits_Literal, sizeof("01111100") - 1, 0, 0b01111100}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jle, {{Bits_Literal, sizeof("01111110") - 1, 0, 0b01111110}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jb, {{Bits_Literal, sizeof("01110010") - 1, 0, 0b01110010}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jbe, {{Bits_Literal, sizeof("01110110") - 1, 0, 0b01110110}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jp, {{Bits_Literal, sizeof("01111010") - 1, 0, 0b01111010}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jo, {{Bits_Literal, sizeof("01110000") - 1, 0, 0b01110000}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_js, {{Bits_Literal, sizeof("01111000") - 1, 0, 0b01111000}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jne, {{Bits_Literal, sizeof("01110101") - 1, 0, 0b01110101}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jnl, {{Bits_Literal, sizeof("01111101") - 1, 0, 0b01111101}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jg, {{Bits_Literal, sizeof("01111111") - 1, 0, 0b01111111}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jnb, {{Bits_Literal, sizeof("01110011") - 1, 0, 0b01110011}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_ja, {{Bits_Literal, sizeof("01110111") - 1, 0, 0b01110111}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jnp, {{Bits_Literal, sizeof("01111011") - 1, 0, 0b01111011}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jno, {{Bits_Literal, sizeof("01110001") - 1, 0, 0b01110001}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jns, {{Bits_Literal, sizeof("01111001") - 1, 0, 0b01111001}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_loop,
     {{Bits_Literal, sizeof("11100010") - 1, 0, 0b11100010}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_loopz,
     {{Bits_Literal, sizeof("11100001") - 1, 0, 0b11100001}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_loopnz,
     {{Bits_Literal, sizeof("11100000") - 1, 0, 0b11100000}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_jcxz,
     {{Bits_Literal, sizeof("11100011") - 1, 0, 0b11100011}, {Bits_Disp, 0, 0, 0}, {Bits_RelJMPDisp, 0, 0, 1}}},
    {Op_int, {{Bits_Literal, sizeof("11001101") - 1, 0, 0b11001101}, {Bits_Data, 0, 0, 0}}},
    {
        Op_int3,
        {{Bits_Literal, sizeof("11001100") - 1, 0, 0b11001100}},
    },
    {
        Op_into,
        {{Bits_Literal, sizeof("11001110") - 1, 0, 0b11001110}},
    },
    {
        Op_iret,
        {{Bits_Literal, sizeof("11001111") - 1, 0, 0b11001111}},
    },
    {
        Op_clc,
        {{Bits_Literal, sizeof("11111000") - 1, 0, 0b11111000}},
    },
    {
        Op_cmc,
        {{Bits_Literal, sizeof("11110101") - 1, 0, 0b11110101}},
    },
    {
        Op_stc,
        {{Bits_Literal, sizeof("11111001") - 1, 0, 0b11111001}},
    },
    {
        Op_cld,
        {{Bits_Literal, sizeof("11111100") - 1, 0, 0b11111100}},
    },
    {
        Op_std,
        {{Bits_Literal, sizeof("11111101") - 1, 0, 0b11111101}},
    },
    {
        Op_cli,
        {{Bits_Literal, sizeof("11111010") - 1, 0, 0b11111010}},
    },
    {
        Op_sti,
        {{Bits_Literal, sizeof("11111011") - 1, 0, 0b11111011}},
    },
    {
        Op_hlt,
        {{Bits_Literal, sizeof("11110100") - 1, 0, 0b11110100}},
    },
    {
        Op_wait,
        {{Bits_Literal, sizeof("10011011") - 1, 0, 0b10011011}},
    },
    {Op_esc,
     {{Bits_Literal, sizeof("11011") - 1, 0, 0b11011},
      {Bits_Data, 3, 0},
      {Bits_MOD, 2},
      {Bits_Data, 3, 3},
      {Bits_RM, 3}}},
    {
        Op_lock,
        {{Bits_Literal, sizeof("11110000") - 1, 0, 0b11110000}},
    },
    {Op_segment,
     {{Bits_Literal, sizeof("001") - 1, 0, 0b001}, {Bits_SR, 2}, {Bits_Literal, sizeof("110") - 1, 0, 0b110}}},
};
