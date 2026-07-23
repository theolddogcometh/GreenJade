/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5157: TTY HT (horizontal tab) control code unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tty_tab_u(void);
 *     - Returns the ASCII HT / TAB code unit (9 / 0x09).
 *   uint32_t __gj_tty_tab_u  (alias)
 *   __libcgj_batch5157_marker = "libcgj-batch5157"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Unique gj_tty_tab_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5157_marker[] = "libcgj-batch5157";

/* ASCII HT (horizontal tab). */
#define B5157_TAB  9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5157_tab(void)
{
	return B5157_TAB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tty_tab_u - ASCII HT / TAB control code unit.
 *
 * Always returns 9 (0x09). Does not call libc. No parent wires.
 */
uint32_t
gj_tty_tab_u(void)
{
	(void)NULL;
	return b5157_tab();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tty_tab_u(void)
    __attribute__((alias("gj_tty_tab_u")));
