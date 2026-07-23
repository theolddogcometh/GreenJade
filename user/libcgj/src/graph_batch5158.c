/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5158: TTY LF (line feed) control code unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tty_lf_u(void);
 *     - Returns the ASCII LF / newline code unit (10 / 0x0a).
 *   uint32_t __gj_tty_lf_u  (alias)
 *   __libcgj_batch5158_marker = "libcgj-batch5158"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Unique gj_tty_lf_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5158_marker[] = "libcgj-batch5158";

/* ASCII LF (line feed / newline). */
#define B5158_LF  10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5158_lf(void)
{
	return B5158_LF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tty_lf_u - ASCII LF / newline control code unit.
 *
 * Always returns 10 (0x0a). Does not call libc. No parent wires.
 */
uint32_t
gj_tty_lf_u(void)
{
	(void)NULL;
	return b5158_lf();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tty_lf_u(void)
    __attribute__((alias("gj_tty_lf_u")));
