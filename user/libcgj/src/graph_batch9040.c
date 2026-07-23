/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9040: xkb/keymap soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9040(void);
 *     - Returns the compile-time graph batch number for this TU (9040).
 *   uint32_t __gj_batch_id_9040  (alias)
 *   __libcgj_batch9040_marker = "libcgj-batch9040"
 *
 * Exclusive continuum CREATE-ONLY (9031-9040: xkb/keymap soft id stubs —
 * xkb_key_a_id_9031, xkb_key_enter_id_9032, xkb_key_esc_id_9033,
 * xkb_mod_shift_id_9034, xkb_mod_ctrl_id_9035, xkb_mod_alt_id_9036,
 * xkb_mod_super_id_9037, xkb_layout_ok_u_9038, xkb_ready_u_9039,
 * batch_id_9040). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128. No XKB implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9040_marker[] = "libcgj-batch9040";

#define B9040_BATCH_ID  9040u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9040_id(void)
{
	return B9040_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9040 - report this TU's graph batch number.
 *
 * Always returns 9040.
 */
uint32_t
gj_batch_id_9040(void)
{
	(void)NULL;
	return b9040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9040(void)
    __attribute__((alias("gj_batch_id_9040")));
