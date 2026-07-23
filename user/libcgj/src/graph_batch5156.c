/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5156: TTY BEL (bell) control code unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tty_bell_u(void);
 *     - Returns the ASCII BEL code unit (7 / 0x07).
 *   uint32_t __gj_tty_bell_u  (alias)
 *   __libcgj_batch5156_marker = "libcgj-batch5156"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Unique gj_tty_bell_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5156_marker[] = "libcgj-batch5156";

/* ASCII BEL (audible / visual alert). */
#define B5156_BEL  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5156_bell(void)
{
	return B5156_BEL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tty_bell_u - ASCII BEL control code unit.
 *
 * Always returns 7 (0x07). Does not call libc. No parent wires.
 */
uint32_t
gj_tty_bell_u(void)
{
	(void)NULL;
	return b5156_bell();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tty_bell_u(void)
    __attribute__((alias("gj_tty_bell_u")));
