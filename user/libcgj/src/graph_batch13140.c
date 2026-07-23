/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13140: exclusive IM soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13140(void);
 *     - Returns the compile-time graph batch number for this TU (13140).
 *   uint32_t __gj_batch_id_13140  (alias)
 *   __libcgj_batch13140_marker = "libcgj-batch13140"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique gj_batch_id_13140
 * surface only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols. Distinct from
 * gj_batch_id_12940 (batch12940) and gj_batch_id_10440 (batch10440).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13140_marker[] = "libcgj-batch13140";

#define B13140_BATCH_ID  13140u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13140_id(void)
{
	return B13140_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13140 - report this TU's graph batch number.
 *
 * Always returns 13140. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_13140(void)
{
	(void)NULL;
	return b13140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13140(void)
    __attribute__((alias("gj_batch_id_13140")));
