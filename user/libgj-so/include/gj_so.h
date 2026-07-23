/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Public surface of product SO libgj-so.so.1 (SysV DT_HASH).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Defined data export — GLOB_DAT / JUMP_SLOT resolve target (value 0x42). */
extern volatile uint64_t gj_so_export;

/** Optional init: restores gj_so_export = 0x42. */
void gj_so_init(void);

/** Canonical marker value for smokes / resolve checks. */
#define GJ_SO_EXPORT_VALUE  ((uint64_t)0x42)

#ifdef __cplusplus
}
#endif
