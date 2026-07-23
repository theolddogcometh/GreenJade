/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7919: printf space vs plus flag resolve stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_flag_space_vs_plus_u_7919(uint32_t flags);
 *     - If FLAG_PLUS (0x02) is set, clear FLAG_SPACE (0x04); else return
 *       flags unchanged (ISO C: '+' overrides ' ').
 *   uint32_t __gj_printf_flag_space_vs_plus_u_7919  (alias)
 *   __libcgj_batch7919_marker = "libcgj-batch7919"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_flag_space_vs_plus_u_7919 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7919_marker[] = "libcgj-batch7919";

#define B7919_FLAG_PLUS  ((uint32_t)0x02u)
#define B7919_FLAG_SPACE ((uint32_t)0x04u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7919_space_vs_plus(uint32_t u32Flags)
{
	if ((u32Flags & B7919_FLAG_PLUS) != 0u) {
		return u32Flags & ~B7919_FLAG_SPACE;
	}
	return u32Flags;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_flag_space_vs_plus_u_7919 - clear space flag when plus set.
 *
 * flags: soft packed printf flags word
 *
 * Returns flags with FLAG_SPACE cleared when FLAG_PLUS is set; otherwise
 * flags unchanged. Pure policy model of ISO C '+' over ' '. No parent wires.
 */
uint32_t
gj_printf_flag_space_vs_plus_u_7919(uint32_t u32Flags)
{
	(void)NULL;
	return b7919_space_vs_plus(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_flag_space_vs_plus_u_7919(uint32_t u32Flags)
    __attribute__((alias("gj_printf_flag_space_vs_plus_u_7919")));
