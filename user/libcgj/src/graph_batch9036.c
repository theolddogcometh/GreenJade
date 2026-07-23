/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9036: soft XKB Alt/Mod1 modifier mask id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkb_mod_alt_id_9036(void);
 *     - Returns 0x8 (classic XKB/X11 Mod1Mask / Alt soft catalog id,
 *       bit 3). Pure-data modifier stub; not a live modifier-state probe.
 *   uint32_t __gj_xkb_mod_alt_id_9036  (alias)
 *   __libcgj_batch9036_marker = "libcgj-batch9036"
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

const char __libcgj_batch9036_marker[] = "libcgj-batch9036";

/* Soft XKB/X11 Mod1Mask (Alt) catalog id (1 << 3). */
#define B9036_XKB_MOD_ALT_ID  0x8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9036_mod_alt_id(void)
{
	return B9036_XKB_MOD_ALT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkb_mod_alt_id_9036 - soft XKB Alt/Mod1 modifier mask catalog id.
 *
 * Always returns 0x8. Soft continuum constant; does not query modifier
 * state or call xkbcommon/X11. No parent wires.
 */
uint32_t
gj_xkb_mod_alt_id_9036(void)
{
	(void)NULL;
	return b9036_mod_alt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkb_mod_alt_id_9036(void)
    __attribute__((alias("gj_xkb_mod_alt_id_9036")));
