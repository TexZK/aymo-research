/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023 Andrea Zoppi.

This file is part of AYMO.

AYMO is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 2.1 of the License, or (at your option)
any later version.

AYMO is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with AYMO. If not, see <https://www.gnu.org/licenses/>.


This work is heavily based on the Nuked OPL3 library, distributed under
the same licensing model.

Thanks:
    Nuke.YKT:
        Nuked OPL3 emulator.  The following thanks inherit from it.
    MAME Development Team (Jarek Burczynski, Tatsuyuki Satoh):
        Feedback and Rhythm part calculation information.
    forums.submarine.org.uk (carbon14, opl3):
        Tremolo and phase generator calculation information.
    OPLx decapsulated (Matthew Gambrell, Olli Niemitalo):
        OPL2 ROMs.
    siliconpr0n.org (John McMaster, digshadow):
        YMF262 and VRC VII decaps and die shots.
*/

#include "aymo_ymf262_armv7_neon.h"
#include "aymo_arch_armv7_neon_macros.h"
#ifdef AYMO_ARCH_IS_ARMV7_NEON


// Exponential look-up table
// Values are pre-multiplied by 2
AYMO_STATIC AYMO_ALIGN_V16
const int16_t aymo_(exp_x2_table)[256 + 4] =
{
    0x0FF4, 0x0FEA, 0x0FDE, 0x0FD4, 0x0FC8, 0x0FBE, 0x0FB4, 0x0FA8,
    0x0F9E, 0x0F92, 0x0F88, 0x0F7E, 0x0F72, 0x0F68, 0x0F5C, 0x0F52,
    0x0F48, 0x0F3E, 0x0F32, 0x0F28, 0x0F1E, 0x0F14, 0x0F08, 0x0EFE,
    0x0EF4, 0x0EEA, 0x0EE0, 0x0ED4, 0x0ECA, 0x0EC0, 0x0EB6, 0x0EAC,
    0x0EA2, 0x0E98, 0x0E8E, 0x0E84, 0x0E7A, 0x0E70, 0x0E66, 0x0E5C,
    0x0E52, 0x0E48, 0x0E3E, 0x0E34, 0x0E2A, 0x0E20, 0x0E16, 0x0E0C,
    0x0E04, 0x0DFA, 0x0DF0, 0x0DE6, 0x0DDC, 0x0DD2, 0x0DCA, 0x0DC0,
    0x0DB6, 0x0DAC, 0x0DA4, 0x0D9A, 0x0D90, 0x0D88, 0x0D7E, 0x0D74,
    0x0D6A, 0x0D62, 0x0D58, 0x0D50, 0x0D46, 0x0D3C, 0x0D34, 0x0D2A,
    0x0D22, 0x0D18, 0x0D10, 0x0D06, 0x0CFE, 0x0CF4, 0x0CEC, 0x0CE2,
    0x0CDA, 0x0CD0, 0x0CC8, 0x0CBE, 0x0CB6, 0x0CAE, 0x0CA4, 0x0C9C,
    0x0C92, 0x0C8A, 0x0C82, 0x0C78, 0x0C70, 0x0C68, 0x0C60, 0x0C56,
    0x0C4E, 0x0C46, 0x0C3C, 0x0C34, 0x0C2C, 0x0C24, 0x0C1C, 0x0C12,
    0x0C0A, 0x0C02, 0x0BFA, 0x0BF2, 0x0BEA, 0x0BE0, 0x0BD8, 0x0BD0,
    0x0BC8, 0x0BC0, 0x0BB8, 0x0BB0, 0x0BA8, 0x0BA0, 0x0B98, 0x0B90,
    0x0B88, 0x0B80, 0x0B78, 0x0B70, 0x0B68, 0x0B60, 0x0B58, 0x0B50,
    0x0B48, 0x0B40, 0x0B38, 0x0B32, 0x0B2A, 0x0B22, 0x0B1A, 0x0B12,
    0x0B0A, 0x0B02, 0x0AFC, 0x0AF4, 0x0AEC, 0x0AE4, 0x0ADE, 0x0AD6,
    0x0ACE, 0x0AC6, 0x0AC0, 0x0AB8, 0x0AB0, 0x0AA8, 0x0AA2, 0x0A9A,
    0x0A92, 0x0A8C, 0x0A84, 0x0A7C, 0x0A76, 0x0A6E, 0x0A68, 0x0A60,
    0x0A58, 0x0A52, 0x0A4A, 0x0A44, 0x0A3C, 0x0A36, 0x0A2E, 0x0A28,
    0x0A20, 0x0A18, 0x0A12, 0x0A0C, 0x0A04, 0x09FE, 0x09F6, 0x09F0,
    0x09E8, 0x09E2, 0x09DA, 0x09D4, 0x09CE, 0x09C6, 0x09C0, 0x09B8,
    0x09B2, 0x09AC, 0x09A4, 0x099E, 0x0998, 0x0990, 0x098A, 0x0984,
    0x097C, 0x0976, 0x0970, 0x096A, 0x0962, 0x095C, 0x0956, 0x0950,
    0x0948, 0x0942, 0x093C, 0x0936, 0x0930, 0x0928, 0x0922, 0x091C,
    0x0916, 0x0910, 0x090A, 0x0904, 0x08FC, 0x08F6, 0x08F0, 0x08EA,
    0x08E4, 0x08DE, 0x08D8, 0x08D2, 0x08CC, 0x08C6, 0x08C0, 0x08BA,
    0x08B4, 0x08AE, 0x08A8, 0x08A2, 0x089C, 0x0896, 0x0890, 0x088A,
    0x0884, 0x087E, 0x0878, 0x0872, 0x086C, 0x0866, 0x0860, 0x085A,
    0x0854, 0x0850, 0x084A, 0x0844, 0x083E, 0x0838, 0x0832, 0x082C,
    0x0828, 0x0822, 0x081C, 0x0816, 0x0810, 0x080C, 0x0806, 0x0800,
    0x0800, 0x0800, 0x0800, 0x0800
};


// Logsin look-up table
AYMO_STATIC AYMO_ALIGN_V16
const int16_t aymo_(logsin_table)[256 + 4] =
{
    0x0859, 0x06C3, 0x0607, 0x058B, 0x052E, 0x04E4, 0x04A6, 0x0471,
    0x0443, 0x041A, 0x03F5, 0x03D3, 0x03B5, 0x0398, 0x037E, 0x0365,
    0x034E, 0x0339, 0x0324, 0x0311, 0x02FF, 0x02ED, 0x02DC, 0x02CD,
    0x02BD, 0x02AF, 0x02A0, 0x0293, 0x0286, 0x0279, 0x026D, 0x0261,
    0x0256, 0x024B, 0x0240, 0x0236, 0x022C, 0x0222, 0x0218, 0x020F,
    0x0206, 0x01FD, 0x01F5, 0x01EC, 0x01E4, 0x01DC, 0x01D4, 0x01CD,
    0x01C5, 0x01BE, 0x01B7, 0x01B0, 0x01A9, 0x01A2, 0x019B, 0x0195,
    0x018F, 0x0188, 0x0182, 0x017C, 0x0177, 0x0171, 0x016B, 0x0166,
    0x0160, 0x015B, 0x0155, 0x0150, 0x014B, 0x0146, 0x0141, 0x013C,
    0x0137, 0x0133, 0x012E, 0x0129, 0x0125, 0x0121, 0x011C, 0x0118,
    0x0114, 0x010F, 0x010B, 0x0107, 0x0103, 0x00FF, 0x00FB, 0x00F8,
    0x00F4, 0x00F0, 0x00EC, 0x00E9, 0x00E5, 0x00E2, 0x00DE, 0x00DB,
    0x00D7, 0x00D4, 0x00D1, 0x00CD, 0x00CA, 0x00C7, 0x00C4, 0x00C1,
    0x00BE, 0x00BB, 0x00B8, 0x00B5, 0x00B2, 0x00AF, 0x00AC, 0x00A9,
    0x00A7, 0x00A4, 0x00A1, 0x009F, 0x009C, 0x0099, 0x0097, 0x0094,
    0x0092, 0x008F, 0x008D, 0x008A, 0x0088, 0x0086, 0x0083, 0x0081,
    0x007F, 0x007D, 0x007A, 0x0078, 0x0076, 0x0074, 0x0072, 0x0070,
    0x006E, 0x006C, 0x006A, 0x0068, 0x0066, 0x0064, 0x0062, 0x0060,
    0x005E, 0x005C, 0x005B, 0x0059, 0x0057, 0x0055, 0x0053, 0x0052,
    0x0050, 0x004E, 0x004D, 0x004B, 0x004A, 0x0048, 0x0046, 0x0045,
    0x0043, 0x0042, 0x0040, 0x003F, 0x003E, 0x003C, 0x003B, 0x0039,
    0x0038, 0x0037, 0x0035, 0x0034, 0x0033, 0x0031, 0x0030, 0x002F,
    0x002E, 0x002D, 0x002B, 0x002A, 0x0029, 0x0028, 0x0027, 0x0026,
    0x0025, 0x0024, 0x0023, 0x0022, 0x0021, 0x0020, 0x001F, 0x001E,
    0x001D, 0x001C, 0x001B, 0x001A, 0x0019, 0x0018, 0x0017, 0x0017,
    0x0016, 0x0015, 0x0014, 0x0014, 0x0013, 0x0012, 0x0011, 0x0011,
    0x0010, 0x000F, 0x000F, 0x000E, 0x000D, 0x000D, 0x000C, 0x000C,
    0x000B, 0x000A, 0x000A, 0x0009, 0x0009, 0x0008, 0x0008, 0x0007,
    0x0007, 0x0007, 0x0006, 0x0006, 0x0005, 0x0005, 0x0005, 0x0004,
    0x0004, 0x0004, 0x0003, 0x0003, 0x0003, 0x0002, 0x0002, 0x0002,
    0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000
};


// Word index to Slot index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(word_to_slot)[AYMO_(SLOT_NUM_MAX)] =
{
     0,  1,  2, 48, 18, 19, 20, 52,
    12, 13, 14, 56, 30, 31, 32, 60,
     3,  4,  5, 49, 21, 22, 23, 53,
    15, 16, 17, 57, 33, 34, 35, 61,
     6,  7,  8, 50, 24, 25, 26, 54,
    42, 43, 44, 58, 36, 37, 38, 62,
     9, 10, 11, 51, 27, 28, 29, 55,
    45, 46, 47, 59, 39, 40, 41, 63
};

// Slot index to Word index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(slot_to_word)[AYMO_(SLOT_NUM_MAX)] =
{
     0,  1,  2, 16, 17, 18, 32, 33,
    34, 48, 49, 50,  8,  9, 10, 24,
    25, 26,  4,  5,  6, 20, 21, 22,
    36, 37, 38, 52, 53, 54, 12, 13,
    14, 28, 29, 30, 44, 45, 46, 60,
    61, 62, 40, 41, 42, 56, 57, 58,
     3, 19, 35, 51,  7, 23, 39, 55,
    11, 27, 43, 59, 15, 31, 47, 63
};


// Word index to Channel_2xOP index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(word_to_ch2x)[AYMO_(SLOT_NUM_MAX)] =
{
     0,  1,  2, 24,  9, 10, 11, 26,
     6,  7,  8, 28, 15, 16, 17, 30,
     0,  1,  2, 24,  9, 10, 11, 26,
     6,  7,  8, 28, 15, 16, 17, 30,
     3,  4,  5, 25, 12, 13, 14, 27,
    21, 22, 23, 29, 18, 19, 20, 31,
     3,  4,  5, 25, 12, 13, 14, 27,
    21, 22, 23, 29, 18, 19, 20, 31
};

// Channel_2xOP index to Word index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(ch2x_to_word)[AYMO_(SLOT_NUM_MAX) / 2][2/* slot */] =
{
    {  0, 16 },  {  1, 17 },  {  2, 18 },  { 32, 48 },
    { 33, 49 },  { 34, 50 },  {  8, 24 },  {  9, 25 },
    { 10, 26 },  {  4, 20 },  {  5, 21 },  {  6, 22 },
    { 36, 52 },  { 37, 53 },  { 38, 54 },  { 12, 28 },
    { 13, 29 },  { 14, 30 },  { 44, 60 },  { 45, 61 },
    { 46, 62 },  { 40, 56 },  { 41, 57 },  { 42, 58 },
    {  3, 19 },  { 35, 51 },  {  7, 23 },  { 39, 55 },
    { 11, 27 },  { 43, 59 },  { 15, 31 },  { 47, 63 }
};


// Word index to Channel_4xOP index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(word_to_ch4x)[AYMO_(SLOT_NUM_MAX)] =
{
     0,  1,  2, 12,  3,  4,  5, 13,
     6,  7,  8, 14,  9, 10, 11, 15,
     0,  1,  2, 12,  3,  4,  5, 13,
     6,  7,  8, 14,  9, 10, 11, 15,
     0,  1,  2, 12,  3,  4,  5, 13,
     6,  7,  8, 14,  9, 10, 11, 15,
     0,  1,  2, 12,  3,  4,  5, 13,
     6,  7,  8, 14,  9, 10, 11, 15
};

// Channel_4xOP index to Word index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(ch4x_to_word)[AYMO_(SLOT_NUM_MAX) / 4][4/* slot */] =
{
    {  0, 16, 32, 48 },  {  1, 17, 33, 49 },
    {  2, 18, 34, 50 },  {  4, 20, 36, 52 },
    {  5, 21, 37, 53 },  {  6, 22, 38, 54 },
    {  8, 24, 40, 56 },  {  9, 25, 41, 57 },
    { 10, 26, 42, 58 },  { 12, 28, 44, 60 },
    { 13, 29, 45, 61 },  { 14, 30, 46, 62 },
    {  3, 19, 35, 51 },  {  7, 23, 39, 55 },
    { 11, 27, 43, 59 },  { 15, 31, 47, 63 }
};

// Channel_4xOP index to Channel_2xOP index pairs
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(ch4x_to_pair)[AYMO_(CHANNEL_NUM_MAX) / 2][2/* slot */] =
{
    {  0,  3 },  {  1,  4 },  {  2,  5 },
    {  9, 12 },  { 10, 13 },  { 11, 14 },
    {  6, 21 },  {  7, 22 },  {  8, 23 },
    { 15, 18 },  { 16, 19 },  { 17, 20 },
    { 24, 25 },  { 26, 27 },  { 28, 29 },  { 30, 31 }
};

// Paired Channel_2xOP index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(ch2x_paired)[AYMO_(CHANNEL_NUM_MAX)] =
{
     3,  4,  5,
     0,  1,  2,
    21, 22, 23,
    12, 13, 14,
     9, 10, 11,
    18, 19, 20,
    15, 16, 17,
     6,  7,  8,
    25, 24, 27, 26,
    29, 28, 31, 30
};


// Slot group index to Channel group index
AYMO_INLINE
int aymo_(sgi_to_cgi)(int sgi)
{
//    return (((sgi / 4) * 2) | (sgi % 2));
    return (((sgi >> 1) & 2) | (sgi & 1));
}


// Sub-address to Slot index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(subaddr_to_slot)[AYMO_(SLOT_NUM_MAX)] =
{
     0,  1,  2,  3,  4,  5, 48, 49,
     6,  7,  8,  9, 10, 11, 50, 51,
    12, 13, 14, 15, 16, 17, 52, 53,
    36, 37, 38, 39, 40, 41, 54, 55,

    18, 19, 20, 21, 22, 23, 56, 57,
    24, 25, 26, 27, 28, 29, 58, 59,
    30, 31, 32, 33, 34, 35, 60, 61,
    42, 43, 44, 45, 46, 47, 62, 63
};

// Address to Slot index
AYMO_INLINE
int8_t aymo_(addr_to_slot)(uint16_t address)
{
    uint16_t subaddr = ((address & 0x1F) | ((address >> 8) & 1));
    int8_t slot = aymo_(subaddr_to_slot)[subaddr];
    return slot;
}

// TODO: slot_to_addr[]


// Sub-addres to Channel_2xOP index
AYMO_STATIC AYMO_ALIGN_V16
const int8_t aymo_(subaddr_to_ch2x)[AYMO_(CHANNEL_NUM_MAX)] =
{
     0,  1,  2,  3,  4,  5,  6,  7,  8,
    18, 19, 20, 21, 22, 23, 24,

     9, 10, 11, 12, 13, 14, 15, 16, 17,
    25, 26, 27, 28, 29, 30, 31
};

// Address to Channel_2xOP index
AYMO_INLINE
int8_t aymo_(addr_to_ch2x)(uint16_t address)
{
    uint16_t subaddr = ((address & 0x0F) | ((address >> 8) & 1));
    int8_t ch2x = aymo_(subaddr_to_ch2x)[subaddr];
    return ch2x;
}

// TODO: ch2x_to_addr[]


AYMO_STATIC
const int8_t aymo_(pg_mult_x2_table)[16] =
{
    1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30
};


AYMO_STATIC
const int8_t aymo_(eg_ksl_table)[16] =
{
    0, 32, 40, 45, 48, 51, 53, 55, 56, 58, 59, 60, 61, 62, 63, 64
};

AYMO_STATIC
const int8_t aymo_(eg_kslsh_table)[4] =
{
    8, 1, 2, 0
};

AYMO_STATIC
const uint16_t aymo_(eg_incstep_table)[4] =
{
    ((1 << 3) | (1 << 2) | (1 << 1) | (0 << 0)),
    ((1 << 3) | (0 << 2) | (0 << 1) | (0 << 0)),
    ((1 << 3) | (1 << 2) | (0 << 1) | (0 << 0)),
    ((0 << 3) | (0 << 2) | (0 << 1) | (0 << 0))
};


// Wave descriptors
AYMO_STATIC AYMO_ALIGN_V16
const struct aymo_(wave) aymo_(wave_table)[8] =
{
    { 0,  0x0000,  0x0200,  0x0100,  0x00FF,  -1 },
    { 0,  0x0200,  0x0000,  0x0100,  0x00FF,  -1 },
    { 0,  0x0000,  0x0000,  0x0100,  0x00FF,  -1 },
    { 0,  0x0100,  0x0000,  0x0100,  0x00FF,  -1 },
    { 1,  0x0400,  0x0200,  0x0100,  0x00FF,  -1 },
    { 1,  0x0400,  0x0000,  0x0100,  0x00FF,  -1 },
    { 0,  0x0000,  0x0200,  0x0200,  0x0001,   0 },
    { 3,  0x0000,  0x1000,  0x1000,  0x1FFF,   0 }
};


// 2-channel connection descriptors
AYMO_STATIC AYMO_ALIGN_V16
const struct aymo_(conn) aymo_(conn_ch2x_table)[2/* cnt */][2/* slot */] =
{
    {
        { -1,   0,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,  -1 },
        {  0,   0,  -1 }
    },
};

// 4-channel connection descriptors
AYMO_STATIC AYMO_ALIGN_V16
const struct aymo_(conn) aymo_(conn_ch4x_table)[4/* cnt */][4/* slot */] =
{
    {
        { -1,   0,   0 },
        {  0,  -1,  -0 },
        {  0,  -1,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,   0 },
        {  0,  -1,  -1 },
        {  0,   0,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,  -1 },
        {  0,   0,   0 },
        {  0,  -1,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,  -1 },
        {  0,   0,   0 },
        {  0,  -1,  -1 },
        {  0,   0,  -1 }
    },
};

// Rhythm connection descriptors
AYMO_STATIC AYMO_ALIGN_V16
const struct aymo_(conn) aymo_(conn_ryt_table)[4][2/* slot */] =
{
    // Channel 6: BD, FM
    {
        { -1,   0,   0 },
        {  0,  -1,  -1 }
    },
    // Channel 6: BD, AM
    {
        { -1,   0,   0 },
        {  0,   0,  -1 }
    },
    // Channel 7: HH + SD
    {
        {  0,   0,  -1 },
        {  0,   0,  -1 }
    },
    // Channel 8: TT + TC
    {
        {  0,   0,  -1 },
        {  0,   0,  -1 }
    }
};


// Slot mask output delay for outputs A and C
AYMO_STATIC
const uint8_t aymo_(og_prout_ac)[AYMO_(SLOT_GROUP_NUM)] =
{
    0xF8,
    0xF8,
    0xF8,
    0xFF,
    0xF8,
    0xFF,
    0xF8,
    0xFF
};


// Slot mask output delay for outputs B and D
AYMO_STATIC
const uint8_t aymo_(og_prout_bd)[AYMO_(SLOT_GROUP_NUM)] =
{
    0x88,
    0xF8,
    0x88,
    0xF8,
    0x88,
    0xFF,
    0x88,
    0xFF
};


// Updates wave generators
AYMO_INLINE
void aymo_(wg_update)(
    struct aymo_(chip)* chip,
    struct aymo_(ch2x_group)* cg,
    struct aymo_(slot_group)* sg
)
{
    // Compute feedback and modulation inputs
    aymoi16_t fbsum = vslli(vadd(sg->wg_out, sg->wg_prout), 1);
    aymoi16_t fbsum_sh = vsrlv(fbsum, sg->wg_fb_shr);
    aymoi16_t prmod = vand(chip->wg_mod, sg->wg_prmod_gate);
    aymoi16_t fbmod = vand(fbsum_sh, sg->wg_fbmod_gate);
    sg->wg_prout = sg->wg_out;

    // Compute operator phase input
    aymoi16_t modsum = vadd(fbmod, prmod);
    aymoi16_t phase = vadd(sg->pg_phase_out, modsum);

    // Process phase
    aymoi16_t phase_sped = vsllv(phase, sg->wg_phase_shl);
    aymoi16_t phase_gate = vcmpz(vand(phase_sped, sg->wg_phase_zero));
    aymoi16_t phase_flip = vcmpp(vand(phase_sped, sg->wg_phase_flip));
    aymoi16_t phase_mask = sg->wg_phase_mask;
    aymoi16_t phase_xor = vand(phase_flip, phase_mask);
    aymoi16_t phase_idx = vxor(phase_sped, phase_xor);
    aymoi16_t phase_out = vand(vand(phase_gate, phase_mask), phase_idx);

    // Compute logsin variant
    aymoi16_t phase_lo = phase_out;  // vgather() masks to low byte
    aymoi16_t logsin_val = vgather(aymo_(logsin_table), phase_lo);
    logsin_val = vblendv(vset1(0x1000), logsin_val, phase_gate);

    // Compute exponential output
    aymoi16_t exp_in = vblendv(phase_out, logsin_val, sg->wg_sine_gate);
    aymoi16_t exp_level = vadd(exp_in, vslli(sg->eg_out, 3));
    exp_level = vu2i(vminu(vi2u(exp_level), vi2u(vset1(0x1FFF))));
    aymoi16_t exp_level_lo = exp_level;  // vgather() masks to low byte
    aymoi16_t exp_level_hi = vsrli(exp_level, 8);
    aymoi16_t exp_value = vgather(aymo_(exp_x2_table), exp_level_lo);
    aymoi16_t exp_out = vsrlv(exp_value, exp_level_hi);

    // Compute operator wave output
    aymoi16_t wave_pos = vcmpz(vand(phase_sped, sg->wg_phase_neg));
    aymoi16_t wave_neg = vandnot(wave_pos, phase_gate);
    aymoi16_t wave_out = vxor(exp_out, wave_neg);
    sg->wg_out = wave_out;
    chip->wg_mod = wave_out;

    // Update chip output accumulators, with quirky slot output delay
    aymoi16_t og_out_ac = vblendv(wave_out, sg->og_prout, sg->og_prout_ac);
    aymoi16_t og_out_bd = vblendv(wave_out, sg->og_prout, sg->og_prout_bd);
    sg->og_prout = wave_out;
    chip->og_acc_a = vadd(chip->og_acc_a, vand(og_out_ac, sg->og_out_ch_gate_a));
    chip->og_acc_c = vadd(chip->og_acc_c, vand(og_out_ac, sg->og_out_ch_gate_c));
    chip->og_acc_b = vadd(chip->og_acc_b, vand(og_out_bd, sg->og_out_ch_gate_b));
    chip->og_acc_d = vadd(chip->og_acc_d, vand(og_out_bd, sg->og_out_ch_gate_d));

#ifdef AYMO_DEBUG
    sg->wg_fbmod = fbsum_sh;
    sg->wg_mod = modsum;
#endif
}


// Updates envelope generators
AYMO_INLINE
void aymo_(eg_update)(
    struct aymo_(chip)* chip,
    struct aymo_(ch2x_group)* cg,
    struct aymo_(slot_group)* sg
)
{
    (void)cg;

    // Compute envelope output
    sg->eg_out = vadd(
        vadd(sg->eg_rout, sg->eg_tl_x4),
        vadd(sg->eg_ksl_sh, sg->eg_tremolo_am)
    );

    // Compute rate
    aymoi16_t eg_gen_rel = vcmpeq(sg->eg_gen, vset1(AYMO_(EG_GEN_RELEASE)));
    aymoi16_t notreset = vcmpz(vand(sg->eg_key, eg_gen_rel));
    sg->pg_notreset = notreset;
    aymoi16_t eg_gen_shl = vblendv(vset1(AYMO_(EG_GEN_SHL_ATTACK)), sg->eg_gen_shl, notreset);
    aymoi16_t reg_rate = vsllv(sg->eg_adsr, eg_gen_shl);  // move to top nibble
    aymoi16_t rate_temp = vand(reg_rate, vset1((int16_t)0xF000));  // keep top nibble
    rate_temp = vsrli(rate_temp, AYMO_(EG_GEN_SRLHI));
    aymoi16_t rate = vadd(sg->eg_ks, rate_temp);
    aymoi16_t rate_lo = vand(rate, vset1(3));
    aymoi16_t rate_hi = vsrli(rate, 2);
    rate_hi = vminu(rate_hi, vset1(15));

    // Compute shift
    aymoi16_t eg_shift = vadd(rate_hi, chip->eg_add);
    aymoi16_t rate_pre_lt12 = vor(vslli(rate_lo, 1), vset1(8));
    aymoi16_t shift_lt12 = vsrlv(rate_pre_lt12, vsubsu(vset1(15), eg_shift));
    shift_lt12 = vand(shift_lt12, chip->eg_statev);

    aymoi16_t incstep_ge12 = vand(vsrlv(chip->eg_incstep, rate_lo), vset1(1));
    aymoi16_t shift_ge12 = vadd(vand(rate_hi, vset1(3)), incstep_ge12);
    shift_ge12 = vminu(shift_ge12, vset1(3));
    shift_ge12 = vblendv(shift_ge12, chip->eg_statev, vcmpz(shift_ge12));

    aymoi16_t shift = vblendv(shift_lt12, shift_ge12, vcmpgt(rate_hi, vset1(11)));
    shift = vandnot(vcmpz(rate_temp), shift);

    // Instant attack
    aymoi16_t eg_rout = sg->eg_rout;
    eg_rout = vandnot(vandnot(notreset, vcmpeq(rate_hi, vset1(15))), eg_rout);

    // Envelope off
    aymoi16_t eg_off = vcmpgt(sg->eg_rout, vset1(0x01F7));
    aymoi16_t eg_gen_natk_and_nrst = vand(vcmpp(sg->eg_gen), notreset);
    eg_rout = vblendv(eg_rout, vset1(0x01FF), vand(eg_gen_natk_and_nrst, eg_off));

    // Compute common increment not in attack state
    aymoi16_t eg_inc_natk_cond = vand(vand(notreset, vcmpz(eg_off)), vcmpp(shift));
    aymoi16_t eg_inc_natk = vand(eg_inc_natk_cond, vpow2m1lt4(shift));
    aymoi16_t eg_gen = sg->eg_gen;

    // Move attack to decay state
    aymoi16_t eg_inc_atk_cond = vand(vand(vcmpp(sg->eg_key), vcmpp(shift)),
                                     vand(vcmpz(sg->eg_gen), vcmpgt(vset1(15), rate_hi)));
    aymoi16_t eg_inc_atk_ninc = vsrlv(sg->eg_rout, vsub(vset1(4), shift));
    aymoi16_t eg_inc = vandnot(eg_inc_atk_ninc, eg_inc_atk_cond);
    aymoi16_t eg_gen_atk_to_dec = vcmpz(vor(sg->eg_gen, sg->eg_rout));
    eg_gen = vsub(eg_gen, eg_gen_atk_to_dec);  // 0 --> 1
    eg_inc = vblendv(eg_inc_natk, eg_inc, vcmpz(sg->eg_gen));
    eg_inc = vandnot(eg_gen_atk_to_dec, eg_inc);

    // Move decay to sustain state
    aymoi16_t eg_gen_dec = vcmpeq(sg->eg_gen, vset1(AYMO_(EG_GEN_DECAY)));
    aymoi16_t sl_hit = vcmpeq(vsrli(sg->eg_rout, 4), sg->eg_sl);
    aymoi16_t eg_gen_dec_to_sus = vand(eg_gen_dec, sl_hit);
    eg_gen = vsub(eg_gen, eg_gen_dec_to_sus);  // 1 --> 2
    eg_inc = vandnot(eg_gen_dec_to_sus, eg_inc);

    // Move back to attack state
    eg_gen = vand(notreset, eg_gen);  // * --> 0

    // Move to release state
    eg_gen = vor(eg_gen, vsrli(vcmpz(sg->eg_key), 14));  // * --> 3

    // Update envelope generator
    eg_rout = vadd(eg_rout, eg_inc);
    eg_rout = vand(eg_rout, vset1(0x01FF));
    sg->eg_rout = eg_rout;
    sg->eg_gen = eg_gen;
    sg->eg_gen_shl = vslli(eg_gen, 2);

#ifdef AYMO_DEBUG
    sg->eg_rate = rate;
    sg->eg_inc = eg_inc;
#endif
}


// Updates phase generator
AYMO_INLINE
void aymo_(pg_update_deltafreq)(
    struct aymo_(chip)* chip,
    struct aymo_(ch2x_group)* cg,
    struct aymo_(slot_group)* sg
)
{
    // Update phase
    aymoi16_t fnum = cg->pg_fnum;
    aymoi16_t range = vand(fnum, vset1(7 << 7));
    range = vand(sg->pg_vib, vsllv(range, chip->pg_vib_shs));
    range = vmul(range, chip->pg_vib_sign);
    fnum = vadd(fnum, range);

    aymoi32_t fnum_lo = vunpacklo(fnum);
    aymoi32_t fnum_hi = vunpackhi(fnum);
    aymoi32_t block_sll_lo = vunpacklo(cg->pg_block);
    aymoi32_t block_sll_hi = vunpackhi(cg->pg_block);
    aymoi32_t basefreq_lo = vvsrli(vvsllv(fnum_lo, block_sll_lo), 1);
    aymoi32_t basefreq_hi = vvsrli(vvsllv(fnum_hi, block_sll_hi), 1);
    aymoi32_t pg_mult_x2_lo = vunpacklo(sg->pg_mult_x2);
    aymoi32_t pg_mult_x2_hi = vunpackhi(sg->pg_mult_x2);
    aymoi32_t deltafreq_lo = vvsrli(vvmul(basefreq_lo, pg_mult_x2_lo), 1);
    aymoi32_t deltafreq_hi = vvsrli(vvmul(basefreq_hi, pg_mult_x2_hi), 1);
    sg->pg_deltafreq_lo = deltafreq_lo;
    sg->pg_deltafreq_hi = deltafreq_hi;
}

// Updates phase generator
AYMO_INLINE
void aymo_(pg_update)(
    struct aymo_(chip)* chip,
    struct aymo_(ch2x_group)* cg,
    struct aymo_(slot_group)* sg
)
{
    (void)cg;

    // Compute phase output
    aymoi32_t phase_out_mask = vvset1(0xFFFF);
    aymoi32_t phase_out_lo = vvand(vvsrli(sg->pg_phase_lo, 9), phase_out_mask);
    aymoi32_t phase_out_hi = vvand(vvsrli(sg->pg_phase_hi, 9), phase_out_mask);
    aymoi16_t phase_out = vvpack(phase_out_lo, phase_out_hi);
    sg->pg_phase_out = phase_out;

    // Update phase
    aymoi32_t notreset_lo = vunpacklo(sg->pg_notreset);
    aymoi32_t notreset_hi = vunpackhi(sg->pg_notreset);
    aymoi32_t pg_phase_lo = vvand(notreset_lo, sg->pg_phase_lo);
    aymoi32_t pg_phase_hi = vvand(notreset_hi, sg->pg_phase_hi);
    sg->pg_phase_lo = vvadd(pg_phase_lo, sg->pg_deltafreq_lo);
    sg->pg_phase_hi = vvadd(pg_phase_hi, sg->pg_deltafreq_hi);
}


// Updates noise generator
AYMO_INLINE
void aymo_(ng_update)(struct aymo_(chip)* chip, unsigned times)
{
    // Update noise
    uint32_t noise = chip->ng_noise;
    while (times--) {
        uint32_t n_bit = (((noise >> 14) ^ noise) & 1);
        noise = ((noise >> 1) | (n_bit << 22));
    }
    chip->ng_noise = noise;
}


// Updates rhythm manager, slot group 1
AYMO_INLINE
void aymo_(rm_update_sg1)(struct aymo_(chip)* chip)
{
    struct aymo_(slot_group)* sg = &chip->sg[1];

    if (chip->chip_regs.reg_BDh.ryt) {
        // Double rhythm outputs
        aymoi16_t ryt_slot_mask = vsetr(-1, -1, -1, 0, 0, 0, 0, 0);
        aymoi16_t wave_out = vand(sg->wg_out, ryt_slot_mask);
        chip->og_acc_a = vadd(chip->og_acc_a, vand(wave_out, sg->og_out_ch_gate_a));
        chip->og_acc_b = vadd(chip->og_acc_b, vand(wave_out, sg->og_out_ch_gate_b));
        chip->og_acc_c = vadd(chip->og_acc_c, vand(wave_out, sg->og_out_ch_gate_c));
        chip->og_acc_d = vadd(chip->og_acc_d, vand(wave_out, sg->og_out_ch_gate_d));
    }

    aymoi16_t phase = sg->pg_phase_out;
    uint16_t phase13 = (uint16_t)vextract(phase, 1);

    // Update noise bits
    chip->rm_hh_bit2 = ((phase13 >> 2) & 1);
    chip->rm_hh_bit3 = ((phase13 >> 3) & 1);
    chip->rm_hh_bit7 = ((phase13 >> 7) & 1);
    chip->rm_hh_bit8 = ((phase13 >> 8) & 1);

    if (chip->chip_regs.reg_BDh.ryt) {
        // Calculate noise bit
        uint16_t rm_xor = (
            (chip->rm_hh_bit2 ^ chip->rm_hh_bit7) |
            (chip->rm_hh_bit3 ^ chip->rm_tc_bit5) |
            (chip->rm_tc_bit3 ^ chip->rm_tc_bit5)
        );

        // Update HH
        uint16_t noise = (uint16_t)chip->ng_noise;
        phase13 = (rm_xor << 9);
        if (rm_xor ^ (noise & 1)) {
            phase13 |= 0xD0;
        } else {
            phase13 |= 0x34;
        }
        phase = vinsert(phase, (int16_t)phase13, 1);

        sg->pg_phase_out = phase;
    }
}


// Updates rhythm manager, slot group 3
AYMO_INLINE
void aymo_(rm_update_sg3)(struct aymo_(chip)* chip)
{
    struct aymo_(slot_group)* sg = &chip->sg[3];

    if (chip->chip_regs.reg_BDh.ryt) {
        // Double rhythm outputs
        aymoi16_t ryt_slot_mask = vsetr(-1, -1, -1, 0, 0, 0, 0, 0);
        aymoi16_t wave_out = vand(sg->wg_out, ryt_slot_mask);
        chip->og_acc_a = vadd(chip->og_acc_a, vand(wave_out, sg->og_out_ch_gate_a));
        chip->og_acc_b = vadd(chip->og_acc_b, vand(wave_out, sg->og_out_ch_gate_b));
        chip->og_acc_c = vadd(chip->og_acc_c, vand(wave_out, sg->og_out_ch_gate_c));
        chip->og_acc_d = vadd(chip->og_acc_d, vand(wave_out, sg->og_out_ch_gate_d));

        // Calculate noise bit
        uint16_t rm_xor = (
            (chip->rm_hh_bit2 ^ chip->rm_hh_bit7) |
            (chip->rm_hh_bit3 ^ chip->rm_tc_bit5) |
            (chip->rm_tc_bit3 ^ chip->rm_tc_bit5)
        );
        aymoi16_t phase = sg->pg_phase_out;

        // Update SD
        uint16_t noise = (uint16_t)chip->ng_noise;
        uint16_t phase16 = (
            ((uint16_t)chip->rm_hh_bit8 << 9) |
            ((uint16_t)(chip->rm_hh_bit8 ^ (noise & 1)) << 8)
        );
        phase = vinsert(phase, (int16_t)phase16, 1);

        // Update TC
        uint32_t phase17 = vextract(phase, 2);
        chip->rm_tc_bit3 = ((phase17 >> 3) & 1);
        chip->rm_tc_bit5 = ((phase17 >> 5) & 1);
        phase17 = ((rm_xor << 9) | 0x80);
        phase = vinsert(phase, (int16_t)phase17, 2);

        sg->pg_phase_out = phase;
    }
}


// Updates slot generators
AYMO_STATIC
void aymo_(sg_update)(
    struct aymo_(chip)* chip,
    struct aymo_(ch2x_group)* cg,
    struct aymo_(slot_group)* sg
)
{
    aymo_(eg_update)(chip, cg, sg);
    aymo_(pg_update)(chip, cg, sg);
    aymo_(wg_update)(chip, cg, sg);
}


// Clear output accumulators
AYMO_INLINE
void aymo_(og_clear)(struct aymo_(chip)* chip)
{
    chip->og_acc_a = vsetz();
    chip->og_acc_b = vsetz();
    chip->og_acc_c = vsetz();
    chip->og_acc_d = vsetz();
}


// Updates output mixdown
AYMO_INLINE
void aymo_(og_update)(struct aymo_(chip)* chip)
{
    chip->og_sum_a = vhsum(chip->og_acc_a);
    chip->og_sum_b = vhsum(chip->og_acc_b);
    chip->og_sum_c = vhsum(chip->og_acc_c);
    chip->og_sum_d = vhsum(chip->og_acc_d);

    chip->og_out_a = clamp16(chip->og_sum_a);
    chip->og_out_b = chip->og_del_b;
    chip->og_del_b = clamp16(chip->og_sum_b);
    chip->og_out_c = clamp16(chip->og_sum_c);
    chip->og_out_d = chip->og_del_d;
    chip->og_del_d = clamp16(chip->og_sum_d);
}


// Updates timer management
AYMO_INLINE
void aymo_(tm_update)(struct aymo_(chip)* chip)
{
    // Update tremolo
    if ((chip->tm_timer & 0x3F) == 0x3F) {
        chip->eg_tremolopos = ((chip->eg_tremolopos + 1) % 210);

        uint16_t eg_tremolopos = chip->eg_tremolopos;
        if (eg_tremolopos >= 105) {
            eg_tremolopos = (210 - eg_tremolopos);
        }
        aymoi16_t eg_tremolo = vset1((int16_t)(eg_tremolopos >> chip->eg_tremoloshift));

        for (int sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
            struct aymo_(slot_group)* sg = &chip->sg[sgi];
            sg->eg_tremolo_am = vand(eg_tremolo, sg->eg_am);
        }
    }

    // Update vibrato
    if ((chip->tm_timer & 0x3FF) == 0x3FF) {
        chip->pg_vibpos = ((chip->pg_vibpos + 1) & 7);
        uint8_t vibpos = chip->pg_vibpos;
        int16_t pg_vib_shs = -7;
        int16_t pg_vib_sign = +1;

        if (!(vibpos & 3)) {
            pg_vib_shs = +16;  // nullify
        }
        else if (vibpos & 1) {
            pg_vib_shs -= 1;
        }
        pg_vib_shs -= (int16_t)(uint16_t)chip->eg_vibshift;

        if (vibpos & 4) {
            pg_vib_sign = -1;
        }
        chip->pg_vib_shs = vset1(pg_vib_shs);
        chip->pg_vib_sign = vset1(pg_vib_sign);

        for (int sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
            int cgi = aymo_(sgi_to_cgi)(sgi);
            struct aymo_(ch2x_group)* cg = &chip->cg[cgi];
            struct aymo_(slot_group)* sg = &chip->sg[sgi];
            aymo_(pg_update_deltafreq)(chip, cg, sg);
        }
    }

    chip->tm_timer++;
    uint16_t eg_incstep = aymo_(eg_incstep_table)[chip->tm_timer & 3];
    chip->eg_incstep = vi2u(vset1((int16_t)eg_incstep));

    // Update timed envelope patterns
    int16_t eg_shift = (int16_t)ffsll((long long)chip->eg_timer);
    int16_t eg_add = ((eg_shift > 13) ? 0 : eg_shift);
    chip->eg_add = vset1(eg_add);

    // Update envelope timer and flip state
    if (chip->eg_state || ((chip->eg_timer & AYMO_(EG_TIMER_MASK)) == 0)) {
        chip->eg_timer = (((chip->eg_timer + 1) & AYMO_(EG_TIMER_MASK)) | AYMO_(EG_TIMER_HIBIT));
    }
    chip->eg_state ^= 1;
    chip->eg_statev = vset1((int16_t)chip->eg_state);
}


// Updates the register queue
AYMO_INLINE
void aymo_(rq_update)(struct aymo_(chip)* chip)
{
    if (chip->rq_delay) {
        if (--chip->rq_delay) {
            return;
        }
    }
    if (chip->rq_head != chip->rq_tail) {
        struct aymo_(reg_queue_item)* item = &chip->rq_buffer[chip->rq_head];

        if (item->address & 0x8000U) {
            chip->rq_delay = AYMO_(REG_QUEUE_LATENCY);
            chip->rq_delay += (((uint32_t)(item->address & 0x7FFFU) << 16) | item->value);
        }
        else {
            aymo_(write)(chip, item->address, item->value);
        }

        if (++chip->rq_head >= AYMO_(REG_QUEUE_LENGTH)) {
            chip->rq_head = 0;
        }
    }
}


// Exceutes a single processing tick
void aymo_(tick)(struct aymo_(chip)* chip)
{
    int sgi;
    int cgi;

    // Clear output accumulators
    aymo_(og_clear)(chip);

    // Process slot group 0
    sgi = 0;
    cgi = aymo_(sgi_to_cgi)(sgi);
    aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);

    // Process slot group 2
    sgi = 2;
    cgi = aymo_(sgi_to_cgi)(sgi);
    aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);

    // Process slot group 4
    sgi = 4;
    cgi = aymo_(sgi_to_cgi)(sgi);
    aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);

    // Process slot group 6
    sgi = 6;
    cgi = aymo_(sgi_to_cgi)(sgi);
    aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);

    // Process slot group 1
    sgi = 1;
    cgi = aymo_(sgi_to_cgi)(sgi);
    aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);
    aymo_(ng_update)(chip, (36 - 3));  // slot 16 --> slot 13
    aymo_(rm_update_sg1)(chip);

    // Process slot group 3
    sgi = 3;
    cgi = aymo_(sgi_to_cgi)(sgi);
    aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);
    aymo_(ng_update)(chip, 3);  // slot 13 --> slot 16
    aymo_(rm_update_sg3)(chip);

    if (chip->process_all_slots) {
        // Process slot group 5
        sgi = 5;
        cgi = aymo_(sgi_to_cgi)(sgi);
        aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);

        // Process slot group 7
        sgi = 7;
        cgi = aymo_(sgi_to_cgi)(sgi);
        aymo_(sg_update)(chip, &chip->cg[cgi], &chip->sg[sgi]);
    }

    // Update outputs
    aymo_(og_update)(chip);

    // Update timers
    aymo_(tm_update)(chip);

    // Dequeue registers
    aymo_(rq_update)(chip);
}


AYMO_STATIC
void aymo_(eg_update_ksl)(struct aymo_(chip)* chip, int word)
{
    int slot = aymo_(word_to_slot)[word];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi);
    struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_(reg_40h)* reg_40h = &(chip->slot_regs[slot].reg_40h);

    int16_t pg_fnum = vextractn(cg->pg_fnum, sgo);
    int16_t pg_fnum_hn = ((pg_fnum >> 6) & 15);

    int ch2x = aymo_(word_to_ch2x)[aymo_(slot_to_word)[slot]];
    int16_t eg_block = (int16_t)(chip->ch2x_regs[ch2x].reg_B0h.block);
    int16_t eg_ksl = aymo_(eg_ksl_table)[pg_fnum_hn];
    eg_ksl = ((eg_ksl << 2) - ((8 - eg_block) << 5));
    if (eg_ksl < 0) {
        eg_ksl = 0;
    }
    int16_t eg_kslsh = aymo_(eg_kslsh_table)[reg_40h->ksl];

    int16_t eg_ksl_sh = (eg_ksl >> eg_kslsh);
    sg->eg_ksl_sh = vinsertn(sg->eg_ksl_sh, eg_ksl_sh, sgo);

#ifdef AYMO_DEBUG
    sg->eg_ksl = vinsertn(sg->eg_ksl, eg_ksl, sgo);
#endif
}


AYMO_STATIC
void aymo_(chip_pg_update_nts)(struct aymo_(chip)* chip)
{
    for (int slot = 0; slot < AYMO_(SLOT_NUM_MAX); ++slot) {
        int word = aymo_(slot_to_word)[slot];
        int ch2x = aymo_(word_to_ch2x)[word];
        struct aymo_(reg_A0h)* reg_A0h = &(chip->ch2x_regs[ch2x].reg_A0h);
        struct aymo_(reg_B0h)* reg_B0h = &(chip->ch2x_regs[ch2x].reg_B0h);
        struct aymo_(reg_08h)* reg_08h = &(chip->chip_regs.reg_08h);
        int16_t pg_fnum = (int16_t)(reg_A0h->fnum_lo | ((uint16_t)reg_B0h->fnum_hi << 8));
        int16_t eg_ksv = ((reg_B0h->block << 1) | ((pg_fnum >> (9 - reg_08h->nts)) & 1));

        int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
        int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
        int cgi = aymo_(sgi_to_cgi)(sgi);
        struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
        struct aymo_(slot_group)* sg = &(chip->sg[sgi]);

        struct aymo_(reg_20h)* reg_20h = &(chip->slot_regs[slot].reg_20h);
        int16_t ks = (eg_ksv >> ((reg_20h->ksr ^ 1) << 1));

        cg->eg_ksv = vinsertn(cg->eg_ksv, eg_ksv, sgo);
        sg->eg_ks  = vinsertn(sg->eg_ks,  ks,     sgo);
    }
}


AYMO_STATIC
void aymo_(pg_update_fnum)(
    struct aymo_(chip)* chip, int ch2x,
    int16_t pg_fnum, int16_t eg_ksv, int16_t pg_block
)
{
    int word0 = aymo_(ch2x_to_word)[ch2x][0];
    int sgi0 = (word0 / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word0 % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi0);
    struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);

    cg->pg_block = vinsertn(cg->pg_block, pg_block, sgo);
    cg->pg_fnum = vinsertn(cg->pg_fnum, pg_fnum, sgo);
    cg->eg_ksv = vinsertn(cg->eg_ksv, eg_ksv, sgo);

    struct aymo_(slot_group)* sg0 = &(chip->sg[sgi0]);
    int slot0 = aymo_(word_to_slot)[word0];
    struct aymo_(reg_20h)* reg_20h0 = &(chip->slot_regs[slot0].reg_20h);
    int16_t ks0 = (eg_ksv >> ((reg_20h0->ksr ^ 1) << 1));
    sg0->eg_ks = vinsertn(sg0->eg_ks, ks0, sgo);
    aymo_(eg_update_ksl)(chip, word0);
    aymo_(pg_update_deltafreq)(chip, cg, sg0);

    int word1 = aymo_(ch2x_to_word)[ch2x][1];
    int sgi1 = (word1 / AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg1 = &(chip->sg[sgi1]);
    int slot1 = aymo_(word_to_slot)[word1];
    struct aymo_(reg_20h)* reg_20h1 = &(chip->slot_regs[slot1].reg_20h);
    int16_t ks1 = (eg_ksv >> ((reg_20h1->ksr ^ 1) << 1));
    sg1->eg_ks = vinsertn(sg1->eg_ks, ks1, sgo);
    aymo_(eg_update_ksl)(chip, word1);
    aymo_(pg_update_deltafreq)(chip, cg, sg1);
}


AYMO_STATIC
void aymo_(ch2x_update_fnum)(struct aymo_(chip)* chip, int ch2x, int8_t ch2p)
{
    struct aymo_(reg_A0h)* reg_A0h = &(chip->ch2x_regs[ch2x].reg_A0h);
    struct aymo_(reg_B0h)* reg_B0h = &(chip->ch2x_regs[ch2x].reg_B0h);
    struct aymo_(reg_08h)* reg_08h = &(chip->chip_regs.reg_08h);
    int16_t pg_fnum = (int16_t)(reg_A0h->fnum_lo | ((uint16_t)reg_B0h->fnum_hi << 8));
    int16_t pg_block = (int16_t)reg_B0h->block;
    int16_t eg_ksv = ((pg_block << 1) | ((pg_fnum >> (9 - reg_08h->nts)) & 1));

    aymo_(pg_update_fnum)(chip, ch2x, pg_fnum, eg_ksv, pg_block);

    if (ch2p >= 0) {
        aymo_(pg_update_fnum)(chip, ch2p, pg_fnum, eg_ksv, pg_block);
    }
}


AYMO_INLINE
void aymo_(eg_key_on)(struct aymo_(chip)* chip, int word, int16_t mode)
{
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &chip->sg[sgi];
    int16_t eg_key = vextractn(sg->eg_key, sgo);
    eg_key |= mode;
    sg->eg_key = vinsertn(sg->eg_key, eg_key, sgo);
}


AYMO_INLINE
void aymo_(eg_key_off)(struct aymo_(chip)* chip, int word, int16_t mode)
{
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &chip->sg[sgi];
    int16_t eg_key = vextractn(sg->eg_key, sgo);
    eg_key &= ~mode;
    sg->eg_key = vinsertn(sg->eg_key, eg_key, sgo);
}


AYMO_STATIC
void aymo_(ch2x_key_on)(struct aymo_(chip)* chip, int ch2x)
{
    if (chip->chip_regs.reg_105h.newm) {
        unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
        unsigned ch2x_is_drum    = (chip->og_ch2x_drum    & (1UL << ch2x));
        int ch2p = aymo_(ch2x_paired)[ch2x];
        int ch2x_is_secondary = (ch2p < ch2x);

        if (ch2x_is_pairing && !ch2x_is_secondary) {
            int ch2x_word0 = aymo_(ch2x_to_word)[ch2x][0];
            int ch2x_word1 = aymo_(ch2x_to_word)[ch2x][1];
            int ch2p_word0 = aymo_(ch2x_to_word)[ch2p][0];
            int ch2p_word1 = aymo_(ch2x_to_word)[ch2p][1];
            aymo_(eg_key_on)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2p_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2p_word1, AYMO_(EG_KEY_NORMAL));
        }
        else if (!ch2x_is_pairing || ch2x_is_drum) {
            int ch2x_word0 = aymo_(ch2x_to_word)[ch2x][0];
            int ch2x_word1 = aymo_(ch2x_to_word)[ch2x][1];
            aymo_(eg_key_on)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
        }
    }
    else {
        int ch2x_word0 = aymo_(ch2x_to_word)[ch2x][0];
        int ch2x_word1 = aymo_(ch2x_to_word)[ch2x][1];
        aymo_(eg_key_on)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
        aymo_(eg_key_on)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
    }
}


AYMO_STATIC
void aymo_(ch2x_key_off)(struct aymo_(chip)* chip, int ch2x)
{
    if (chip->chip_regs.reg_105h.newm) {
        unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
        unsigned ch2x_is_drum    = (chip->og_ch2x_drum    & (1UL << ch2x));
        int ch2p = aymo_(ch2x_paired)[ch2x];
        int ch2x_is_secondary = (ch2p < ch2x);

        if (ch2x_is_pairing && !ch2x_is_secondary) {
            int ch2x_word0 = aymo_(ch2x_to_word)[ch2x][0];
            int ch2x_word1 = aymo_(ch2x_to_word)[ch2x][1];
            int ch2p_word0 = aymo_(ch2x_to_word)[ch2p][0];
            int ch2p_word1 = aymo_(ch2x_to_word)[ch2p][1];
            aymo_(eg_key_off)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2p_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2p_word1, AYMO_(EG_KEY_NORMAL));
        }
        else if (!ch2x_is_pairing || ch2x_is_drum) {
            int ch2x_word0 = aymo_(ch2x_to_word)[ch2x][0];
            int ch2x_word1 = aymo_(ch2x_to_word)[ch2x][1];
            aymo_(eg_key_off)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
        }
    }
    else {
        int ch2x_word0 = aymo_(ch2x_to_word)[ch2x][0];
        int ch2x_word1 = aymo_(ch2x_to_word)[ch2x][1];
        aymo_(eg_key_off)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
        aymo_(eg_key_off)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
    }
}


AYMO_STATIC
void aymo_(cm_rewire_slot)(struct aymo_(chip)* chip, int word, const struct aymo_(conn)* conn)
{
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &chip->sg[sgi];
    sg->wg_fbmod_gate = vinsertn(sg->wg_fbmod_gate, conn->wg_fbmod_gate, sgo);
    sg->wg_prmod_gate = vinsertn(sg->wg_prmod_gate, conn->wg_prmod_gate, sgo);
    sg->og_out_gate   = vinsertn(sg->og_out_gate,   conn->og_out_gate,   sgo);

    int cgi = aymo_(sgi_to_cgi)(sgi);
    struct aymo_(ch2x_group)* cg = &chip->cg[cgi];
    sg->og_out_ch_gate_a = vand(sg->og_out_gate, cg->og_ch_gate_a);
    sg->og_out_ch_gate_b = vand(sg->og_out_gate, cg->og_ch_gate_b);
    sg->og_out_ch_gate_c = vand(sg->og_out_gate, cg->og_ch_gate_c);
    sg->og_out_ch_gate_d = vand(sg->og_out_gate, cg->og_ch_gate_d);
}


AYMO_STATIC
void aymo_(cm_rewire_ch2x)(struct aymo_(chip)* chip, int ch2x)
{
    if (chip->chip_regs.reg_105h.newm && (chip->og_ch2x_pairing & (1UL << ch2x))) {
        int ch2p = aymo_(ch2x_paired)[ch2x];
        int ch2x_is_secondary = (ch2p < ch2x);
        if (ch2x_is_secondary) {
            int t = ch2x;
            ch2x = ch2p;
            ch2p = t;
        }
        unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
        unsigned ch2p_cnt = chip->ch2x_regs[ch2p].reg_C0h.cnt;
        unsigned ch4x_cnt = ((ch2x_cnt << 1) | ch2p_cnt);
        const struct aymo_(conn)* ch4x_conn = aymo_(conn_ch4x_table)[ch4x_cnt];
        aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][0], &ch4x_conn[0]);
        aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][1], &ch4x_conn[1]);
        aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2p][0], &ch4x_conn[2]);
        aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2p][1], &ch4x_conn[3]);
    }
    else {
        unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
        const struct aymo_(conn)* ch2x_conn = aymo_(conn_ch2x_table)[ch2x_cnt];
        aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][0], &ch2x_conn[0]);
        aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][1], &ch2x_conn[1]);
    }
}


AYMO_STATIC
void aymo_(cm_rewire_conn)(struct aymo_(chip)* chip, const struct aymo_(reg_104h)* reg_104h_prev)
{
    struct aymo_(reg_104h)* reg_104h = &chip->chip_regs.reg_104h;
    unsigned diff = (reg_104h_prev ? (reg_104h_prev->conn ^ reg_104h->conn) : 0xFF);

    for (int ch4x = 0; ch4x < (AYMO_(CHANNEL_NUM_MAX) / 2); ++ch4x) {
        if (diff & (1 << ch4x)) {
            int ch2x = aymo_(ch4x_to_pair)[ch4x][0];
            int ch2p = aymo_(ch4x_to_pair)[ch4x][1];

            if (reg_104h->conn & (1 << ch4x)) {
                chip->og_ch2x_pairing |= ((1UL << ch2x) | (1UL << ch2p));

                unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
                unsigned ch2p_cnt = chip->ch2x_regs[ch2p].reg_C0h.cnt;
                unsigned ch4x_cnt = ((ch2x_cnt << 1) | ch2p_cnt);
                const struct aymo_(conn)* ch4x_conn = aymo_(conn_ch4x_table)[ch4x_cnt];
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][0], &ch4x_conn[0]);
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][1], &ch4x_conn[1]);
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2p][0], &ch4x_conn[2]);
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2p][1], &ch4x_conn[3]);
            }
            else {
                chip->og_ch2x_pairing &= ~((1UL << ch2x) | (1UL << ch2p));

                unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
                const struct aymo_(conn)* ch2x_conn = aymo_(conn_ch2x_table)[ch2x_cnt];
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][0], &ch2x_conn[0]);
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2x][1], &ch2x_conn[1]);

                unsigned ch2p_cnt = chip->ch2x_regs[ch2p].reg_C0h.cnt;
                const struct aymo_(conn)* ch2p_conn = aymo_(conn_ch2x_table)[ch2p_cnt];
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2p][0], &ch2p_conn[0]);
                aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[ch2p][1], &ch2p_conn[1]);
            }
        }
    }
}


AYMO_STATIC
void aymo_(cm_rewire_rhythm)(struct aymo_(chip)* chip, const struct aymo_(reg_BDh)* reg_BDh_prev)
{
    const struct aymo_(reg_BDh) reg_BDh_zero = { 0, 0, 0, 0, 0, 0, 0, 0 };
    const struct aymo_(reg_BDh)* reg_BDh = &chip->chip_regs.reg_BDh;
    int force_update = 0;

    if (reg_BDh->ryt) {
        if (!reg_BDh_prev->ryt) {
            // Apply special connection for rhythm mode
            unsigned ch6_cnt = chip->ch2x_regs[6].reg_C0h.cnt;
            const struct aymo_(conn)* ch6_conn = aymo_(conn_ryt_table)[ch6_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[6][0], &ch6_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[6][1], &ch6_conn[1]);

            const struct aymo_(conn)* ch7_conn = aymo_(conn_ryt_table)[2];
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[7][0], &ch7_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[7][1], &ch7_conn[1]);

            const struct aymo_(conn)* ch8_conn = aymo_(conn_ryt_table)[3];
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[8][0], &ch8_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[8][1], &ch8_conn[1]);

            force_update = 1;
        }
    }
    else {
        if (reg_BDh_prev->ryt) {
            // Apply standard Channel_2xOP connection
            unsigned ch6_cnt = chip->ch2x_regs[6].reg_C0h.cnt;
            const struct aymo_(conn)* ch6_conn = aymo_(conn_ch2x_table)[ch6_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[6][0], &ch6_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[6][1], &ch6_conn[1]);

            unsigned ch7_cnt = chip->ch2x_regs[7].reg_C0h.cnt;
            const struct aymo_(conn)* ch7_conn = aymo_(conn_ch2x_table)[ch7_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[7][0], &ch7_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[7][1], &ch7_conn[1]);

            unsigned ch8_cnt = chip->ch2x_regs[8].reg_C0h.cnt;
            const struct aymo_(conn)* ch8_conn = aymo_(conn_ch2x_table)[ch8_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[8][0], &ch8_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_(ch2x_to_word)[8][1], &ch8_conn[1]);

            reg_BDh = &reg_BDh_zero;  // force all keys off
            force_update = 1;
        }
    }

    if ((reg_BDh->hh != reg_BDh_prev->hh) || force_update) {
        int word_hh = aymo_(ch2x_to_word)[7][0];
        if (reg_BDh->hh) {
            aymo_(eg_key_on)(chip, word_hh, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_hh, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->tc != reg_BDh_prev->tc) || force_update) {
        int word_tc = aymo_(ch2x_to_word)[8][1];
        if (reg_BDh->tc) {
            aymo_(eg_key_on)(chip, word_tc, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_tc, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->tom != reg_BDh_prev->tom) || force_update) {
        int word_tom = aymo_(ch2x_to_word)[8][0];
        if (reg_BDh->tom) {
            aymo_(eg_key_on)(chip, word_tom, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_tom, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->sd != reg_BDh_prev->sd) || force_update) {
        int word_sd = aymo_(ch2x_to_word)[7][1];
        if (reg_BDh->sd) {
            aymo_(eg_key_on)(chip, word_sd, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_sd, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->bd != reg_BDh_prev->bd) || force_update) {
        int word_bd0 = aymo_(ch2x_to_word)[6][0];
        int word_bd1 = aymo_(ch2x_to_word)[6][1];
        if (reg_BDh->bd) {
            aymo_(eg_key_on)(chip, word_bd0, AYMO_(EG_KEY_DRUM));
            aymo_(eg_key_on)(chip, word_bd1, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_bd0, AYMO_(EG_KEY_DRUM));
            aymo_(eg_key_off)(chip, word_bd1, AYMO_(EG_KEY_DRUM));
        }
    }
}


AYMO_STATIC
void aymo_(write_00h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    switch (address) {
    case 0x01: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_01h) = value;
        break;
    }
    case 0x02: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_02h) = value;
        break;
    }
    case 0x03: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_03h) = value;
        break;
    }
    case 0x04: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_04h) = value;
        break;
    }
    case 0x104: {
        struct aymo_(reg_104h) reg_104h_prev = chip->chip_regs.reg_104h;
        *(uint8_t*)(void*)&(chip->chip_regs.reg_104h) = value;
        aymo_(cm_rewire_conn)(chip, &reg_104h_prev);
        break;
    }
    case 0x105: {
        struct aymo_(reg_105h) reg_105h_prev = chip->chip_regs.reg_105h;
        *(uint8_t*)(void*)&(chip->chip_regs.reg_105h) = value;
        if (chip->chip_regs.reg_105h.newm != reg_105h_prev.newm) {
            ;
        }
        break;
    }
    case 0x08: {
        struct aymo_(reg_08h) reg_08h_prev = chip->chip_regs.reg_08h;
        *(uint8_t*)(void*)&(chip->chip_regs.reg_08h) = value;
        if (chip->chip_regs.reg_08h.nts != reg_08h_prev.nts) {
            aymo_(chip_pg_update_nts)(chip);
        }
        break;
    }
    }
}


AYMO_STATIC
void aymo_(write_20h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int sgi = (aymo_(slot_to_word)[slot] / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (aymo_(slot_to_word)[slot] % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi);
    struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_(reg_20h)* reg_20h = &(chip->slot_regs[slot].reg_20h);
    struct aymo_(reg_20h) reg_20h_prev = *reg_20h;
    *(uint8_t*)(void*)reg_20h = value;
    unsigned update_deltafreq = 0;

    if (reg_20h->mult != reg_20h_prev.mult) {
        int16_t pg_mult_x2 = aymo_(pg_mult_x2_table)[reg_20h->mult];
        sg->pg_mult_x2 = vinsertn(sg->pg_mult_x2, pg_mult_x2, sgo);
        update_deltafreq = 1;  // force
    }

    if (reg_20h->ksr != reg_20h_prev.ksr) {
        int16_t eg_ksv = vextractn(cg->eg_ksv, sgo);
        int16_t eg_ks = (eg_ksv >> ((reg_20h->ksr ^ 1) << 1));
        sg->eg_ks = vinsertn(sg->eg_ks, eg_ks, sgo);
    }

    if (reg_20h->egt != reg_20h_prev.egt) {
        int16_t eg_adsr_word = vextractn(sg->eg_adsr, sgo);
        struct aymo_(eg_adsr)* eg_adsr = (struct aymo_(eg_adsr)*)(void*)&eg_adsr_word;
        eg_adsr->sr = (reg_20h->egt ? 0 : chip->slot_regs[slot].reg_80h.rr);
        sg->eg_adsr = vinsertn(sg->eg_adsr, eg_adsr_word, sgo);
    }

    if (reg_20h->vib != reg_20h_prev.vib) {
        int16_t pg_vib = (reg_20h->vib ? -1 : 0);
        sg->pg_vib = vinsertn(sg->pg_vib, pg_vib, sgo);
        update_deltafreq = 1;  // force
    }

    if (reg_20h->am != reg_20h_prev.am) {
        int16_t eg_am = (reg_20h->am ? -1 : 0);
        sg->eg_am = vinsertn(sg->eg_am, eg_am, sgo);

        uint16_t eg_tremolopos = chip->eg_tremolopos;
        if (eg_tremolopos >= 105) {
            eg_tremolopos = (210 - eg_tremolopos);
        }
        aymoi16_t eg_tremolo = vset1((int16_t)(eg_tremolopos >> chip->eg_tremoloshift));
        sg->eg_tremolo_am = vand(eg_tremolo, sg->eg_am);
    }

    if (update_deltafreq) {
        for (int sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
            int cgi = aymo_(sgi_to_cgi)(sgi);
            struct aymo_(ch2x_group)* cg = &chip->cg[cgi];
            struct aymo_(slot_group)* sg = &chip->sg[sgi];
            aymo_(pg_update_deltafreq)(chip, cg, sg);
        }
    }
}


AYMO_STATIC
void aymo_(write_40h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_(slot_to_word)[slot];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_(reg_40h)* reg_40h = &(chip->slot_regs[slot].reg_40h);
    struct aymo_(reg_40h) reg_40h_prev = *reg_40h;
    *(uint8_t*)(void*)reg_40h = value;

    if (reg_40h->tl != reg_40h_prev.tl) {
        int16_t eg_tl_x4 = ((int16_t)reg_40h->tl << 2);
        sg->eg_tl_x4 = vinsertn(sg->eg_tl_x4, eg_tl_x4, sgo);
    }

    if (reg_40h->ksl != reg_40h_prev.ksl) {
        aymo_(eg_update_ksl)(chip, word);
    }
}


AYMO_STATIC
void aymo_(write_60h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_(slot_to_word)[slot];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_(reg_60h)* reg_60h = &(chip->slot_regs[slot].reg_60h);
    struct aymo_(reg_60h) reg_60h_prev = *reg_60h;
    *(uint8_t*)(void*)reg_60h = value;

    if ((reg_60h->dr != reg_60h_prev.dr) || (reg_60h->ar != reg_60h_prev.ar)) {
        int16_t eg_adsr_word = vextractn(sg->eg_adsr, sgo);
        struct aymo_(eg_adsr)* eg_adsr = (struct aymo_(eg_adsr)*)(void*)&eg_adsr_word;
        eg_adsr->dr = reg_60h->dr;
        eg_adsr->ar = reg_60h->ar;
        sg->eg_adsr = vinsertn(sg->eg_adsr, eg_adsr_word, sgo);
    }
}


AYMO_STATIC
void aymo_(write_80h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_(slot_to_word)[slot];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_(reg_80h)* reg_80h = &(chip->slot_regs[slot].reg_80h);
    struct aymo_(reg_80h) reg_80h_prev = *reg_80h;
    *(uint8_t*)(void*)reg_80h = value;

    if ((reg_80h->rr != reg_80h_prev.rr) || (reg_80h->sl != reg_80h_prev.sl)) {
        int16_t eg_adsr_word = vextractn(sg->eg_adsr, sgo);
        struct aymo_(eg_adsr)* eg_adsr = (struct aymo_(eg_adsr)*)(void*)&eg_adsr_word;
        eg_adsr->sr = (chip->slot_regs[slot].reg_20h.egt ? 0 : reg_80h->rr);
        eg_adsr->rr = reg_80h->rr;
        sg->eg_adsr = vinsertn(sg->eg_adsr, eg_adsr_word, sgo);
        int16_t eg_sl = (int16_t)reg_80h->sl;
        if (eg_sl == 0x0F) {
            eg_sl = 0x1F;
        }
        sg->eg_sl = vinsertn(sg->eg_sl, eg_sl, sgo);
    }
}


AYMO_STATIC
void aymo_(write_E0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_(slot_to_word)[slot];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_(reg_E0h)* reg_E0h = &(chip->slot_regs[slot].reg_E0h);
    struct aymo_(reg_E0h) reg_E0h_prev = *reg_E0h;
    *(uint8_t*)(void*)reg_E0h = value;

    if (!chip->chip_regs.reg_105h.newm) {
        reg_E0h->ws &= 3;
    }

    if (reg_E0h->ws != reg_E0h_prev.ws) {
        const struct aymo_(wave)* wave = &aymo_(wave_table)[reg_E0h->ws];
        sg->wg_phase_shl   = vinsertn(sg->wg_phase_shl,   wave->wg_phase_shl,   sgo);
        sg->wg_phase_zero  = vinsertn(sg->wg_phase_zero,  wave->wg_phase_zero,  sgo);
        sg->wg_phase_neg   = vinsertn(sg->wg_phase_neg,   wave->wg_phase_neg,   sgo);
        sg->wg_phase_flip  = vinsertn(sg->wg_phase_flip,  wave->wg_phase_flip,  sgo);
        sg->wg_phase_mask  = vinsertn(sg->wg_phase_mask,  wave->wg_phase_mask,  sgo);
        sg->wg_sine_gate   = vinsertn(sg->wg_sine_gate,   wave->wg_sine_gate,   sgo);
    }
}


AYMO_STATIC
void aymo_(write_A0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
    int ch2p = aymo_(ch2x_paired)[ch2x];
    int ch2x_is_secondary = (ch2p < ch2x);
    if (chip->chip_regs.reg_105h.newm && ch2x_is_pairing && ch2x_is_secondary) {
        return;
    }
    if (!ch2x_is_pairing || ch2x_is_secondary) {
        ch2p = -1;
    }

    struct aymo_(reg_A0h)* reg_A0h = &(chip->ch2x_regs[ch2x].reg_A0h);
    struct aymo_(reg_A0h) reg_A0h_prev = *reg_A0h;
    *(uint8_t*)(void*)reg_A0h = value;

    if (reg_A0h->fnum_lo != reg_A0h_prev.fnum_lo) {
        aymo_(ch2x_update_fnum)(chip, ch2x, ch2p);
    }
}


AYMO_STATIC
void aymo_(write_B0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
    int ch2p = aymo_(ch2x_paired)[ch2x];
    int ch2x_is_secondary = (ch2p < ch2x);
    if (chip->chip_regs.reg_105h.newm && ch2x_is_pairing && ch2x_is_secondary) {
        return;
    }
    if (!ch2x_is_pairing || ch2x_is_secondary) {
        ch2p = -1;
    }

    if (address == 0xBD) {
        struct aymo_(reg_BDh)* reg_BDh = &chip->chip_regs.reg_BDh;
        struct aymo_(reg_BDh) reg_BDh_prev = *reg_BDh;
        *(uint8_t*)(void*)reg_BDh = value;

        chip->eg_tremoloshift = (((reg_BDh->dam ^ 1) << 1) + 2);
        chip->eg_vibshift = (reg_BDh->dvb ^ 1);
        aymo_(cm_rewire_rhythm)(chip, &reg_BDh_prev);
    }
    else {
        struct aymo_(reg_B0h)* reg_B0h = &(chip->ch2x_regs[ch2x].reg_B0h);
        struct aymo_(reg_B0h) reg_B0h_prev = *reg_B0h;
        *(uint8_t*)(void*)reg_B0h = value;

        if ((reg_B0h->fnum_hi != reg_B0h_prev.fnum_hi) || (reg_B0h->block != reg_B0h_prev.block)) {
            aymo_(ch2x_update_fnum)(chip, ch2x, ch2p);
        }

        if (reg_B0h->kon != reg_B0h_prev.kon) {
            if (reg_B0h->kon) {
                aymo_(ch2x_key_on)(chip, ch2x);
            } else {
                aymo_(ch2x_key_off)(chip, ch2x);
            }
        }
    }
}


AYMO_STATIC
void aymo_(write_C0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    struct aymo_(reg_C0h)* reg_C0h = &(chip->ch2x_regs[ch2x].reg_C0h);
    struct aymo_(reg_C0h) reg_C0h_prev = *reg_C0h;
    if (!chip->chip_regs.reg_105h.newm) {
        value = ((value | 0x30) & 0x3F);
    }
    *(uint8_t*)(void*)reg_C0h = value;

    int ch2x_word0 = aymo_(ch2x_to_word)[ch2x][0];
    int ch2x_word1 = aymo_(ch2x_to_word)[ch2x][1];
    int sgo = (ch2x_word0 % AYMO_(SLOT_GROUP_LENGTH));
    int sgi0 = (ch2x_word0 / AYMO_(SLOT_GROUP_LENGTH));
    int sgi1 = (ch2x_word1 / AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg0 = &chip->sg[sgi0];
    struct aymo_(slot_group)* sg1 = &chip->sg[sgi1];
    int cgi = (sgi0 / 2);
    struct aymo_(ch2x_group)* cg = &chip->cg[cgi];

    if (reg_C0h->cha != reg_C0h_prev.cha) {
        cg->og_ch_gate_a = vinsertn(cg->og_ch_gate_a, (reg_C0h->cha ? -1 : 0), sgo);
        sg0->og_out_ch_gate_a = vand(sg0->og_out_gate, cg->og_ch_gate_a);
        sg1->og_out_ch_gate_a = vand(sg1->og_out_gate, cg->og_ch_gate_a);
    }
    if (reg_C0h->chb != reg_C0h_prev.chb) {
        cg->og_ch_gate_b = vinsertn(cg->og_ch_gate_b, (reg_C0h->chb ? -1 : 0), sgo);
        sg0->og_out_ch_gate_b = vand(sg0->og_out_gate, cg->og_ch_gate_b);
        sg1->og_out_ch_gate_b = vand(sg1->og_out_gate, cg->og_ch_gate_b);
    }
    if (reg_C0h->chc != reg_C0h_prev.chc) {
        cg->og_ch_gate_c = vinsertn(cg->og_ch_gate_c, (reg_C0h->chc ? -1 : 0), sgo);
        sg0->og_out_ch_gate_c = vand(sg0->og_out_gate, cg->og_ch_gate_c);
        sg1->og_out_ch_gate_c = vand(sg1->og_out_gate, cg->og_ch_gate_c);
    }
    if (reg_C0h->chd != reg_C0h_prev.chd) {
        cg->og_ch_gate_d = vinsertn(cg->og_ch_gate_d, (reg_C0h->chd ? -1 : 0), sgo);
        sg0->og_out_ch_gate_d = vand(sg0->og_out_gate, cg->og_ch_gate_d);
        sg1->og_out_ch_gate_d = vand(sg1->og_out_gate, cg->og_ch_gate_d);
    }

    if (reg_C0h->fb != reg_C0h_prev.fb) {
        int16_t fb_shr = (reg_C0h->fb ? (9 - reg_C0h->fb) : 16);
        sg0->wg_fb_shr = vinsertn(sg0->wg_fb_shr, fb_shr, sgo);
        sg1->wg_fb_shr = vinsertn(sg1->wg_fb_shr, fb_shr, sgo);
    }

    if (chip->chip_regs.reg_105h.stereo) {
        // TODO
    }

    if (reg_C0h->cnt != reg_C0h_prev.cnt) {
        aymo_(cm_rewire_ch2x)(chip, ch2x);
    }
}


AYMO_STATIC
void aymo_(write_D0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    *(uint8_t*)(void*)&(chip->ch2x_regs[ch2x].reg_C0h) = value;

    if (chip->chip_regs.reg_105h.stereo) {
        // TODO
    }
}


void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    if (address > 0x1FF) {
        return;
    }

    switch (address & 0xF0) {
    case 0x00: {
        aymo_(write_00h)(chip, address, value);
        break;
    }
    case 0x20:
    case 0x30: {
        aymo_(write_20h)(chip, address, value);
        break;
    }
    case 0x40:
    case 0x50: {
        aymo_(write_40h)(chip, address, value);
        break;
    }
    case 0x60:
    case 0x70: {
        aymo_(write_60h)(chip, address, value);
        break;
    }
    case 0x80:
    case 0x90: {
        aymo_(write_80h)(chip, address, value);
        break;
    }
    case 0xE0:
    case 0xF0: {
        aymo_(write_E0h)(chip, address, value);
        break;
    }
    case 0xA0: {
        aymo_(write_A0h)(chip, address, value);
        break;
    }
    case 0xB0: {
        aymo_(write_B0h)(chip, address, value);
        break;
    }
    case 0xC0: {
        aymo_(write_C0h)(chip, address, value);
        break;
    }
    case 0xD0: {
        aymo_(write_D0h)(chip, address, value);
        break;
    }
    }
}


AYMO_STATIC
int aymo_(rq_enqueue)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    uint16_t rq_tail = chip->rq_tail;
    uint16_t rq_next = (rq_tail + 1);
    if (rq_next >= AYMO_(REG_QUEUE_LENGTH)) {
        rq_next = 0;
    }

    if (rq_next != chip->rq_head) {
        chip->rq_buffer[rq_tail].address = address;
        chip->rq_buffer[rq_tail].value = value;
        chip->rq_tail = rq_next;
        return 1;
    }
    return 0;
}


int aymo_(enqueue_write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    if (address < 0x8000U) {
        return aymo_(rq_enqueue)(chip, address, value);
    }
    return 0;
}


int aymo_(enqueue_delay)(struct aymo_(chip)* chip, uint32_t ticks)
{
    if (ticks < 0x8000U) {
        uint16_t address = (uint16_t)((ticks >> 8) | 0x8000U);
        uint8_t value = (uint8_t)(ticks & 0xFFU);
        return aymo_(rq_enqueue)(chip, address, value);
    }
    return 0;
}


// Cheap alternative to memset()
// No care for performance; made just to avoid a library call
AYMO_INLINE
void aymo_(memset)(void* data, uint8_t value, size_t size)
{
    volatile uint8_t* ptr = (uint8_t*)data;
    const uint8_t* end = (uint8_t*)data + size;
    while (ptr != end) {
        *ptr++ = value;
    }
}


// Returns the size of a chip instance
size_t aymo_(size)(void)
{
    return sizeof(struct aymo_(chip));
}


// Initializes chip status
void aymo_(init)(struct aymo_(chip)* chip)
{
    // Wipe everything
    aymo_(memset)(chip, 0, sizeof(*chip));

    // Initialize slots
    for (int sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
        struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
        sg->eg_rout = vset1(0x01FF);
        sg->eg_out = vset1(0x01FF);
        sg->eg_gen = vset1(AYMO_(EG_GEN_RELEASE));
        sg->eg_gen_shl = vset1(AYMO_(EG_GEN_SHL_RELEASE));
        sg->pg_notreset = vset1(-1);
        sg->pg_mult_x2 = vset1(aymo_(pg_mult_x2_table)[0]);
        sg->og_prout_ac = vsetm(aymo_(og_prout_ac)[sgi]);
        sg->og_prout_bd = vsetm(aymo_(og_prout_bd)[sgi]);

        for (int sgo = 0; sgo < AYMO_(SLOT_GROUP_LENGTH); ++sgo) {
            const struct aymo_(wave)* wave = &aymo_(wave_table)[0];
            sg->wg_phase_shl   = vinsertn(sg->wg_phase_shl,   wave->wg_phase_shl,   sgo);
            sg->wg_phase_zero  = vinsertn(sg->wg_phase_zero,  wave->wg_phase_zero,  sgo);
            sg->wg_phase_neg   = vinsertn(sg->wg_phase_neg,   wave->wg_phase_neg,   sgo);
            sg->wg_phase_flip  = vinsertn(sg->wg_phase_flip,  wave->wg_phase_flip,  sgo);
            sg->wg_phase_mask  = vinsertn(sg->wg_phase_mask,  wave->wg_phase_mask,  sgo);
            sg->wg_sine_gate   = vinsertn(sg->wg_sine_gate,   wave->wg_sine_gate,   sgo);
        }
    }

    // Initialize channels
    for (int cgi = 0; cgi < (AYMO_(SLOT_GROUP_NUM) / 2); ++cgi) {
        struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
        cg->og_ch_gate_a = vset1(-1);
        cg->og_ch_gate_b = vset1(-1);
    }
    for (int ch2x = 0; ch2x < AYMO_(CHANNEL_NUM_MAX); ++ch2x) {
        aymo_(cm_rewire_ch2x)(chip, ch2x);
    }

    // Initialize chip
    chip->eg_statev = vset1(1);

    chip->eg_timer = AYMO_(EG_TIMER_HIBIT);

    chip->ng_noise = 1;

    chip->eg_state = 1;
    chip->eg_tremoloshift = 4;
    chip->eg_vibshift = 1;
}


#endif  // AYMO_ARCH_IS_ARMV7_NEON
