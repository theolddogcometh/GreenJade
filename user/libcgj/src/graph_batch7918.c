/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7918: printf zero-pad vs left-justify resolve stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_flag_resolve_zero_u_7918(uint32_t flags);
 *     - If FLAG_LEFT (0x01) is set, clear FLAG_ZERO (0x10); else return
 *       flags unchanged (ISO C: '-' overrides '0').
 *   uint32_t __gj_printf_flag_resolve_zero_u_7918  (alias)
 *   __libcgj_batch7918_marker = "libcgj-batch7918"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_flag_resolve_zero_u_7918 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7918_marker[] = "libcgj-batch7918";

#define B7918_FLAG_LEFT  ((uint32_t)0x01u)
#define B7918_FLAG_ZERO  ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7918_resolve_zero(uint32_t u32Flags)
{
	if ((u32Flags & B7918_FLAG_LEFT) != 0u) {
		return u32Flags & ~B7918_FLAG_ZERO;
	}
	return u32Flags;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_flag_resolve_zero_u_7918 - clear zero-pad when left-justify set.
 *
 * flags: soft packed printf flags word
 *
 * Returns flags with FLAG_ZERO cleared when FLAG_LEFT is set; otherwise
 * flags unchanged. Pure policy model of ISO C '-' over '0'. No parent wires.
 */
uint32_t
gj_printf_flag_resolve_zero_u_7918(uint32_t u32Flags)
{
	(void)NULL;
	return b7918_resolve_zero(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_flag_resolve_zero_u_7918(uint32_t u32Flags)
    __attribute__((alias("gj_printf_flag_resolve_zero_u_7918")));
