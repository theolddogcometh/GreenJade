/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8157: strtok whitespace-separator predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_is_ws_u_8157(uint32_t ch);
 *     - Return 1 if low octet is space/tab/nl/cr, else 0.
 *   uint32_t __gj_strtok_is_ws_u_8157  (alias)
 *   __libcgj_batch8157_marker = "libcgj-batch8157"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_is_ws_u_8157
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8157_marker[] = "libcgj-batch8157";

/* Soft whitespace delimiter set (space, HT, LF, CR). */
#define B8157_SPACE  ((uint32_t)' ')
#define B8157_TAB    ((uint32_t)'\t')
#define B8157_NL     ((uint32_t)'\n')
#define B8157_CR     ((uint32_t)'\r')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8157_is_ws(uint32_t u32Ch)
{
	uint32_t c = u32Ch & 0xffu;

	if (c == B8157_SPACE || c == B8157_TAB || c == B8157_NL ||
	    c == B8157_CR) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_is_ws_u_8157 - 1 if ch is a soft whitespace separator.
 *
 * ch: candidate octet (only low 8 bits inspected)
 *
 * Returns 1 for ' ', '\\t', '\\n', or '\\r'; else 0. Does not call
 * strtok or isspace. No parent wires.
 */
uint32_t
gj_strtok_is_ws_u_8157(uint32_t u32Ch)
{
	(void)NULL;
	return b8157_is_ws(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_is_ws_u_8157(uint32_t u32Ch)
    __attribute__((alias("gj_strtok_is_ws_u_8157")));
