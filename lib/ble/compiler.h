/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 ****************************************************************************************
 *
 * @file compiler.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifndef __GNUC__
#error "File only included with ARM GCC"
#endif // __GNUC__

/// define the static keyword for this compiler
#define __STATIC static

/// define the force inlining attribute for this compiler
#define __INLINE static __attribute__((__always_inline__)) inline

/// define the IRQ handler attribute for this compiler
#define __IRQ __attribute__((__interrupt__("IRQ")))

/// define the BLE IRQ handler attribute for this compiler
#define __BTIRQ

/// define the BLE IRQ handler attribute for this compiler
#define __BLEIRQ

/// define the FIQ handler attribute for this compiler
#define __FIQ __attribute__((__interrupt__("FIQ")))

/// define size of an empty array (used to declare structure with an array size not defined)
#define __ARRAY_EMPTY

/// Function returns struct in registers (4 in rvds, var with gnuarm).
/// With Gnuarm, feature depends on command line options and
/// impacts ALL functions returning 2-words max structs
/// (check -freg-struct-return and -mabi=xxx)
#define __VIR

/// function has no side effect and return depends only on arguments
#define __PURE __attribute__((const))

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__((aligned(4)))

/// __MODULE__ comes from the RVDS compiler that supports it
#define __MODULE__ __BASE_FILE__

/// Pack a structure field
#define __PACKED __attribute__ ((__packed__))

/// Put a variable in a memory maintained during deep sleep
#define __LOWPOWER_SAVED

#endif // _COMPILER_H_
