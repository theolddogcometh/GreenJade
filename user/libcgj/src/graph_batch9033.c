/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9033: soft XKB keysym id for Escape.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkb_key_esc_id_9033(void);
 *     - Returns 0xff1b (classic XKB_KEY_Escape / XK_Escape soft catalog
 *       id). Pure-data keymap stub; not a live keyboard event path.
 *   uint32_t __gj_xkb_key_esc_id_9033  (alias)
 *   __libcgj_batch9033_marker = "libcgj-batch9033"
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

const char __libcgj_batch9033_marker[] = "libcgj-batch9033";

/* Soft XKB_KEY_Escape / XK_Escape keysym catalog id. */
#define B9033_XKB_KEY_ESC_ID  0xff1bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9033_key_esc_id(void)
{
	return B9033_XKB_KEY_ESC_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkb_key_esc_id_9033 - soft XKB keysym catalog id for Escape.
 *
 * Always returns 0xff1b. Soft continuum constant; does not load keymaps
 * or call xkbcommon/X11. No parent wires.
 */
uint32_t
gj_xkb_key_esc_id_9033(void)
{
	(void)NULL;
	return b9033_key_esc_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkb_key_esc_id_9033(void)
    __attribute__((alias("gj_xkb_key_esc_id_9033")));
