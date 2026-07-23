/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9031: soft XKB keysym id for Latin 'a'.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkb_key_a_id_9031(void);
 *     - Returns 0x0061 (classic XKB_KEY_a / XK_a soft catalog id).
 *       Pure-data keymap stub; not a live xkbcommon or X11 keymap.
 *   uint32_t __gj_xkb_key_a_id_9031  (alias)
 *   __libcgj_batch9031_marker = "libcgj-batch9031"
 *
 * Exclusive continuum CREATE-ONLY (9031-9040: xkb/keymap soft id stubs —
 * xkb_key_a_id_9031, xkb_key_enter_id_9032, xkb_key_esc_id_9033,
 * xkb_mod_shift_id_9034, xkb_mod_ctrl_id_9035, xkb_mod_alt_id_9036,
 * xkb_mod_super_id_9037, xkb_layout_ok_u_9038, xkb_ready_u_9039,
 * batch_id_9040). Unique surface only; no multi-def. No parent wires.
 * No __int128. No XKB implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9031_marker[] = "libcgj-batch9031";

/* Soft XKB_KEY_a / XK_a keysym catalog id. */
#define B9031_XKB_KEY_A_ID  0x0061u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9031_key_a_id(void)
{
	return B9031_XKB_KEY_A_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkb_key_a_id_9031 - soft XKB keysym catalog id for Latin 'a'.
 *
 * Always returns 0x0061. Soft continuum constant; does not load keymaps
 * or call xkbcommon/X11. No parent wires.
 */
uint32_t
gj_xkb_key_a_id_9031(void)
{
	(void)NULL;
	return b9031_key_a_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkb_key_a_id_9031(void)
    __attribute__((alias("gj_xkb_key_a_id_9031")));
