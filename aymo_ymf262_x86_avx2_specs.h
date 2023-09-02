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

#ifndef include_aymo_ymf262_x86_avx2_specs_h_
#define include_aymo_ymf262_x86_avx2_specs_h_

#include "aymo_arch_x86_avx2_specs.h"

#include <stdint.h>


#ifdef __cplusplus
    extern "C" {
#endif  // __cplusplus


#undef AYMO_
#undef aymo_
#define AYMO_(TOKEN)    AYMO_YMF262_X86_AVX2_##TOKEN
#define aymo_(token)    aymo_ymf262_x86_avx2_##token


#define AYMO_YMF262_X86_AVX2_SLOT_NUM_MAX           64
#define AYMO_YMF262_X86_AVX2_SLOT_NUM               36
#define AYMO_YMF262_X86_AVX2_CHANNEL_NUM_MAX        32
#define AYMO_YMF262_X86_AVX2_CHANNEL_NUM            18
#define AYMO_YMF262_X86_AVX2_SLOT_GROUP_NUM         4
#define AYMO_YMF262_X86_AVX2_SLOT_GROUP_LENGTH      16
#define AYMO_YMF262_X86_AVX2_CONN_NUM_MAX           6
#define AYMO_YMF262_X86_AVX2_SAMPLE_RATE            49716


#ifdef __GNUC__
    #pragma scalar_storage_order little-endian
#endif

// Wave descriptor for single slot
struct aymo_(wave) {
    int16_t wg_phase_mullo;
    int16_t wg_phase_zero;
    int16_t wg_phase_neg;
    int16_t wg_phase_flip;
    int16_t wg_phase_mask;
    int16_t wg_sine_gate;
};

// Waveform enumerator
enum aymo_(wf) {
    aymo_(wf_sin) = 0,
    aymo_(wf_sinup),
    aymo_(wf_sinabs),
    aymo_(wf_sinabsqrt),
    aymo_(wf_sinfast),
    aymo_(wf_sinabsfast),
    aymo_(wf_square),
    aymo_(wf_log)
};


// Connection descriptor for a single slot
struct aymo_(conn) {
    int16_t wg_fbmod_gate;
    int16_t wg_prmod_gate;
    int16_t og_out_gate;
};


// Registers; little-endian bitfields
AYMO_PRAGMA_PACK_PUSH_1

struct aymo_(reg_01h) {
    uint8_t lsitest_lo : 8;
};
struct aymo_(reg_101h) {
    uint8_t lsitest_hi : 6;
    uint8_t _7_6 : 2;
};
struct aymo_(reg_02h) {
    uint8_t timer1 : 8;
};
struct aymo_(reg_03h) {
    uint8_t timer2 : 8;
};
struct aymo_(reg_04h) {
    uint8_t st1 : 1;
    uint8_t st2 : 1;
    uint8_t _4_2 : 3;
    uint8_t mt2 : 1;
    uint8_t mt1 : 1;
    uint8_t rst : 1;
};
struct aymo_(reg_104h) {
    uint8_t conn : 6;
    uint8_t _7_6 : 2;
};
struct aymo_(reg_105h) {
    uint8_t newm : 1;
    uint8_t stereo : 1;
    uint8_t _7_2 : 6;
};
struct aymo_(reg_08h) {
    uint8_t _5_0 : 6;
    uint8_t nts : 1;
    uint8_t csm : 1;
};
struct aymo_(reg_20h) {
    uint8_t mult : 4;
    uint8_t ksr : 1;
    uint8_t egt : 1;
    uint8_t vib : 1;
    uint8_t am : 1;
};
struct aymo_(reg_40h) {
    uint8_t tl : 6;
    uint8_t ksl : 2;
};
struct aymo_(reg_60h) {
    uint8_t dr : 4;
    uint8_t ar : 4;
};
struct aymo_(reg_80h) {
    uint8_t rr : 4;
    uint8_t sl : 4;
};
struct aymo_(reg_A0h) {
    uint8_t fnum_lo : 8;
};
struct aymo_(reg_B0h) {
    uint8_t fnum_hi : 2;
    uint8_t block : 3;
    uint8_t kon : 1;
    uint8_t _7_6 : 2;
};
struct aymo_(reg_BDh) {
    uint8_t hh : 1;
    uint8_t tc : 1;
    uint8_t tom : 1;
    uint8_t sd : 1;
    uint8_t bd : 1;
    uint8_t ryt : 1;
    uint8_t dvb : 1;
    uint8_t dam : 1;
};
struct aymo_(reg_C0h) {
    uint8_t cnt : 1;
    uint8_t fb : 3;
    uint8_t cha : 1;
    uint8_t chb : 1;
    uint8_t chc : 1;
    uint8_t chd : 1;
};
struct aymo_(reg_E0h) {
    uint8_t ws : 3;
    uint8_t _7_3 : 5;
};

struct aymo_(chip_regs) {
    struct aymo_(reg_01h) reg_01h;
    struct aymo_(reg_02h) reg_02h;
    struct aymo_(reg_03h) reg_03h;
    struct aymo_(reg_04h) reg_04h;
    struct aymo_(reg_08h) reg_08h;
    struct aymo_(reg_BDh) reg_BDh;
    struct aymo_(reg_101h) reg_101h;
    struct aymo_(reg_104h) reg_104h;
    struct aymo_(reg_105h) reg_105h;
    uint8_t pad32_[3];
};

struct aymo_(slot_regs) {
    struct aymo_(reg_20h) reg_20h;
    struct aymo_(reg_40h) reg_40h;
    struct aymo_(reg_60h) reg_60h;
    struct aymo_(reg_80h) reg_80h;
    struct aymo_(reg_E0h) reg_E0h;
    uint8_t pad32_[3];
};

struct aymo_(chan_regs) {
    struct aymo_(reg_A0h) reg_A0h;
    struct aymo_(reg_B0h) reg_B0h;
    struct aymo_(reg_C0h) reg_C0h;
    struct aymo_(reg_C0h) reg_D0h;
};

AYMO_PRAGMA_PACK_POP


#ifndef AYMO_YMF262_X86_AVX2_REG_QUEUE_LENGTH
#define AYMO_YMF262_X86_AVX2_REG_QUEUE_LENGTH       256
#endif
#ifndef AYMO_YMF262_X86_AVX2_REG_QUEUE_LATENCY
#define AYMO_YMF262_X86_AVX2_REG_QUEUE_LATENCY      2
#endif

struct aymo_(reg_queue_item) {
    uint16_t address;
    uint8_t value;
};

#define AYMO_YMF262_X86_AVX2_EG_TIMER_HIBIT         (1ULL << 36)
#define AYMO_YMF262_X86_AVX2_EG_TIMER_MASK          (AYMO_YMF262_X86_AVX2_EG_TIMER_HIBIT - 1ULL)


#define AYMO_YMF262_X86_AVX2_EG_GEN_ATTACK          0
#define AYMO_YMF262_X86_AVX2_EG_GEN_DECAY           1
#define AYMO_YMF262_X86_AVX2_EG_GEN_SUSTAIN         2
#define AYMO_YMF262_X86_AVX2_EG_GEN_RELEASE         3

#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_ATTACK    (1 <<  0)
#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_DECAY     (1 <<  4)
#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_SUSTAIN   (1 <<  8)
#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_RELEASE   (1 << 12)
#define AYMO_YMF262_X86_AVX2_EG_GEN_SRLHI           10

#define AYMO_YMF262_X86_AVX2_EG_KEY_NORMAL          (1 << 0)
#define AYMO_YMF262_X86_AVX2_EG_KEY_DRUM            (1 << 8)

// Packed ADSR register values
AYMO_ALIGN(4)
struct aymo_(eg_adsr) {
    uint16_t rr : 4;
    uint16_t sr : 4;
    uint16_t dr : 4;
    uint16_t ar : 4;
};


// Slot SIMD group status
// Processing order (kinda)
AYMO_ALIGN_V16
struct aymo_(slot_group) {
    aymo16_t wg_out;
    aymo16_t wg_prout;
    aymo16_t wg_fb_mulhi;
    aymo16_t wg_fbmod_gate;
    aymo16_t wg_prmod_gate;
    aymo16_t wg_phase_mullo;
    aymo16_t wg_phase_zero;
    aymo16_t wg_phase_neg;
    aymo16_t wg_phase_flip;
    aymo16_t wg_phase_mask;
    aymo16_t wg_sine_gate;

    aymo16_t og_prout;
    aymo16_t og_prout_ac;
    aymo16_t og_prout_bd;
    aymo16_t og_out_ch_gate_a;
    aymo16_t og_out_ch_gate_c;
    aymo16_t og_out_ch_gate_b;
    aymo16_t og_out_ch_gate_d;

    aymo16_t eg_rout;
    aymo16_t eg_tl_x4;
    aymo16_t eg_ksl_sh;
    aymo16_t eg_tremolo_am;
    aymo16_t eg_out;
    aymo16_t eg_gen;
    aymo16_t eg_sl;
    aymo16_t eg_key;            // bit 8 = drum, bit 0 = normal
    aymo16_t pg_notreset;
    aymo16_t eg_adsr;           // struct aymo_(eg_adsr)
    aymo16_t eg_gen_mullo;      // depends on reg_type for reg_sr
    aymo16_t eg_ks;

    aymo16_t pg_vib;
    aymo16_t pg_mult_x2;
    aymo32_t pg_deltafreq_lo;
    aymo32_t pg_deltafreq_hi;
    aymo32_t pg_phase_lo;
    aymo32_t pg_phase_hi;
    aymo16_t pg_phase_out;

    // Updated only by writing registers
    aymo16_t eg_am;
    aymo16_t og_out_gate;

#ifdef AYMO_DEBUG
    // Variables for debug
    aymo16_t eg_ksl;
    aymo16_t eg_rate;
    aymo16_t eg_inc;
    aymo16_t wg_fbmod;
    aymo16_t wg_mod;
#endif  // AYMO_dEBUG
};

// Channel_2xOP SIMD group status  // TODO
// Processing order (kinda)
AYMO_ALIGN_V16
struct aymo_(ch2x_group) {
    aymo16_t pg_fnum;
    aymo16_t pg_block;

    // Updated only by writing registers
    aymo16_t eg_ksv;

    aymo16_t og_ch_gate_a;
    aymo16_t og_ch_gate_b;
    aymo16_t og_ch_gate_c;
    aymo16_t og_ch_gate_d;

#ifdef AYMO_DEBUG
    // Variables for debug
#endif  // AYMO_dEBUG
};

// Chip SIMD and scalar status data
// Processing order (kinda), size/alignment order
AYMO_ALIGN_V16
struct aymo_(chip) {
    // Vector data
    struct aymo_(slot_group) sg[AYMO_(SLOT_GROUP_NUM)];
    struct aymo_(ch2x_group) cg[AYMO_(SLOT_GROUP_NUM) / 2];

    aymo16_t wg_mod;
    aymo16_t eg_statev;
    aymo16_t eg_add;
    aymo16_t eg_incstep;
    aymo16_t pg_vib_mulhi;
    aymo16_t pg_vib_neg;

    aymo16_t og_acc_a;
    aymo16_t og_acc_c;
    aymo16_t og_acc_b;
    aymo16_t og_acc_d;

    // 64-bit data
    uint64_t eg_timer;
    uint64_t tm_timer;

    // 32-bit data
    uint32_t rq_delay;
    int32_t og_sum_a;
    int32_t og_sum_c;
    int32_t og_sum_b;
    int32_t og_sum_d;
    uint32_t og_ch2x_pairing;
    uint32_t og_ch2x_drum;
    uint32_t ng_noise;

    // 16-bit data
    int16_t og_out_a;
    int16_t og_out_b;
    int16_t og_out_c;
    int16_t og_out_d;
    int16_t og_del_b;
    int16_t og_del_d;
    uint16_t rq_head;
    uint16_t rq_tail;

    // 8-bit data
    uint8_t eg_state;
    uint8_t rm_hh_bit2;
    uint8_t rm_hh_bit3;
    uint8_t rm_hh_bit7;
    uint8_t rm_hh_bit8;
    uint8_t rm_tc_bit3;
    uint8_t rm_tc_bit5;
    uint8_t eg_tremolopos;
    uint8_t eg_tremoloshift;
    uint8_t eg_vibshift;
    uint8_t pg_vibpos;
    uint8_t pad32_[3];

    struct aymo_(chip_regs) chip_regs;
    struct aymo_(slot_regs) slot_regs[AYMO_(SLOT_NUM_MAX)];
    struct aymo_(chan_regs) ch2x_regs[AYMO_(CHANNEL_NUM_MAX)];

    struct aymo_(reg_queue_item) rq_buffer[AYMO_(REG_QUEUE_LENGTH)];

#ifdef AYMO_DEBUG
    // Variables for debug
#endif  // AYMO_dEBUG
};


void aymo_(tick)(struct aymo_(chip)* chip);
void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
int aymo_(enqueue_write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
int aymo_(enqueue_delay)(struct aymo_(chip)* chip, uint32_t ticks);
size_t aymo_(size)(void);
void aymo_(init)(struct aymo_(chip)* chip);


#ifdef __GNUC__
    #pragma scalar_storage_order default
#endif


#if !defined(CONFIG_AYMO_KEEP_SHORTHANDS)
    #undef AYMO_
    #undef aymo_
#endif  // CONFIG_AYMO_KEEP_SHORTHANDS

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // include_aymo_ymf262_x86_avx2_specs_h_
