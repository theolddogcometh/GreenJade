/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8158: strtok CSV-separator predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_is_csv_u_8158(uint32_t ch);
 *     - Return 1 if low octet is comma or semicolon, else 0.
 *   uint32_t __gj_strtok_is_csv_u_8158  (alias)
 *   __libcgj_batch8158_marker = "libcgj-batch8158"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_is_csv_u_8158
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8158_marker[] = "libcgj-batch8158";

/* Soft CSV / list field separators. */
#define B8158_COMMA  ((uint32_t)',')
#define B8158_SEMI   ((uint32_t)';')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8158_is_csv(uint32_t u32Ch)
{
	uint32_t c = u32Ch & 0xffu;

	if (c == B8158_COMMA || c == B8158_SEMI) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_is_csv_u_8158 - 1 if ch is a soft CSV field separator.
 *
 * ch: candidate octet (only low 8 bits inspected)
 *
 * Returns 1 for ',' or ';'; else 0. Does not call strtok. No parent
 * wires.
 */
uint32_t
gj_strtok_is_csv_u_8158(uint32_t u32Ch)
{
	(void)NULL;
	return b8158_is_csv(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_is_csv_u_8158(uint32_t u32Ch)
    __attribute__((alias("gj_strtok_is_csv_u_8158")));
