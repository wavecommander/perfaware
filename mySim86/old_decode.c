#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_size_t.h>

// DEBUG MACROS
// #define DEBUG 1

#ifdef DEBUG
#define IF_DEBUG if (1)

#else
#define IF_DEBUG if (0)

#endif // ifdef DEBUG
// END DEBUG MACROS


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;


#define OUT_SZ_MAX (sizeof(char) * 1024 * 512)


// REGISTER LO_WIDE ADDRESSES
#define REG_AL_WAX 0b000
#define REG_CL_WCX 0b001
#define REG_DL_WDX 0b010
#define REG_BL_WBX 0b011
#define REG_AH_WSP 0b100
#define REG_CH_WBP 0b101
#define REG_DH_WSI 0b110
#define REG_BH_WDI 0b111

char *REG_STR[] = {
    // NARROW
    "al",
    "cl",
    "dl",
    "bl",
    "ah",
    "ch",
    "dh",
    "bh",

    // WIDE
    "ax",
    "cx",
    "dx",
    "bx",
    "sp",
    "bp",
    "si",
    "di",
};


#define MSK_MOV_IME_W 0b00001000
#define MSK_MOV_IME_REG 0b00000111

#define MSK_MOV_D_CATA_D_IS 0b00000010
#define MSK_MOV_REG_CATA_SOP 0b00111000

#define MSK_MOV_CATA_W 0b00000001
#define MSK_MOV_CATA_MOD 0b11000000
#define MSK_MOV_CATA_R_M 0b00000111

#define PREFIX_SOP_ADD 0b000
#define PREFIX_SOP_SUB 0b101
#define PREFIX_SOP_CMP 0b111

#define PREFIX_PRESENT(byte, prefix, mask) (((byte) & (mask)) == (prefix))


struct OpCode {
    u8 prefix;
    u8 mask;
    char *str;
};

struct OpCode OP_MOV_REG_REGM = {
    0b10001000,
    0b11111100,
    "mov"
};

struct OpCode OP_MOV_IME_REGM = {
    0b11000110,
    0b11111110,
    "mov"
};

struct OpCode OP_MOV_IME_REG = {
    0b10110000,
    0b11110000,
    "mov"
};

struct OpCode OP_ADD_REG_REGM = {
    0b00000000,
    0b11111100,
    "add"
};

struct OpCode OP_SUB_REG_REGM = {
    0b00101000,
    0b11111100,
    "sub"
};

struct OpCode OP_CMP_REG_REGM = {
    0b00111000,
    0b11111100,
    "cmp"
};

struct OpCode OP_CATA_IME_REGM = {
    0b10000000,
    0b11111100,
    "$DEDUCE$"
};

struct OpCode OP_COND_JMP = {
    0b01110000,
    0b11110000,
    "$DEDUCE$"
};


u32 dec_opcode(u8 b, FILE *f, char *out, u32 idx);
void decode_bin(FILE *f, char *out, u32 idx);

int main(int argc, char **argv)
{
    // define, malloc, and initialize string buffer
    char *out_str = (char *)malloc(OUT_SZ_MAX);
    memset(out_str, 0, OUT_SZ_MAX);
    u32 out_idx = sprintf(out_str, "%s", "bits 16\n\n");

    FILE *bin_file = NULL;

    // try to find binary file
    if (argc > 1) {
        // IF_DEBUG { printf("argv[1] = %s\n", argv[1]); }

        if (isprint(argv[1][0]) && !isspace(argv[1][0])) {
            bin_file = fopen(argv[1], "rb");
        }
    }
    // if no valid file provided, get binary from stdin
    if (bin_file == NULL) {
        bin_file = stdin;
    }

    decode_bin(bin_file, out_str, out_idx);

    printf("%s\n", out_str);

    if (bin_file != stdin && bin_file != NULL) {
        fclose(bin_file);
    }
}

void decode_bin(FILE *f, char *out, u32 idx)
{
    char c = EOF;

    if ((c = getc(f)) != EOF) {
        do {
            idx += dec_opcode(c, f, out, idx);
        } while ((c = getc(f)) != EOF);
    }
}

void get_cata_sop_str(u8 b, char *op_str) {
    u8 sop = (b & MSK_MOV_REG_CATA_SOP) >> 3;

    switch (sop) {
        case PREFIX_SOP_ADD:
        sprintf(op_str, "%s", "add");
        break;

        case PREFIX_SOP_SUB:
        sprintf(op_str, "%s", "sub");
        break;

        case PREFIX_SOP_CMP:
        sprintf(op_str, "%s", "cmp");
        break;
    }
}

void gen_disp_expr(u8 mod, u8 r_m, FILE *f, char *expr) {
    // TODO: !! FIX BUGS WITH DA

    u8 eidx = sprintf(expr, "%s", "[");

    // gotten ahead of time incase of DIRECT ADDRESS
    u8 lo = 0;
    u8 hi = 0;
    if (mod) {
        lo = getc(f);
        if (mod == 0b10 || (mod == 0 && r_m == 0b110)) {
            hi = getc(f);
        }
    }

    if (!(r_m & 0b100)) {
        // R_M == 0??
        if (r_m & 0b010) {
            // R_M == 01?
            eidx += sprintf(&expr[eidx], "%s", REG_STR[REG_CH_WBP + 8]);
        } else {
            // R_M == 00?
            eidx += sprintf(&expr[eidx], "%s", REG_STR[REG_BL_WBX + 8]);
        }
        if (r_m & 0b001) {
            // R_M == 0?1
            eidx += sprintf(&expr[eidx], " + %s", REG_STR[REG_BH_WDI + 8]);
        } else {
            // R_M == 0?0
            eidx += sprintf(&expr[eidx], " + %s", REG_STR[REG_DH_WSI + 8]);
        }
    } else {
        // R_M == 1??
        if (!(r_m & 0b010)) {
            // R_M == 10?
            if (r_m & 0b001) {
                // R_M == 101
                eidx += sprintf(&expr[eidx], "%s", REG_STR[REG_BH_WDI + 8]);
            } else {
                // R_M == 100
                eidx += sprintf(&expr[eidx], "%s", REG_STR[REG_DH_WSI + 8]);
            }
        } else {
            // R_M == 11?
            if (r_m & 0b001) {
                // R_M == 111
                eidx += sprintf(&expr[eidx], "%s", REG_STR[REG_BL_WBX + 8]);
            } else {
                // TODO: !! FIX BUGS WITH DA
                if (mod == 0b11) {
                    // R_M == 110 && MOD == 00, DIRECT ADDRESS
                    u16 da = (hi << 8) + lo;
                    eidx += sprintf(&expr[eidx], "%d", da);
                } else {
                    // R_M == 110 && MOD == 01 OR MOD == 10
                    eidx += sprintf(&expr[eidx], "%s", REG_STR[REG_CH_WBP + 8]);
                }
            }
        }
    }

    if (mod) {
        u16 disp = lo;
        if (mod == 0b10) {
            disp += hi << 8;
        }
        eidx += sprintf(&expr[eidx], " + %u", disp);
    }

    eidx += sprintf(&expr[eidx], "%s", "]");
}

u32 dec_mov_ime_reg(u8 b, FILE *f, char *out, u32 idx)
{
    u8 w = b & MSK_MOV_IME_W;
    u8 reg = b & MSK_MOV_IME_REG;

    // get lo data byte
    u16 data = getc(f);

    // if 16bit data add hi byte, and make reg wide ver.
    if (w) {
        reg += 8;
        data += getc(f) << 8;
    }

    IF_DEBUG
    {
        printf("W: %o\nREG: %o\nDATA: %u\n\n", w, reg, data);
    }

    return sprintf(&out[idx], "%s %s, %u\n", OP_MOV_IME_REG.str, REG_STR[reg], data);
}

u32 dec_cata_reg_regm(u8 b, FILE *f, char *out, u32 idx)
{
    char op_str[8];
    if (PREFIX_PRESENT(b, OP_MOV_REG_REGM.prefix, OP_MOV_REG_REGM.mask)) {
        sprintf(op_str, "%s", "mov");
    } else {
        get_cata_sop_str(b, op_str);
    }

    u8 d = b & MSK_MOV_D_CATA_D_IS;
    u8 w = b & MSK_MOV_CATA_W;

    // move to analyze next byte in stream
    b = getc(f);
    u8 mod = (b & MSK_MOV_CATA_MOD) >> 6;
    u8 reg = (b & MSK_MOV_REG_CATA_SOP) >> 3;
    u8 r_m = (b & MSK_MOV_CATA_R_M);

    IF_DEBUG
    {
        printf("D: %o\nW: %o\nMOD: %o\nREG: %o\nR_M: %o\n\n", d, w, mod, reg, r_m);
    }

    // if wide make reg address wide ver.
    if (w) {
        reg += 8;
    }

    // reg-reg mode
    if (mod == 0b11) {
        // if wide make r_m(reg) address wide ver.
        if (w) {
            r_m += 8;
        }

        return sprintf(&out[idx], "%s %s, %s\n", op_str,
            REG_STR[d ? reg : r_m],
            REG_STR[d ? r_m : reg]);
    }

    // reg-mem mode
    char expr[64];
    gen_disp_expr(mod, r_m, f, expr);

    char *dst = d ? REG_STR[reg] : expr;
    char *src = d ? expr : REG_STR[reg];

    return sprintf(&out[idx], "%s %s, %s\n", op_str, dst, src);
}

u32 dec_cata_ime_regm(u8 b, FILE *f, char *out, u32 idx)
{
    u8 s = b & MSK_MOV_D_CATA_D_IS;
    u8 w = b & MSK_MOV_CATA_W;

    // move to analyze next byte in stream
    b = getc(f);
    u8 mod = (b & MSK_MOV_CATA_MOD) >> 6;
    u8 r_m = (b & MSK_MOV_CATA_R_M);

    char op_str[8];
    if (PREFIX_PRESENT(b, OP_MOV_REG_REGM.prefix, OP_MOV_REG_REGM.mask)) {
        sprintf(op_str, "%s", "mov");
    } else {
        get_cata_sop_str(b, op_str);
    }

    char expr[64];
    gen_disp_expr(mod, r_m, f, expr);

    // get lo data byte
    u16 data = getc(f);

    // if wide and unsigned, add hi byte and make reg wide ver.
    if (w && !s) {
        data += getc(f) << 8;

        if (mod == 0b11) {
            r_m += 8;
        }
    }

    // sign extend if necessary
    if (w && s) {
        data += (char) data < 0 ? 0b11111111 : 0;
    }

    return sprintf(&out[idx], "%s %s, %u\n", op_str, expr, data);
}

u32 dec_cond_jmp(u8 b, FILE *f, char *out, u32 idx) {
    u8 disp = getc(f);

    char op_str[8] = "";
    switch (b & ~OP_COND_JMP.mask) {
        // TODO: fill out with cases of different jmp op strs
    }

    return sprintf(&out[idx], "%s %d\n", op_str, disp);
}

u32 dec_opcode(u8 b, FILE *f, char *out, u32 idx)
{
    if (PREFIX_PRESENT(b, OP_MOV_REG_REGM.prefix, OP_MOV_REG_REGM.mask) ||
    PREFIX_PRESENT(b, OP_ADD_REG_REGM.prefix, OP_ADD_REG_REGM.mask) ||
    PREFIX_PRESENT(b, OP_SUB_REG_REGM.prefix, OP_SUB_REG_REGM.mask) ||
    PREFIX_PRESENT(b, OP_CMP_REG_REGM.prefix, OP_CMP_REG_REGM.mask)) {
        return dec_cata_reg_regm(b, f, out, idx);
    }
    if (PREFIX_PRESENT(b, OP_MOV_IME_REG.prefix, OP_MOV_IME_REG.mask)) {
        return dec_mov_ime_reg(b, f, out, idx);
    }
    if (PREFIX_PRESENT(b, OP_MOV_IME_REGM.prefix, OP_MOV_IME_REGM.mask) ||
    PREFIX_PRESENT(b, OP_CATA_IME_REGM.prefix, OP_CATA_IME_REGM.mask)) {
        return dec_cata_ime_regm(b, f, out, idx);
    }
    if (PREFIX_PRESENT(b, OP_COND_JMP.prefix, OP_COND_JMP.mask)) {
        return dec_cond_jmp(b, f, out, idx);
    }

    return 0;
}
