/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7916: printf format flag character classify stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_is_flag_char_u_7916(uint32_t ch);
 *     - Return 1 if ch is a printf flag character among '-', '+', ' ',
 *       '#', '0'; else 0.
 *   uint32_t __gj_printf_is_flag_char_u_7916  (alias)
 *   __libcgj_batch7916_marker = "libcgj-batch7916"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_is_flag_char_u_7916 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7916_marker[] = "libcgj-batch7916";

/* Soft ASCII code points for printf flag characters. */
#define B7916_CH_MINUS  ((uint32_t)0x2Du) /* '-' */
#define B7916_CH_PLUS   ((uint32_t)0x2Bu) /* '+' */
#define B7916_CH_SPACE  ((uint32_t)0x20u) /* ' ' */
#define B7916_CH_HASH   ((uint32_t)0x23u) /* '#' */
#define B7916_CH_ZERO   ((uint32_t)0x30u) /* '0' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7916_is_flag_char(uint32_t u32Ch)
{
	if (u32Ch == B7916_CH_MINUS) {
		return 1u;
	}
	if (u32Ch == B7916_CH_PLUS) {
		return 1u;
	}
	if (u32Ch == B7916_CH_SPACE) {
		return 1u;
	}
	if (u32Ch == B7916_CH_HASH) {
		return 1u;
	}
	if (u32Ch == B7916_CH_ZERO) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_is_flag_char_u_7916 - 1 if ch is a printf format flag character.
 *
 * ch: soft character code (ASCII) from a format-string scan
 *
 * Returns 1 for '-', '+', ' ', '#', '0'; else 0. Pure classify; does not
 * call printf. No parent wires.
 */
uint32_t
gj_printf_is_flag_char_u_7916(uint32_t u32Ch)
{
	(void)NULL;
	return b7916_is_flag_char(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_is_flag_char_u_7916(uint32_t u32Ch)
    __attribute__((alias("gj_printf_is_flag_char_u_7916")));
