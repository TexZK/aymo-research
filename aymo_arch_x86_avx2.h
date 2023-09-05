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

#ifndef include_aymo_arch_x86_avx2_h_
#define include_aymo_arch_x86_avx2_h_

#include "aymo_cc.h"
#ifdef AYMO_ARCH_IS_X86_AVX2

#include <immintrin.h>


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


typedef __m256i aymoi16_t;
typedef __m256i aymou16_t;

typedef __m256i aymoi32_t;
typedef __m256i aymou32_t;


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // AYMO_ARCH_IS_X86_AVX2
#endif  // include_aymo_arch_x86_avx2_h_
