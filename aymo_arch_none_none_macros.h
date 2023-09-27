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

#ifndef include_aymo_arch_none_none_macros_h_
#define include_aymo_arch_none_none_macros_h_

#include "aymo_cc.h"

#include <intrin.h>
#include <stdint.h>
#if !defined(_MSC_VER)
    #include <string.h>  // ffsll()
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


#define vi2u(x)         ((uint16_t)(x))
#define vu2i(x)         ((int16_t)(x))

#define vsetx           vsetz
#define vset1(x)        ((int16_t)(x))
#define vseta           vset1
#define vsetr           vsetr
#define vsetz()         ((int16_t)0)
#define vsetf()         ((int16_t)-1)
#define vsetm(m)        ((m) ? vsetf() : vsetz())

#define vnot(x)         (vxor((x), vsetf()))
#define vand(a,b)       ((a) & (b))
#define vor(a,b)        ((a) | (b))
#define vxor(a,b)       ((a) ^ (b))
#define vandnot(a,b)    (vnot(a) & (b))  // ~A & B
#define vblendi(a,b,m)  ((m) ? (b) : (a))
#define vblendv         vblendi

#define vcmpeq(a,b)     ((a) == (b))
#define vcmpgt(a,b)     ((a) > (b))
#define vcmpz(x)        (vcmpeq((x), vsetz()))
#define vcmpp(x)        (vcmpgt((x), vsetz()))

#define vadd(a,b)       ((a) + (b))
#define vaddsi(a,b)     ((a) + (b))  // FIXME
#define vaddsu(a,b)     ((a) + (b))  // FIXME

#define vsub(a,b)       ((a) - (b))
#define vsubsi(a,b)     ((a) - (b))  // FIXME
#define vsubsu(a,b)     ((a) - (b))  // FIXME
#define vneg(x)         (-(x))

#define vslli(a,b)      ((a) << (b))
#define vsrli(a,b)      ((a) >> (b))
#define vsrai(a,b)      ((a) >> (b))
#define vsllv           vslli
#define vsrlv           vsrli
#define vsrav           vsrai

#define vmullo(a,b)     ((a) * (b))

#define vmini(a,b)      ((a) <= (b) ? (a) : (b))
#define vminu(a,b)      vmini
             
#define vmaxi(a,b)      ((a) >= (b) ? (a) : (b))
#define vmaxu(a,b)      vmaxi

#define vextract(a,i)   ((void)(i), (a))
#define vextractn       vextract

#define vinsert(a,n,i)  ((void)(i), ((a) = (n)))
#define vinsertn        vinsert

#define vgather(v,n)    ((v)[(n) & 0xFF])

#define vhsum(x)        (x)
#define vhsums(x)       (x)

#define vpow2m1lt4(x)   (1 << ((x) - 1))
#define vpow2lt4        (1 << (x))

#define vunpack(x)      ((int32_t)(x))


#define vvi2u(x)        ((uint32_t)(x))
#define vvu2i(x)        ((int32_t)(x))

#define vvsetx          vvsetz
#define vvset1(x)       (x)
#define vvsetz()        (0)
#define vvsetf()        (-1)

#define vvand(a,b)      ((a) & (b))
#define vvor(a,b)       ((a) | (b))
#define vvxor(a,b)      ((a) ^ (b))
#define vvandnot(a,b)   (vvxor((a), vsetf()) & (b))  // ~A & B

#define vvadd(a,b)      ((a) + (b))

#define vvsrli(x,n)     (vvu2i(vvi2u(x) >> (n)))

#define vvsllv(x,n)     ((x) << (n))

#define vvmullo(a,b)    ((a) * (b))

#define vvextract       // TOOD
#define vvextractn      vvextract

#define vvinsert(x,n,i) // TOOD
#define vvinsertn       vvinsert

#define vvpack(x)       ((int16_t)(x))


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
    unsigned long i = 0;
#if defined(_WIN32)
    if (_BitScanForward(&i, (uint32_t)x)) {
        return (int)(i + 1);
    }
    if (_BitScanForward(&i, (uint32_t)(x >> 32))) {
        return (int)(i + 33);
    }
#else
    if (_BitScanForward64(&i, (unsigned long long)x)) {
        return (int)(i + 1);
    }
#endif
    return 0;
}
#endif  // _MSC_VER


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // include_aymo_arch_none_none_macros_h_
