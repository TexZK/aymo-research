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

#ifndef include_aymo_cc_specs_h_
#define include_aymo_cc_specs_h_


#if (defined(_DEBUG) && !defined(NDEBUG))
#define AYMO_DEBUG
#endif

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


#endif  // include_aymo_cc_specs_h_
