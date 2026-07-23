/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5152: default TTY row count (classic 25).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tty_rows_default_u(void);
 *     - Returns the conventional default terminal height in rows (25).
 *   uint32_t __gj_tty_rows_default_u  (alias)
 *   __libcgj_batch5152_marker = "libcgj-batch5152"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Unique gj_tty_rows_default_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5152_marker[] = "libcgj-batch5152";

/* Classic VT / console default row count. */
#define B5152_ROWS_DEFAULT  25u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5152_rows_default(void)
{
	return B5152_ROWS_DEFAULT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tty_rows_default_u - conventional default TTY height in rows.
 *
 * Always returns 25 (classic terminal geometry). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_tty_rows_default_u(void)
{
	(void)NULL;
	return b5152_rows_default();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tty_rows_default_u(void)
    __attribute__((alias("gj_tty_rows_default_u")));
