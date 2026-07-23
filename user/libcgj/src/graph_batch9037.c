/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9037: soft XKB Super/Mod4 modifier mask id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkb_mod_super_id_9037(void);
 *     - Returns 0x40 (classic XKB/X11 Mod4Mask / Super soft catalog id,
 *       bit 6). Pure-data modifier stub; not a live modifier-state probe.
 *   uint32_t __gj_xkb_mod_super_id_9037  (alias)
 *   __libcgj_batch9037_marker = "libcgj-batch9037"
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

const char __libcgj_batch9037_marker[] = "libcgj-batch9037";

/* Soft XKB/X11 Mod4Mask (Super) catalog id (1 << 6). */
#define B9037_XKB_MOD_SUPER_ID  0x40u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9037_mod_super_id(void)
{
	return B9037_XKB_MOD_SUPER_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkb_mod_super_id_9037 - soft XKB Super/Mod4 modifier mask catalog id.
 *
 * Always returns 0x40. Soft continuum constant; does not query modifier
 * state or call xkbcommon/X11. No parent wires.
 */
uint32_t
gj_xkb_mod_super_id_9037(void)
{
	(void)NULL;
	return b9037_mod_super_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkb_mod_super_id_9037(void)
    __attribute__((alias("gj_xkb_mod_super_id_9037")));
