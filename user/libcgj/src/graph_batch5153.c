/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5153: test whether a code unit is C0 control.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tty_is_ctrl_u(uint32_t c);
 *     - Return 1 if c is a C0 control (c < 32), else 0. Treats c as an
 *       unsigned code unit; values 0..31 are controls, 32+ are not.
 *   uint32_t __gj_tty_is_ctrl_u  (alias)
 *   __libcgj_batch5153_marker = "libcgj-batch5153"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Unique gj_tty_is_ctrl_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5153_marker[] = "libcgj-batch5153";

/* First non-control ASCII code unit (space). */
#define B5153_CTRL_LIMIT  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5153_is_ctrl(uint32_t u32C)
{
	if (u32C < B5153_CTRL_LIMIT) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tty_is_ctrl_u - 1 if c is a C0 control code unit (c < 32), else 0.
 *
 * c: unsigned code unit to classify
 *
 * Returns 1 when c is in 0..31, else 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_tty_is_ctrl_u(uint32_t u32C)
{
	(void)NULL;
	return b5153_is_ctrl(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tty_is_ctrl_u(uint32_t u32C)
    __attribute__((alias("gj_tty_is_ctrl_u")));
