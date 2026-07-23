/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14030: fontconfig/IM soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14030(void);
 *     - Returns the compile-time graph batch number for this TU (14030).
 *   uint32_t __gj_batch_id_14030  (alias)
 *   __libcgj_batch14030_marker = "libcgj-batch14030"
 *
 * Exclusive continuum CREATE-ONLY (14021-14030: fontconfig/IM soft —
 * fc_config_gate_u_14021, fc_pattern_gate_u_14022, fc_size_gate_u_14023,
 * fcitx_gate_u_14024, ibus_gate_u_14025, im_context_gate_u_14026,
 * xim_gate_u_14027, fc_charset_gate_u_14028, fc_im_soft_ready_u_14029,
 * batch_id_14030). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128. No fontconfig / fcitx / ibus
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14030_marker[] = "libcgj-batch14030";

#define B14030_BATCH_ID  14030u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14030_id(void)
{
	return B14030_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14030 - report this TU's graph batch number.
 *
 * Always returns 14030. Soft continuum lamp for the 14021-14030
 * fontconfig/IM soft gates wave. No parent wires.
 */
uint32_t
gj_batch_id_14030(void)
{
	(void)NULL;
	return b14030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14030(void)
    __attribute__((alias("gj_batch_id_14030")));
