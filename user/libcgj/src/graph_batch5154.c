/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5154: test printable ASCII (0x20..0x7e).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tty_is_print_u(uint32_t c);
 *     - Return 1 if c is a printable ASCII code unit (0x20..0x7e
 *       inclusive), else 0. Excludes controls and DEL.
 *   uint32_t __gj_tty_is_print_u  (alias)
 *   __libcgj_batch5154_marker = "libcgj-batch5154"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Unique gj_tty_is_print_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5154_marker[] = "libcgj-batch5154";

/* Printable ASCII closed interval [0x20, 0x7e]. */
#define B5154_PRINT_LO  0x20u
#define B5154_PRINT_HI  0x7eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5154_is_print(uint32_t u32C)
{
	if (u32C >= B5154_PRINT_LO && u32C <= B5154_PRINT_HI) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tty_is_print_u - 1 if c is printable ASCII (0x20..0x7e), else 0.
 *
 * c: unsigned code unit to classify
 *
 * Returns 1 when c is in 0x20..0x7e inclusive, else 0. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_tty_is_print_u(uint32_t u32C)
{
	(void)NULL;
	return b5154_is_print(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tty_is_print_u(uint32_t u32C)
    __attribute__((alias("gj_tty_is_print_u")));
