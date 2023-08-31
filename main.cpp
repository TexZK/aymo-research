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
*/

#define CONFIG_AYMO_YMF262_X86_AVX2_DEFINE_MACROS
#define CONFIG_AYMO_YMF262_X86_AVX2_DEFINE_TYPES
#define CONFIG_AYMO_KEEP_SHORTHANDS
#include "aymo_ymf262_x86_avx2.h"

#include "imf.h"
#include "opl3.h"
#include "regdump.h"

//#include <cassert>//XXX
#define assert(c) {if(!(c))__debugbreak();}//XXX
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>


static struct aymo_(chip) aymo_chip;
static opl3_chip nuked_chip;


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

static const uint8_t mt[16] = {
    1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30
};


void compare_slots(const struct aymo_(chip)* aymo_chip, const opl3_chip* nuked_chip, int8_t slot_)
{
    if (slot_ >= 36) {
        return;
    }

    int8_t word = aymo_(slot_to_word)[slot_];
    int8_t sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int8_t sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    int8_t cgi = (sgi / 2);
    const struct aymo_(slot_group)* sg = &aymo_chip->sg[sgi];
    const struct aymo_(ch2x_group)* cg = &aymo_chip->cg[cgi];
    const opl3_slot* slot = &nuked_chip->slot[slot_];
    (void)cg;

    // TODO: Commented stuff
    assert(vextractn(sg->wg_out, sgo) == slot->out);
    //assert(vextractn(sg->wg_fbmod, sgo) == slot->fbmod);
    //assert(vextractn(sg->wg_mod, sgo) == *slot->mod);
    assert(vextractn(sg->wg_prout, sgo) == slot->prout);
    assert((uint16_t)vextractn(sg->eg_rout, sgo) == slot->eg_rout);
    assert((uint16_t)vextractn(sg->eg_out, sgo) == slot->eg_out);
    //assert((uint16_t)vextractn(sg->eg_inc, sgo) == slot->eg_inc);
    assert((uint16_t)vextractn(sg->eg_gen, sgo) == slot->eg_gen);
    //assert(vextractn(sg->eg_rate, sgo) == slot->eg_rate);
    //assert(vextractn(sg->eg_ksl, sgo) == slot->eg_ksl);
    //assert(vextractn(sg->eg_trem, sgo) == *slot->trem);
    assert((uint16_t)-vextractn(sg->pg_vib, sgo) == slot->reg_vib);
    //assert(vextractn(sg->eg_egt, sgo) == slot->reg_type);
    //assert(vextractn(sg->eg_ksr, sgo) == slot->reg_ksr);
    assert((uint16_t)vextractn(sg->pg_mult_x2, sgo) == mt[slot->reg_mult]);
    assert((uint16_t)vextractn(sg->eg_tl_x4, sgo) == slot->reg_tl * 4U);
    assert((((uint16_t)vextractn(sg->eg_adsr, sgo) >> 12) & 15) == slot->reg_ar);
    assert((((uint16_t)vextractn(sg->eg_adsr, sgo) >>  8) & 15) == slot->reg_dr);
    assert((uint16_t)vextractn(sg->eg_sl, sgo) == slot->reg_sl);
    assert((((uint16_t)vextractn(sg->eg_adsr, sgo) >>  0) & 15) == slot->reg_rr);
    //assert(vextractn(sg->wg_wf, sgo) == slot->reg_wf);
    assert((uint16_t)vextractn(sg->eg_key, sgo) == slot->key);
    assert((uint16_t)-vextractn(sg->pg_notreset, sgo) == !slot->pg_reset);
    const int8_t sgo_side[16] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1 };
    const int8_t sgo_cell[16] = { 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7, 4, 5, 6, 7 };
    uint32_t pg_phase = (sgo_side[sgo] ? sg->pg_phase_hi.m256i_u32 : sg->pg_phase_lo.m256i_u32)[sgo_cell[sgo]];
    assert(pg_phase == slot->pg_phase);
    assert((uint16_t)vextractn(sg->pg_phase_out, sgo) == slot->pg_phase_out);
}


void compare_ch2xs(const struct aymo_(chip)* aymo_chip, const opl3_chip* nuked_chip, int8_t ch2x)
{
    if (ch2x >= 18) {
        return;
    }

    int8_t word = aymo_(ch2x_to_word)[ch2x][0];
    int8_t sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int8_t sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    int8_t cgi = (sgi / 2);
    const struct aymo_(ch2x_group)* cg = &aymo_chip->cg[cgi];
    const opl3_channel* channel = &nuked_chip->channel[ch2x];

    // TODO: Commented stuff
    //int16_t* out[4];
    //uint8_t chtype;
    assert((uint16_t)vextractn(cg->pg_fnum, sgo) == channel->f_num);
    assert((uint16_t)vextractn(cg->pg_block, sgo) == channel->block);
    //assert((uint16_t)vextractn(cg->fb, sgo) == channel->fb);
    //uint8_t con;
    //uint8_t alg;
    //uint8_t ksv;
    //uint16_t cha, chb;
    //uint16_t chc, chd;
    //uint8_t ch_num;
}


void compare_chips(const struct aymo_(chip)* aymo_chip, const opl3_chip* nuked_chip)
{
    // TODO: Commented stuff
    assert((uint16_t)aymo_chip->tm_timer == nuked_chip->timer);
    assert((aymo_chip->eg_timer & AYMO_(EG_TIMER_MASK)) == nuked_chip->eg_timer);
    //assert((aymo_chip->eg_timerrem & AYMO_(EG_TIMER_MASK)) == nuked_chip->eg_timerrem);
    assert(aymo_chip->eg_state == nuked_chip->eg_state);
    assert((uint16_t)vextractn(aymo_chip->eg_add, 0) == nuked_chip->eg_add);
    //uint8_t newm;
    //uint8_t nts;
    //uint8_t rhy;
    assert(aymo_chip->pg_vibpos == nuked_chip->vibpos);
    assert(aymo_chip->eg_vibshift == nuked_chip->vibshift);
    //assert((uint16_t)vextractn(aymo_chip->eg_tremolo, 0) == nuked_chip->tremolo);
    assert(aymo_chip->eg_tremolopos == nuked_chip->tremolopos);
    assert(aymo_chip->eg_tremoloshift == nuked_chip->tremoloshift);
    assert(aymo_chip->ng_noise == nuked_chip->noise);
    assert(aymo_chip->og_sum_a == nuked_chip->mixbuff[0]);
    assert(aymo_chip->og_sum_b == nuked_chip->mixbuff[1]);
    assert(aymo_chip->og_sum_c == nuked_chip->mixbuff[2]);
    assert(aymo_chip->og_sum_d == nuked_chip->mixbuff[3]);
    assert(aymo_chip->rm_hh_bit2 == nuked_chip->rm_hh_bit2);
    assert(aymo_chip->rm_hh_bit3 == nuked_chip->rm_hh_bit3);
    assert(aymo_chip->rm_hh_bit7 == nuked_chip->rm_hh_bit7);
    assert(aymo_chip->rm_hh_bit8 == nuked_chip->rm_hh_bit8);
    assert(aymo_chip->rm_tc_bit3 == nuked_chip->rm_tc_bit3);
    assert(aymo_chip->rm_tc_bit5 == nuked_chip->rm_tc_bit5);

    for (int8_t ch2x = 0; ch2x < 18; ++ch2x) {
        compare_ch2xs(aymo_chip, nuked_chip, ch2x);
    }

    for (int8_t slot = 0; slot < 36; ++slot) {
        compare_slots(aymo_chip, nuked_chip, slot);
    }
}


void silence_benchmark(void)
{
    int64_t time_ms_aymo = 0;
    {
        aymo_(init)(&aymo_chip);

        auto time_start = std::chrono::steady_clock::now();

        for (uint64_t i = 0; i < 10'000'000; ++i) {
            aymo_(tick)(&aymo_chip);
        }

        auto time_end = std::chrono::steady_clock::now();
        auto time_diff = (time_end - time_start);
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_diff).count();
        time_ms_aymo = time_ms;

        printf_s("aymo: %lld\n", time_ms);
    }

    int64_t time_ms_nuked = 0;
    {
        OPL3_Reset(&nuked_chip, 49716);

        int16_t nuked_out[4];
        auto time_start = std::chrono::steady_clock::now();

        for (uint64_t i = 0; i < 10'000'000; ++i) {
            OPL3_Generate4Ch(&nuked_chip, nuked_out);
        }

        auto time_end = std::chrono::steady_clock::now();
        auto time_diff = (time_end - time_start);
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_diff).count();
        time_ms_nuked = time_ms;

        printf_s("nuked: %lld\n", time_ms);
    }

    double time_ratio = ((double)time_ms_aymo / (double)time_ms_nuked);
    printf_s("nuked/aymo: %5.3f\n", 1 / time_ratio);
}


void imf_test_simple(void)
{
    static const uint8_t imf_buffer[] = {
        0x48, 0x00,

        0xBD, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00,
        0xB0, 0x00, 0x00, 0x00,

        0x20, 0xC1, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x00,
        0x60, 0x1F, 0x00, 0x00,
        0x80, 0x0F, 0x00, 0x00,
        0xE0, 0x00, 0x00, 0x00,

        0x23, 0x00, 0x00, 0x00,
        0x43, 0x3F, 0x00, 0x00,
        0x63, 0x00, 0x00, 0x00,
        0x83, 0x00, 0x00, 0x00,
        0xE3, 0x00, 0x00, 0x00,

        0xA0, 0x22, 0x00, 0x00,
        0xB0, 0x15, 0x00, 0x00,
        0xC0, 0x31, 0x00, 0x00,

        0xB0, 0x35, 0x00, 0x10,
        0xB0, 0x15, 0x04, 0x00,
    };

    struct imf_status imf_status;
    imf_init(&imf_status, (uint32_t)imf_rate_wolfenstein_3d, (uint32_t)AYMO_(SAMPLE_RATE));
    uint8_t imf_type = imf_guess_type(imf_buffer, sizeof(imf_buffer));
    imf_load(&imf_status, imf_buffer, sizeof(imf_buffer), imf_type);
    std::ofstream ofs("simple.raw", std::ios::binary);

    aymo_(init)(&aymo_chip);

    OPL3_Reset(&nuked_chip, 49716);
    int16_t nuked_out[4] = { 0, 0, 0, 0 };

    struct imf_cmd cmd = { 0, 0, 1 };
    while (cmd.delaying < 2) {
        compare_chips(&aymo_chip, &nuked_chip);
        cmd = imf_opl_tick(&imf_status);
        if (cmd.address) {
            printf_s("@ 0x%04X 0x%02X\n", cmd.address, cmd.value);
            OPL3_WriteRegBuffered(&nuked_chip, cmd.address, cmd.value);
            aymo_(enqueue_write)(&aymo_chip, cmd.address, cmd.value);
        }
        OPL3_Generate4Ch(&nuked_chip, nuked_out);
        aymo_(tick)(&aymo_chip);
        ofs.write(reinterpret_cast<const char*>(&aymo_chip.og_out_a), sizeof(int16_t));
        //ofs.write(reinterpret_cast<const char*>(&chip.og_out_b), sizeof(chip.og_out_b));
        ofs.write(reinterpret_cast<const char*>(&nuked_out[0]), sizeof(int16_t));
    }
}


void imf_test_file(void)
{
    std::string imf_buffer;
    {
        std::string path = "IMF\\23 - Award Ceremony.wlf";
        //std::string path = "adlib_38.imf.wlf";
        std::ifstream ifs(path, std::ios::binary);
        std::stringstream ss;
        ss << ifs.rdbuf();
        imf_buffer = ss.str();
    }
    static struct imf_status imf_status;
    imf_init(&imf_status, (uint32_t)imf_rate_wolfenstein_3d, (uint32_t)AYMO_(SAMPLE_RATE));
    uint8_t imf_type = imf_guess_type(imf_buffer.c_str(), imf_buffer.size());
    imf_load(&imf_status, imf_buffer.c_str(), imf_buffer.size(), imf_type);
    std::ofstream ofs("file.raw", std::ios::binary);

    aymo_(init)(&aymo_chip);

    OPL3_Reset(&nuked_chip, 49716);
    static int16_t nuked_out[4] = { 0, 0, 0, 0 };
    
    struct imf_cmd cmd = { 0, 0, 1 };
    while (cmd.delaying < 2) {
        compare_chips(&aymo_chip, &nuked_chip);
        cmd = imf_opl_tick(&imf_status);
        if (cmd.address) {
            //printf_s("@ 0x%04X 0x%02X\n", cmd.address, cmd.value);
            OPL3_WriteRegBuffered(&nuked_chip, cmd.address, cmd.value);
            aymo_(enqueue_write)(&aymo_chip, cmd.address, cmd.value);
        }
        OPL3_Generate4Ch(&nuked_chip, nuked_out);
        aymo_(tick)(&aymo_chip);
        ofs.write(reinterpret_cast<const char*>(&aymo_chip.og_out_a), sizeof(int16_t));
        //ofs.write(reinterpret_cast<const char*>(&chip.og_out_b), sizeof(chip.og_out_b));
        ofs.write(reinterpret_cast<const char*>(&nuked_out[0]), sizeof(int16_t));
    }
}


void regdump_test_file(void)
{
    std::string regdump_buffer;
    {
        std::string path = "regdumpopl.bin";
        std::ifstream ifs(path, std::ios::binary);
        std::stringstream ss;
        ss << ifs.rdbuf();
        regdump_buffer = ss.str();
    }
    static struct regdump_status regdump_status;
    regdump_init(&regdump_status);
    uint8_t imf_type = imf_guess_type(regdump_buffer.c_str(), regdump_buffer.size());
    regdump_load(&regdump_status, regdump_buffer.c_str(), regdump_buffer.size());
    std::ofstream ofs("file.raw", std::ios::binary);

    aymo_(init)(&aymo_chip);

    OPL3_Reset(&nuked_chip, 49716);
    static int16_t nuked_out[4] = { 0, 0, 0, 0 };

    struct regdump_cmd cmd = { 0, 0, 1 };
    while (cmd.delaying < 2) {
        //compare_chips(&aymo_chip, &nuked_chip);
        cmd = regdump_opl_tick(&regdump_status);
        if (cmd.address) {
            //printf_s("@ 0x%04X 0x%02X\n", cmd.address, cmd.value);
            //OPL3_WriteRegBuffered(&nuked_chip, cmd.address, cmd.value);  // FIXME: different queue delays
            //aymo_(enqueue_write)(&aymo_chip, cmd.address, cmd.value);
            OPL3_WriteReg(&nuked_chip, cmd.address, cmd.value);
            aymo_(write)(&aymo_chip, cmd.address, cmd.value);
        }
        OPL3_Generate4Ch(&nuked_chip, nuked_out);
        aymo_(tick)(&aymo_chip);
        ofs.write(reinterpret_cast<const char*>(&aymo_chip.og_out_a), sizeof(int16_t));
        //ofs.write(reinterpret_cast<const char*>(&aymo_chip.og_out_b), sizeof(int16_t));
        ofs.write(reinterpret_cast<const char*>(&nuked_out[0]), sizeof(int16_t));
    }
}


void file_benchmark(void)
{
    std::string regdump_buffer;
    {
        std::string path = "regdumpopl.bin";
        //std::string path = "IMF\\16 - Zero Hour.wlf";
        //std::string path = "adlib_38.imf.wlf";
        std::ifstream ifs(path, std::ios::binary);
        std::stringstream ss;
        ss << ifs.rdbuf();
        regdump_buffer = ss.str();
    }
    static struct regdump_status regdump_status;
    regdump_init(&regdump_status);
    uint8_t imf_type = imf_guess_type(regdump_buffer.c_str(), regdump_buffer.size());
    regdump_load(&regdump_status, regdump_buffer.c_str(), regdump_buffer.size());
    std::ofstream ofs("file.raw", std::ios::binary);

    int64_t time_ms_aymo = 0;
    {
        aymo_(init)(&aymo_chip);
        regdump_restart(&regdump_status);

        auto time_start = std::chrono::steady_clock::now();

        struct regdump_cmd cmd = { 0, 0, 1 };
        while (cmd.delaying < 2) {
            cmd = regdump_opl_tick(&regdump_status);
            if (cmd.address) {
                aymo_(write)(&aymo_chip, cmd.address, cmd.value);
            }
            aymo_(tick)(&aymo_chip);
        }

        auto time_end = std::chrono::steady_clock::now();
        auto time_diff = (time_end - time_start);
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_diff).count();
        time_ms_aymo = time_ms;

        printf_s("aymo: %lld\n", time_ms);
    }

    int64_t time_ms_nuked = 0;
    {
        OPL3_Reset(&nuked_chip, 49716);
        regdump_restart(&regdump_status);

        int16_t nuked_out[4];
        auto time_start = std::chrono::steady_clock::now();

        struct regdump_cmd cmd = { 0, 0, 1 };
        while (cmd.delaying < 2) {
            cmd = regdump_opl_tick(&regdump_status);
            if (cmd.address) {
                OPL3_WriteReg(&nuked_chip, cmd.address, cmd.value);
            }
            OPL3_Generate4Ch(&nuked_chip, nuked_out);
        }

        auto time_end = std::chrono::steady_clock::now();
        auto time_diff = (time_end - time_start);
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_diff).count();
        time_ms_nuked = time_ms;

        printf_s("nuked: %lld\n", time_ms);
    }

    double time_ratio = ((double)time_ms_aymo / (double)time_ms_nuked);
    printf_s("nuked/aymo: %5.3f\n", 1 / time_ratio);
}


void test_vhsum(void)
{
    aymo16_t a = vsetr(1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, -32768);
    volatile int b = vhsum(a);
    assert(b == -1);
}


int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    //test_vhsum();

    //imf_test_simple();
    //imf_test_file();
    //regdump_test_file();

    //silence_benchmark();
    file_benchmark();

    return EXIT_SUCCESS;
}
