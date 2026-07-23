/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7927: scanf conversion assignment-suppress stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_suppress_id_7927(void);
 *     - Return soft assignment-suppression marker (ASCII '*', 0x2A).
 *   uint32_t gj_scanf_has_suppress_u_7927(uint32_t flags);
 *     - Return 1 if flags has suppress bit (0x1) set, else 0.
 *   uint32_t __gj_scanf_suppress_id_7927  (alias)
 *   uint32_t __gj_scanf_has_suppress_u_7927  (alias)
 *   __libcgj_batch7927_marker = "libcgj-batch7927"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_*_7927 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7927_marker[] = "libcgj-batch7927";

/* Soft assignment-suppression catalog: '*' and internal flag bit. */
#define B7927_SUPPRESS_CHAR  ((uint32_t)0x2Au) /* '*' */
#define B7927_SUPPRESS_BIT   ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7927_suppress_id(void)
{
	return B7927_SUPPRESS_CHAR;
}

static uint32_t
b7927_has_suppress(uint32_t u32Flags)
{
	return ((u32Flags & B7927_SUPPRESS_BIT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_suppress_id_7927 - soft scanf assignment-suppression marker.
 *
 * Always returns 0x2A (ASCII '*'). Catalog id only; does not parse
 * format strings. No parent wires.
 */
uint32_t
gj_scanf_suppress_id_7927(void)
{
	(void)NULL;
	return b7927_suppress_id();
}

/*
 * gj_scanf_has_suppress_u_7927 - soft suppress-bit presence test.
 *
 * flags: conversion flag word (bit 0 = assignment suppress)
 *
 * Returns 1 when suppress bit is set, else 0.
 */
uint32_t
gj_scanf_has_suppress_u_7927(uint32_t u32Flags)
{
	return b7927_has_suppress(u32Flags);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_scanf_suppress_id_7927(void)
    __attribute__((alias("gj_scanf_suppress_id_7927")));

uint32_t __gj_scanf_has_suppress_u_7927(uint32_t u32Flags)
    __attribute__((alias("gj_scanf_has_suppress_u_7927")));
