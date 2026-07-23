/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7917: printf format flag-from-char map stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_flag_from_char_u_7917(uint32_t ch);
 *     - Map a printf flag character to its soft flags bit
 *       ('-'→0x01, '+'→0x02, ' '→0x04, '#'→0x08, '0'→0x10); else 0.
 *   uint32_t __gj_printf_flag_from_char_u_7917  (alias)
 *   __libcgj_batch7917_marker = "libcgj-batch7917"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_flag_from_char_u_7917 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7917_marker[] = "libcgj-batch7917";

#define B7917_CH_MINUS  ((uint32_t)0x2Du)
#define B7917_CH_PLUS   ((uint32_t)0x2Bu)
#define B7917_CH_SPACE  ((uint32_t)0x20u)
#define B7917_CH_HASH   ((uint32_t)0x23u)
#define B7917_CH_ZERO   ((uint32_t)0x30u)

#define B7917_FLAG_LEFT  ((uint32_t)0x01u)
#define B7917_FLAG_PLUS  ((uint32_t)0x02u)
#define B7917_FLAG_SPACE ((uint32_t)0x04u)
#define B7917_FLAG_ALT   ((uint32_t)0x08u)
#define B7917_FLAG_ZERO  ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7917_flag_from_char(uint32_t u32Ch)
{
	if (u32Ch == B7917_CH_MINUS) {
		return B7917_FLAG_LEFT;
	}
	if (u32Ch == B7917_CH_PLUS) {
		return B7917_FLAG_PLUS;
	}
	if (u32Ch == B7917_CH_SPACE) {
		return B7917_FLAG_SPACE;
	}
	if (u32Ch == B7917_CH_HASH) {
		return B7917_FLAG_ALT;
	}
	if (u32Ch == B7917_CH_ZERO) {
		return B7917_FLAG_ZERO;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_flag_from_char_u_7917 - map flag character to soft flags bit.
 *
 * ch: soft character code from a format-string flag scan
 *
 * Returns 0x01/0x02/0x04/0x08/0x10 for '-'+' '#'0 respectively; else 0.
 * Pure map; does not call printf. No parent wires.
 */
uint32_t
gj_printf_flag_from_char_u_7917(uint32_t u32Ch)
{
	(void)NULL;
	return b7917_flag_from_char(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_flag_from_char_u_7917(uint32_t u32Ch)
    __attribute__((alias("gj_printf_flag_from_char_u_7917")));
