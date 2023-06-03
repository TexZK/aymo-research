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

#ifndef include_aymo_arch_x86_avx2_h_
#define include_aymo_arch_x86_avx2_h_

#include <immintrin.h>
#include <stdint.h>


#ifdef __cplusplus
    extern "C" {
#endif  // __cplusplus

#if defined(CONFIG_AYMO_YMF262_X86_AVX2_DEFINE_MACROS)

#undef AYMO_
#define AYMO_(TOKEN)    AYMO_YMF262_X86_AVX2_##TOKEN

#undef aymo_
#define aymo_(token)    aymo_ymf262_x86_avx2_##token

#ifndef AYMO_STATIC
#define AYMO_STATIC     static
#endif

#ifndef AYMO_INLINE
#define AYMO_INLINE     static inline __forceinline
#endif

#ifndef AYMO_API
#define AYMO_API        __stdcall
#endif

#ifndef AYMO_ALIGN
#define AYMO_ALIGN(n)   __declspec(align(n))
#endif

#ifndef AYMO_PRAGMA_PACK_PUSH_1
#define AYMO_PRAGMA_PACK_PUSH_1     _Pragma("pack(push, 1)")
#endif

#ifndef AYMO_PRAGMA_PACK_POP
#define AYMO_PRAGMA_PACK_POP        _Pragma("pack(pop)")
#endif

#ifndef CONFIG_AYMO_YMF262_X86_AVX2_GATHER16_STRATEGY
#define CONFIG_AYMO_YMF262_X86_AVX2_GATHER16_STRATEGY  2
#endif

#undef AYMO_ALIGN_V16
#define AYMO_ALIGN_V16  AYMO_ALIGN(64)

typedef __m256i aymo16_t;

#define vsetx       _mm256_undefined_si256
#define vset1       _mm256_set1_epi16
#define vseta       _mm256_set_epi16
#define vsetr       _mm256_setr_epi16
#define vsetz       _mm256_setzero_si256
#define vsetf()     (vset1(-1))

#define vnot(x)     (vxor((x), vsetf()))
#define vand        _mm256_and_si256
#define vor         _mm256_or_si256
#define vxor        _mm256_xor_si256
#define vandnot     _mm256_andnot_si256  // ~A & B
#define vblendi     _mm256_blend_epi16
#define vblendv     _mm256_blendv_epi8

#define vcmpeq      _mm256_cmpeq_epi16
#define vcmpgt      _mm256_cmpgt_epi16
#define vcmpz(x)    (vcmpeq((x), vsetz()))
#define vcmpp(x)    (vcmpgt((x), vsetz()))

#define vadd        _mm256_add_epi16
#define vaddsi      _mm256_adds_epi16
#define vaddsu      _mm256_adds_epu16

#define vsub        _mm256_sub_epi16
#define vsubsi      _mm256_subs_epi16
#define vsubsu      _mm256_subs_epu16
#define vneg(x)     (vsub(vsetz(), x))

#define vslli       _mm256_slli_epi16
#define vsrli       _mm256_srli_epi16
#define vsrai       _mm256_srai_epi16
#define vsllv        mm256_sllv_epi16
#define vsrlv        mm256_srlv_epi16

#define vmulihi     _mm256_mulhi_epi16
#define vmuluhi     _mm256_mulhi_epu16

#define vmulilo     _mm256_mullo_epi16
#define vmululo     _mm256_mullo_epi16

#define vmini       _mm256_min_epi16
#define vminu       _mm256_min_epu16

#define vmaxi       _mm256_max_epi16
#define vmaxu       _mm256_max_epu16

#define vextract    _mm256_extract_epi16
#define vextractn    mm256_extractn_epi16

#define vinsert     _mm256_insert_epi16
#define vinsertn     mm256_insertn_epi16

#define vgather      mm256_i16gather_epi16lo

#define vhsum        mm256_hsum_epi16
#define vhsums       mm256_hsums_epi16

#define vpow2m1lt4   mm256_pow2m1lt4_epi16
#define vpow2lt4     mm256_pow2lt4_epi16

#define vunpacklo   _mm256_unpacklo_epi16
#define vunpackhi   _mm256_unpackhi_epi16

#define vpackus     _mm256_packus_epi32

#define vvsetx      _mm256_undefined_si256
#define vvset1      _mm256_set1_epi32
#define vvseta      _mm256_set_epi32
#define vvsetr      _mm256_setr_epi32
#define vvsetz      _mm256_setzero_si256
#define vvsetf()    (vvset1(-1))

#define vvadd       _mm256_add_epi32

#define vvsrli      _mm256_srli_epi32

#define vvsllv      _mm256_sllv_epi32

#define vvmullo     _mm256_mullo_epi32


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
AYMO_INLINE
__m256i mm256_sllv_epi16(__m256i x, __m256i n)
{
    const __m256i m = _mm256_set1_epi32(0xFFFF0000);
    __m256i lo = _mm256_sllv_epi32(x, _mm256_andnot_si256(m, n));
    __m256i hi = _mm256_sllv_epi32(
        _mm256_and_si256(m, x),
        _mm256_srli_epi32(n, 16)
    );
    return _mm256_blend_epi16(lo, hi, 0xAA);
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
AYMO_INLINE
__m256i mm256_srlv_epi16(__m256i x, __m256i n)
{
    const __m256i m = _mm256_set1_epi32(0x0000FFFF);
    __m256i lo = _mm256_srlv_epi32(
        _mm256_and_si256(m, x),
        _mm256_and_si256(m, n)
    );
    __m256i hi = _mm256_srlv_epi32(x, _mm256_srli_epi32(n, 16));
    return _mm256_blend_epi16(lo, hi, 0xAA);
}


AYMO_INLINE
short mm256_extractn_epi16(__m256i x, const int i)
{
#if defined(_MSC_VER)
    {//if ((i >= 0) && (i < 16)) {
        return x.m256i_i16[i];
    }
    //return 0;
#elif 1
    {//if ((i >= 0) && (i < 16)) {
        volatile int16_t* m256i_i16 = (int16_t*)&x;
        return m256i_i16[i];
    }
    //return 0;
#else
    switch (i) {
    case  0: return vextract(x,  0);
    case  1: return vextract(x,  1);
    case  2: return vextract(x,  2);
    case  3: return vextract(x,  3);
    case  4: return vextract(x,  4);
    case  5: return vextract(x,  5);
    case  6: return vextract(x,  6);
    case  7: return vextract(x,  7);
    case  8: return vextract(x,  8);
    case  9: return vextract(x,  9);
    case 10: return vextract(x, 10);
    case 11: return vextract(x, 11);
    case 12: return vextract(x, 12);
    case 13: return vextract(x, 13);
    case 14: return vextract(x, 14);
    case 15: return vextract(x, 15);
    default: return 0;
    }
#endif
}


AYMO_INLINE
__m256i mm256_insertn_epi16(__m256i x, short n, const int i)
{
#if defined(_MSC_VER)
    {//if ((i >= 0) && (i < 16)) {
        x.m256i_i16[i] = n;
    }
    return x;
#elif 1
    {//if ((i >= 0) && (i < 16)) {
        volatile int16_t* m256i_i16 = (int16_t*)&x;
        m256i_i16[i] = n;
    }
    return x;
#else
    switch (i) {
    case  0: return vinsert(x, n, 0);
    case  1: return vinsert(x, n, 1);
    case  2: return vinsert(x, n, 2);
    case  3: return vinsert(x, n, 3);
    case  4: return vinsert(x, n, 4);
    case  5: return vinsert(x, n, 5);
    case  6: return vinsert(x, n, 6);
    case  7: return vinsert(x, n, 7);
    case  8: return vinsert(x, n, 8);
    case  9: return vinsert(x, n, 9);
    case 10: return vinsert(x, n, 10);
    case 11: return vinsert(x, n, 11);
    case 12: return vinsert(x, n, 12);
    case 13: return vinsert(x, n, 13);
    case 14: return vinsert(x, n, 14);
    case 15: return vinsert(x, n, 15);
    default: return x;
    }
#endif
}


// Gathers 16x 16-bit words via 16x 8-bit (low) indexes
AYMO_INLINE
__m256i mm256_i16gather_epi16lo(int16_t const* v, __m256i i)
{
#if (CONFIG_AYMO_YMF262_X86_AVX2_GATHER16_STRATEGY == 2)
    // 2x 32-bit gatherings, 16-bit words, smallest cache footprint
    const __m256i sl = _mm256_set_epi8(
        -1, -1, -1, 12, -1, -1, -1, 8, -1, -1, -1, 4, -1, -1, -1, 0,
        -1, -1, -1, 12, -1, -1, -1, 8, -1, -1, -1, 4, -1, -1, -1, 0
    );
    const __m256i sh = _mm256_set_epi8(
        -1, -1, -1, 14, -1, -1, -1, 10, -1, -1, -1, 6, -1, -1, -1, 2,
        -1, -1, -1, 14, -1, -1, -1, 10, -1, -1, -1, 6, -1, -1, -1, 2
    );
    __m256i jl = _mm256_shuffle_epi8(i, sl);
    __m256i rl = _mm256_i32gather_epi32((const int*)v, jl, 2);
    rl = _mm256_and_epi32(rl, _mm256_set1_epi32(0x0000FFFF));
    __m256i jh = _mm256_shuffle_epi8(i, sh);
    __m256i rh = _mm256_i32gather_epi32((const int*)v, jh, 2);
    rh = _mm256_slli_epi32(rh, 16);
    __m256i r = _mm256_or_si256(rl, rh);
    return r;

#elif (CONFIG_AYMO_X86_AVX2_GATHER16_STRATEGY == 1)
    // 1x 32-bit gathering, joint 16-bit words, squared cache footprint
    const __m256i s = _mm256_set_epi8(
        -1, -1, 14, 12, -1, -1, 10, 8, -1, -1, 6, 4, -1, -1, 2, 0,
        -1, -1, 14, 12, -1, -1, 10, 8, -1, -1, 6, 4, -1, -1, 2, 0
    );
    __m256i j = _mm256_shuffle_epi8(i, s);
    return _mm256_i32gather_epi32((const int32_t*)v, j, 4);

#else  // CONFIG_AYMO_X86_AVX2_GATHER16_STRATEGY
    // Plain C lookup, smallest cache footprint
    return vseta(
        v[vextract(i, 0xF)],
        v[vextract(i, 0xE)],
        v[vextract(i, 0xD)],
        v[vextract(i, 0xC)],
        v[vextract(i, 0xB)],
        v[vextract(i, 0xA)],
        v[vextract(i, 0x9)],
        v[vextract(i, 0x8)],
        v[vextract(i, 0x7)],
        v[vextract(i, 0x6)],
        v[vextract(i, 0x5)],
        v[vextract(i, 0x4)],
        v[vextract(i, 0x3)],
        v[vextract(i, 0x2)],
        v[vextract(i, 0x1)],
        v[vextract(i, 0x0)]
    );
#endif  // CONFIG_AYMO_X86_AVX2_GATHER16_STRATEGY
}


// see: https://stackoverflow.com/questions/60108658/fastest-method-to-calculate-sum-of-all-packed-32-bit-integers-using-avx512-or-av/
AYMO_INLINE
int mm_hsum_epi32(__m128i x)
{
    __m128i hi64 = _mm_unpackhi_epi64(x, x);
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);
}


AYMO_INLINE
int mm256_hsum_epi32(__m256i x)
{
    __m128i lo128 = _mm256_castsi256_si128(x);
    __m128i hi128 = _mm256_extracti128_si256(x, 1);
    __m128i sum32 = _mm_add_epi32(lo128, hi128);
    return mm_hsum_epi32(sum32);
}


// see: https://stackoverflow.com/questions/55057933/simd-accumulate-adjacent-pairs
AYMO_INLINE
int mm256_hsum_epi16(__m256i x)
{
    __m256i sum16 = _mm256_madd_epi16(x, vset1(1));
    return mm256_hsum_epi32(sum16);
}


AYMO_INLINE
int mm256_hsums_epi16(__m256i x)
{
    __m256i hs16 = _mm256_hadds_epi16(x, vsetz());
    __m256i sum16 = _mm256_unpacklo_epi16(hs16, vsetz());
    return mm256_hsum_epi32(sum16);
}


// 0 <= x < 4  -->  (1 << (x - 1))  -->  0, 1, 2, 4
AYMO_INLINE
__m256i mm256_pow2m1lt4_epi16(__m256i x)
{
    return vsub(x, vcmpgt(x, vset1(2)));
}


// 0 <= x < 4  -->  (1 << x)
AYMO_INLINE
__m256i mm256_pow2lt4_epi16(__m256i x)
{
    __m256i a = vadd(x, vset1(1));
    __m256i b = vsubsu(x, vset1(2));
    __m256i c = vmululo(b, b);
    return vadd(a, c);
}


AYMO_INLINE
int16_t clamp16(int x)
{
    if (x < INT16_MIN) {
        return (int16_t)INT16_MIN;
    }
    if (x > INT16_MAX) {
        return (int16_t)INT16_MAX;
    }
    return (int16_t)x;
}


// Finds first set bit = Counts trailing zeros
// Emulates the BSD function
AYMO_INLINE
int ffsll(long long x)
{
    unsigned long i = 0;
    if (_BitScanForward64(&i, (unsigned long long)x)) {
        return (int)(i + 1);
    }
    return 0;
}



#endif  // CONFIG_AYMO_YMF262_X86_AVX2_DEFINE_MACROS
#if !defined(CONFIG_AYMO_YMF262_X86_AVX2_SKIP_SPECS)


#define AYMO_YMF262_X86_AVX2_SLOT_NUM_MAX           64
#define AYMO_YMF262_X86_AVX2_SLOT_NUM               36
#define AYMO_YMF262_X86_AVX2_CHANNEL_NUM_MAX        32
#define AYMO_YMF262_X86_AVX2_CHANNEL_NUM            18
#define AYMO_YMF262_X86_AVX2_SLOT_GROUP_NUM         4
#define AYMO_YMF262_X86_AVX2_SLOT_GROUP_LENGTH      16
#define AYMO_YMF262_X86_AVX2_CONN_NUM_MAX           6
#define AYMO_YMF262_X86_AVX2_SAMPLE_RATE            49716


#endif  // !CONFIG_AYMO_YMF262_X86_AVX2_SKIP_SPECS
#if defined(CONFIG_AYMO_YMF262_X86_AVX2_DEFINE_TYPES)

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
    aymo16_t og_out_ch_gate_a;
    aymo16_t og_out_ch_gate_b;
    aymo16_t og_out_ch_gate_c;
    aymo16_t og_out_ch_gate_d;

    aymo16_t eg_rout;
    aymo16_t eg_tl_x4;
    aymo16_t eg_ksl_sh;
    aymo16_t eg_am;
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
    aymo16_t pg_phase_lo;
    aymo16_t pg_phase_hi;
    aymo16_t pg_phase_out;

    // Updated only by writing registers
    aymo16_t og_out_gate;
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
};

// Chip SIMD and scalar status data
// Processing order (kinda), size/alignment order
AYMO_ALIGN_V16
struct aymo_(chip) {
    // Vector data
    struct aymo_(slot_group) sg[AYMO_(SLOT_GROUP_NUM)];
    struct aymo_(ch2x_group) cg[AYMO_(SLOT_GROUP_NUM) / 2];

    aymo16_t wg_mod;
    aymo16_t eg_tremolo;
    aymo16_t eg_statev;
    aymo16_t eg_add;
    aymo16_t eg_incstep;
    aymo16_t pg_vib_mulhi;

    aymo16_t og_acc_a;
    aymo16_t og_acc_b;
    aymo16_t og_acc_c;
    aymo16_t og_acc_d;

    // 64-bit data
    uint64_t eg_timer;
    uint64_t rq_timestamp;

    // 32-bit data
    uint32_t rq_delay;
    int32_t og_sum_a;
    int32_t og_sum_b;
    int32_t og_sum_c;
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
    uint16_t tm_timer;
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

#endif  // CONFIG_AYMO_YMF262_X86_AVX2_DEFINE_TYPES

#if !defined(CONFIG_AYMO_KEEP_SHORTHANDS)
    #undef AYMO_
    #undef aymo_
#endif  // CONFIG_AYMO_KEEP_SHORTHANDS

#ifdef __cplusplus
    }  // extern "C"
#endif  // __cplusplus

#endif  // include_aymo_arch_x86_avx2_h_
