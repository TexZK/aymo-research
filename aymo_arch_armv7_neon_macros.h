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

#ifndef include_aymo_arch_armv7_neon_macros_h_
#define include_aymo_arch_armv7_neon_macros_h_

#include "aymo_arch_armv7_neon.h"
#ifdef AYMO_ARCH_IS_ARMV7_NEON

#include <stdint.h>
#if defined(_MSC_VER)
    #include <armintr.h>
#else
    #include <string.h>  // ffsll()
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


#define vi2u            vreinterpretq_u16_s16
#define vu2i            vreinterpretq_s16_u16

#define vsetx           vsetz
#define vset1           vdupq_n_s16
#define vseta           vseta_s16
#define vsetr           vsetr_s16
#define vsetz()         (vset1(0))
#define vsetf()         (vset1(-1))
#define vsetm           vsetm_s16

#define vnot            vmvnq_s16
#define vand            vandq_s16
#define vor             vorrq_s16
#define vxor            veorq_s16
#define vandnot(a,b)    (vbicq_s16((b), (a)))  // ~A & B
#define vblendv(a,b,m)  (vbslq_s16(vi2u(m), (b), (a)))  // B if M else A

#define vcmpeq(a, b)    (vu2i(vceqq_s16((a), (b))))
#define vcmpgt(a, b)    (vu2i(vcgtq_s16((a), (b))))
#define vcmpz(x)        (vcmpeq((x), vsetz()))
#define vcmpp(x)        (vcmpgt((x), vsetz()))

#define vadd            vaddq_s16
#define vaddsi          vqaddq_s16
#define vaddsu          vqaddq_u16

#define vsub            vsubq_s16
#define vsubsi          vqsubq_s16
#define vsubsu          vqsubq_u16
#define vneg            vnegq_s16

#define vslli           vshlq_n_s16
#define vsrli(x,n)      (vu2i(vshrq_n_u16(vi2u(x), (n))))
#define vsrai           vshrq_n_s16
#define vsllv           vshlq_s16
#define vsrlv(a,b)      (vu2i(vshlq_u16(vi2u(a), vnegq_s16(b))))
#define vsrav(a,b)      (vshlq_s16((a), vnegq_s16(b)))

#define vmini           vminq_s16
#define vminu           vminq_u16

#define vmaxi           vmaxq_s16
#define vmaxu           vmaxq_u16

#define vmul            vmulq_s16

#define vextract        vgetq_lane_s16
#define vextractn       vextractn_s16

#define vinsert(x,n,i)  (vsetq_lane_s16((n), (x), (i)))
#define vinsertn        vinsertn_s16

#define vgather         vgather_s16

#define vhsum           vhsum_s16
#define vhsums          vhsum

#define vpow2m1lt4      vpow2m1lt4_s16
#define vpow2lt4        vpow2lt4_s16

#define vgetlo          vget_low_s16
#define vgethi          vget_high_s16

#define vunpack         vmovl_s16
#define vunpacklo(x)    (vunpack(vgetlo(x)))
#define vunpackhi(x)    (vunpack(vgethi(x)))


#define wmul            vmull_s16

#define wcombine        vcombine_s16


#define vvi2u           vreinterpretq_u32_s32
#define vvu2i           vreinterpretq_s32_u32

#define vvsetx          vvsetz
#define vvset1          vdupq_n_s32
#define vvsetz()        (vvset1(0))
#define vvsetf()        (vvset1(-1))

#define vvand           vandq_s32
#define vvor            vorrq_s32
#define vvxor           veorq_s32
#define vvandnot(a,b)   (vbicq_s32((b), (a)))  // ~A & B

#define vvadd           vaddq_s32

#define vvsrli(x,n)     (vvu2i(vshrq_n_u32(vvi2u(x), (n))))

#define vvsllv          vshlq_s32

#define vvmul           vmulq_s32

#define vvextract       vgetq_lane_s32
#define vvextractn      vvextractn_s32

#define vvinsert(x,n,i) (vsetq_lane_s32((n), (x), (i)))
#define vvinsertn       vvinsertn_s32

#define vvgetlo         vget_low_s32
#define vvgethi         vget_high_s32

#define vvcombine       vcombine_s32
#define vvpack(a,b)     (vcombine_s16(vmovn_s32(a), vmovn_s32(b)))


AYMO_INLINE
int16x8_t vseta_s16(
    int16_t i7,
    int16_t i6,
    int16_t i5,
    int16_t i4,
    int16_t i3,
    int16_t i2,
    int16_t i1,
    int16_t i0
)
{
#if defined(_MSC_VER)
    int16x8_t r;
    r.n128_i16[0] = i0;
    r.n128_i16[1] = i1;
    r.n128_i16[2] = i2;
    r.n128_i16[3] = i3;
    r.n128_i16[4] = i4;
    r.n128_i16[5] = i5;
    r.n128_i16[6] = i6;
    r.n128_i16[7] = i7;
    return r;
#else
    int16x8_t r;
    int16_t* r_n128_i16 = (int16_t*)(void*)&r;
    r_n128_i16[0] = i0;
    r_n128_i16[1] = i1;
    r_n128_i16[2] = i2;
    r_n128_i16[3] = i3;
    r_n128_i16[4] = i4;
    r_n128_i16[5] = i5;
    r_n128_i16[6] = i6;
    r_n128_i16[7] = i7;
    return r;
#endif
}


AYMO_INLINE
int16x8_t vsetr_s16(
    int16_t i7, 
    int16_t i6, 
    int16_t i5, 
    int16_t i4, 
    int16_t i3, 
    int16_t i2,
    int16_t i1,
    int16_t i0
)
{
    return vseta(i0, i1, i2, i3, i4, i5, i6, i7);
}


AYMO_INLINE
int16x8_t vsetm_s16(uint8_t m)
{
    static const uint64_t table[16] = {
        0x0000000000000000ULL,
        0x000000000000FFFFULL,
        0x00000000FFFF0000ULL,
        0x00000000FFFFFFFFULL,
        0x0000FFFF00000000ULL,
        0x0000FFFF0000FFFFULL,
        0x0000FFFFFFFF0000ULL,
        0x0000FFFFFFFFFFFFULL,
        0xFFFF000000000000ULL,
        0xFFFF00000000FFFFULL,
        0xFFFF0000FFFF0000ULL,
        0xFFFF0000FFFFFFFFULL,
        0xFFFFFFFF00000000ULL,
        0xFFFFFFFF0000FFFFULL,
        0xFFFFFFFFFFFF0000ULL,
        0xFFFFFFFFFFFFFFFFULL,
    };

    return vcombine_s16(
        vcreate_s16(table[m & 15]),
        vcreate_s16(table[m >> 4])
    );
}


AYMO_INLINE
short vextractn_s16(int16x8_t x, const int i)
{
#if defined(_MSC_VER)
    return x.n128_i16[i];
#elif 1
    int16_t* x_n128i_i16 = (int16_t*)(void*)&x;
    return x_n128i_i16[i];
#else
    switch (i) {
    case  0: return vextract(x, 0);
    case  1: return vextract(x, 1);
    case  2: return vextract(x, 2);
    case  3: return vextract(x, 3);
    case  4: return vextract(x, 4);
    case  5: return vextract(x, 5);
    case  6: return vextract(x, 6);
    case  7: return vextract(x, 7);
    default: return 0;
    }
#endif
}


AYMO_INLINE
int16x8_t vinsertn_s16(int16x8_t x, short n, const int i)
{
#if defined(_MSC_VER)
    x.n128_i16[i] = n;
    return x;
#elif 1
    int16_t* x_n128_i16 = (int16_t*)(void*)&x;
    x_n128_i16[i] = n;
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


AYMO_INLINE
int16x8_t vgather_s16(const int16_t* v, int16x8_t i)
{
#if defined(_MSC_VER)
    int16x8_t r;
    r.n128_i16[0] = v[i.n128_u8[ 0]];
    r.n128_i16[1] = v[i.n128_u8[ 2]];
    r.n128_i16[2] = v[i.n128_u8[ 4]];
    r.n128_i16[3] = v[i.n128_u8[ 6]];
    r.n128_i16[4] = v[i.n128_u8[ 8]];
    r.n128_i16[5] = v[i.n128_u8[10]];
    r.n128_i16[6] = v[i.n128_u8[12]];
    r.n128_i16[7] = v[i.n128_u8[14]];
    return r;
#else
    int16x8_t r;
    int16_t* r_n128_i16 = (int16_t*)(void*)&r;
    uint8_t* i_n128_u8 = (uint8_t*)(void*)&i;
    r_n128_i16[0] = v[i_n128_u8[ 0]];
    r_n128_i16[1] = v[i_n128_u8[ 2]];
    r_n128_i16[2] = v[i_n128_u8[ 4]];
    r_n128_i16[3] = v[i_n128_u8[ 6]];
    r_n128_i16[4] = v[i_n128_u8[ 8]];
    r_n128_i16[5] = v[i_n128_u8[10]];
    r_n128_i16[6] = v[i_n128_u8[12]];
    r_n128_i16[7] = v[i_n128_u8[14]];
    return r;
#endif
}


AYMO_INLINE
int32_t vhsum_s16(int16x8_t x)
{
    int32x4_t sum16 = vpaddlq_s16(x);
    int64x2_t sum32 = vpaddlq_s32(sum16);
    int32x2_t lo32 = vreinterpret_s32_s64(vget_low_s64(sum32));
    int32x2_t hi32 = vreinterpret_s32_s64(vget_high_s64(sum32));
    return (vget_lane_s32(lo32, 0) + vget_lane_s32(hi32, 0));
}


// 0 <= x < 4  -->  (1 << (x - 1))  -->  0, 1, 2, 4
AYMO_INLINE
int16x8_t vpow2m1lt4_s16(int16x8_t x)
{
    return vsub(x, vcmpgt(x, vset1(2)));
}


// 0 <= x < 4  -->  (1 << x)
AYMO_INLINE
int16x8_t vpow2lt4_s16(int16x8_t x)
{
    return vsllv(vset1(1), x);
}


AYMO_INLINE
long vvextractn_s32(int32x4_t x, const int i)
{
#if defined(_MSC_VER)
    return x.n128_i32[i];
#elif 1
    int32_t* x_n128i_i32 = (int32_t*)(void*)&x;
    return x_n128i_i32[i];
#else
    switch (i) {
    case  0: return vvextract(x, 0);
    case  1: return vvextract(x, 1);
    case  2: return vvextract(x, 2);
    case  3: return vvextract(x, 3);
    default: return 0;
    }
#endif
}


AYMO_INLINE
int32x4_t vvinsertn_s32(int32x4_t x, long n, const int i)
{
#if defined(_MSC_VER)
    x.n128_i32[i] = n;
    return x;
#elif 1
    int32_t* x_n128_i32 = (int32_t*)(void*)&x;
    x_n128_i32[i] = n;
    return x;
#else
    switch (i) {
    case  0: return vvinsert(x, n, 0);
    case  1: return vvinsert(x, n, 1);
    case  2: return vvinsert(x, n, 2);
    case  3: return vvinsert(x, n, 3);
    default: return x;
    }
#endif
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


#ifdef _MSC_VER
// Finds first set bit = Counts trailing zeros
// Emulates the BSD function
AYMO_INLINE
int ffsll(long long x)
{
    unsigned long long u = (unsigned long long)x;
    unsigned lo = (unsigned)u;
    unsigned lon = _arm_clz(lo);
    if (lon < 32) {
        return lon;
    }
    unsigned hi = (unsigned)(u >> 32);
    unsigned hin = _arm_clz(hi);
    return (int)(hin + 32);
}
#endif  // _MSC_VER


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // AYMO_ARCH_IS_ARMV7_NEON
#endif  // include_aymo_arch_armv7_neon_macros_h_
