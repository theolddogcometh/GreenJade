/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9038: soft XKB layout-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkb_layout_ok_u_9038(void);
 *     - Returns 1 (XKB layout soft-id continuum ok). Pure-data product
 *       tag; does not load layouts or call xkbcommon/X11.
 *   uint32_t __gj_xkb_layout_ok_u_9038  (alias)
 *   __libcgj_batch9038_marker = "libcgj-batch9038"
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

const char __libcgj_batch9038_marker[] = "libcgj-batch9038";

/* Soft layout-ok lamp for xkb/keymap continuum. */
#define B9038_XKB_LAYOUT_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9038_layout_ok(void)
{
	return B9038_XKB_LAYOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkb_layout_ok_u_9038 - XKB layout soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not load keymaps
 * or call libc. No parent wires.
 */
uint32_t
gj_xkb_layout_ok_u_9038(void)
{
	(void)NULL;
	return b9038_layout_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkb_layout_ok_u_9038(void)
    __attribute__((alias("gj_xkb_layout_ok_u_9038")));
