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

#ifndef include_aymo_arch_armv7_neon_h_
#define include_aymo_arch_armv7_neon_h_

#include "aymo_cc.h"
#ifdef AYMO_ARCH_IS_ARMV7_NEON

#include <arm_neon.h>


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


typedef int16x8_t aymoi16_t;
typedef uint16x8_t aymou16_t;

typedef int32x4_t aymoi32_t;
typedef uint32x4_t aymou32_t;


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // AYMO_ARCH_IS_ARMV7_NEON
#endif  // include_aymo_arch_armv7_neon_h_
