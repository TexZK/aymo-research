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

#ifndef include_aymo_arch_x86_sse41_macros_h_
#define include_aymo_arch_x86_sse41_macros_h_

#include "aymo_arch_x86_sse41_specs.h"

#include <intrin.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


#define vsetx       _mm_undefined_si128
#define vset1       _mm_set1_epi16
#define vseta       _mm_set_epi16
#define vsetr       _mm_setr_epi16
#define vsetz       _mm_setzero_si128
#define vsetf()     (vset1(-1))
#define vsetm        mm_setm_epi16

#define vnot(x)     (vxor((x), vsetf()))
#define vand        _mm_and_si128
#define vor         _mm_or_si128
#define vxor        _mm_xor_si128
#define vandnot     _mm_andnot_si128  // ~A & B
#define vblendi     _mm_blend_epi16
#define vblendv     _mm_blendv_epi8

#define vcmpeq      _mm_cmpeq_epi16
#define vcmpgt      _mm_cmpgt_epi16
#define vcmpz(x)    (vcmpeq((x), vsetz()))
#define vcmpp(x)    (vcmpgt((x), vsetz()))

#define vadd        _mm_add_epi16
#define vaddsi      _mm_adds_epi16
#define vaddsu      _mm_adds_epu16

#define vsub        _mm_sub_epi16
#define vsubsi      _mm_subs_epi16
#define vsubsu      _mm_subs_epu16
#define vneg(x)     (vsub(vsetz(), x))

#define vslli       _mm_slli_epi16
#define vsrli       _mm_srli_epi16
#define vsrai       _mm_srai_epi16
#define vsllv        mm_sllv_epi16
#define vsrlv        mm_srlv_epi16
#define vsrav        mm_srav_epi16

#define vmulihi     _mm_mulhi_epi16
#define vmuluhi     _mm_mulhi_epu16

#define vmulilo     _mm_mullo_epi16
#define vmululo     _mm_mullo_epi16

#define vmini       _mm_min_epi16
#define vminu       _mm_min_epu16

#define vmaxi       _mm_max_epi16
#define vmaxu       _mm_max_epu16

#define vextract    _mm_extract_epi16
#define vextractn    mm_extractn_epi16

#define vinsert     _mm_insert_epi16
#define vinsertn     mm_insertn_epi16

#define vgather      mm_i16gather_epi16lo

#define vhsum        mm_hsum_epi16
#define vhsums       mm_hsums_epi16

#define vpow2m1lt4   mm_pow2m1lt4_epi16
#define vpow2lt4     mm_pow2lt4_epi16

#define vunpacklo   _mm_unpacklo_epi16
#define vunpackhi   _mm_unpackhi_epi16

#define vpackus     _mm_packus_epi32


#define vvsetx      _mm_undefined_si128
#define vvset1      _mm_set1_epi32
#define vvseta      _mm_set_epi32
#define vvsetr      _mm_setr_epi32
#define vvsetz      _mm_setzero_si128
#define vvsetf()    (vvset1(-1))

#define vvand       vand
#define vvor        vor
#define vvxor       vxor
#define vvandnot    vandnot

#define vvadd       _mm_add_epi32

#define vvsrli      _mm_srli_epi32

#define vvsllv      _mm_sllv_epi32

#define vvmullo     _mm_mullo_epi32


AYMO_INLINE
__m128i mm_setm_epi16(uint8_t m)
{
    __m128i x = vsetr(
        (((m >>  0) & 1) ? -1 : 0),
        (((m >>  1) & 1) ? -1 : 0),
        (((m >>  2) & 1) ? -1 : 0),
        (((m >>  3) & 1) ? -1 : 0),
        (((m >>  4) & 1) ? -1 : 0),
        (((m >>  5) & 1) ? -1 : 0),
        (((m >>  6) & 1) ? -1 : 0),
        (((m >>  7) & 1) ? -1 : 0)
    );
    return x;
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
AYMO_INLINE
__m128i mm_sllv_epi16(__m128i x, __m128i n)
{
    const __m128i m = _mm_set1_epi32(0xFFFF0000);
    __m128i lo = _mm_sllv_epi32(x, _mm_andnot_si128(m, n));
    __m128i hi = _mm_sllv_epi32(
        _mm_and_si128(m, x),
        _mm_srli_epi32(n, 16)
    );
    return _mm_blend_epi16(lo, hi, 0xAA);
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
AYMO_INLINE
__m128i mm_srlv_epi16(__m128i x, __m128i n)
{
    const __m128i m = _mm_set1_epi32(0x0000FFFF);
    __m128i lo = _mm_srlv_epi32(
        _mm_and_si128(m, x),
        _mm_and_si128(m, n)
    );
    __m128i hi = _mm_srlv_epi32(x, _mm_srli_epi32(n, 16));
    return _mm_blend_epi16(lo, hi, 0xAA);
}


// see: https://stackoverflow.com/questions/51789685/reproduce-mm256-sllv-epi16-and-mm256-sllv-epi8-in-avx2/51805592#51805592
AYMO_INLINE
__m128i mm_srav_epi16(__m128i x, __m128i n)
{
    const __m128i m = _mm_set1_epi32(0x0000FFFF);
    __m128i lo = _mm_srav_epi32(
        _mm_and_si128(m, x),
        _mm_and_si128(m, n)
    );
    __m128i hi = _mm_srav_epi32(x, _mm_srli_epi32(n, 16));
    return _mm_blend_epi16(lo, hi, 0xAA);
}


AYMO_INLINE
short mm_extractn_epi16(__m128i x, const int i)
{
#if defined(_MSC_VER)
    {//if ((i >= 0) && (i < 16)) {
        return x.m128i_i16[i];
    }
    //return 0;
#elif 1
    {//if ((i >= 0) && (i < 16)) {
        volatile int16_t* m128i_i16 = (int16_t*)&x;
        return m128i_i16[i];
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
    default: return 0;
    }
#endif
}


AYMO_INLINE
__m128i mm_insertn_epi16(__m128i x, short n, const int i)
{
#if defined(_MSC_VER)
    {//if ((i >= 0) && (i < 16)) {
        x.m128i_i16[i] = n;
    }
    return x;
#elif 1
    {//if ((i >= 0) && (i < 16)) {
        volatile int16_t* m128i_i16 = (int16_t*)&x;
        m128i_i16[i] = n;
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
    default: return x;
    }
#endif
}


// Gathers 16x 16-bit words via 16x 8-bit (low) indexes
AYMO_INLINE
__m128i mm_i16gather_epi16lo(int16_t const* v, __m128i i)
{
    // Plain C lookup, smallest cache footprint
    i = _mm_and_si128(i, vset1(0x00FF));
    return vsetr(
        v[vextract(i, 0x0)],
        v[vextract(i, 0x1)],
        v[vextract(i, 0x2)],
        v[vextract(i, 0x3)],
        v[vextract(i, 0x4)],
        v[vextract(i, 0x5)],
        v[vextract(i, 0x6)],
        v[vextract(i, 0x7)]
    );
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


// see: https://stackoverflow.com/questions/55057933/simd-accumulate-adjacent-pairs
AYMO_INLINE
int mm_hsum_epi16(__m128i x)
{
    __m128i sum16 = _mm_madd_epi16(x, vset1(1));
    return mm_hsum_epi32(sum16);
}


AYMO_INLINE
int mm_hsums_epi16(__m128i x)
{
    __m128i hs16 = _mm_hadds_epi16(x, vsetz());
    __m128i sum16 = _mm_unpacklo_epi16(hs16, vsetz());
    return mm_hsum_epi32(sum16);
}


// 0 <= x < 4  -->  (1 << (x - 1))  -->  0, 1, 2, 4
AYMO_INLINE
__m128i mm_pow2m1lt4_epi16(__m128i x)
{
    return vsub(x, vcmpgt(x, vset1(2)));
}


// 0 <= x < 4  -->  (1 << x)
AYMO_INLINE
__m128i mm_pow2lt4_epi16(__m128i x)
{
    __m128i a = vadd(x, vset1(1));
    __m128i b = vsubsu(x, vset1(2));
    __m128i c = vmululo(b, b);
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


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // include_aymo_arch_x86_sse41_macros_h_
