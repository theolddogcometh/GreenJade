/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5155: map a code unit to its C0 control form.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tty_to_ctrl_u(uint32_t c);
 *     - Return c & 0x1f (low five bits), the classic Ctrl+letter map
 *       used by terminals (e.g. 'A' / 0x41 -> 0x01).
 *   uint32_t __gj_tty_to_ctrl_u  (alias)
 *   __libcgj_batch5155_marker = "libcgj-batch5155"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Unique gj_tty_to_ctrl_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5155_marker[] = "libcgj-batch5155";

/* Low five bits — C0 control mask (Ctrl+letter). */
#define B5155_CTRL_MASK  0x1fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5155_to_ctrl(uint32_t u32C)
{
	return u32C & B5155_CTRL_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tty_to_ctrl_u - map code unit to C0 control via c & 0x1f.
 *
 * c: unsigned code unit (typically an ASCII letter)
 *
 * Returns the low five bits of c. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_tty_to_ctrl_u(uint32_t u32C)
{
	(void)NULL;
	return b5155_to_ctrl(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tty_to_ctrl_u(uint32_t u32C)
    __attribute__((alias("gj_tty_to_ctrl_u")));
