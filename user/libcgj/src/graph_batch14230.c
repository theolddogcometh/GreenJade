/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14230: fontconfig/IM soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14230(void);
 *     - Returns the compile-time graph batch number for this TU (14230).
 *   uint32_t __gj_batch_id_14230  (alias)
 *   __libcgj_batch14230_marker = "libcgj-batch14230"
 *
 * Exclusive continuum CREATE-ONLY (14221-14230: fontconfig/IM soft —
 * fc_config_gate_u_14221, fc_pattern_gate_u_14222, fc_size_gate_u_14223,
 * fcitx_gate_u_14224, ibus_gate_u_14225, im_context_gate_u_14226,
 * xim_gate_u_14227, fc_charset_gate_u_14228, fc_im_soft_ready_u_14229,
 * batch_id_14230). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. Distinct from gj_batch_id_14030 (batch14030). No parent
 * wires. No __int128. No fontconfig / fcitx / ibus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14230_marker[] = "libcgj-batch14230";

#define B14230_BATCH_ID  14230u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14230_id(void)
{
	return B14230_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14230 - report this TU's graph batch number.
 *
 * Always returns 14230. Soft continuum lamp for the 14221-14230
 * fontconfig/IM soft gates wave. No parent wires.
 */
uint32_t
gj_batch_id_14230(void)
{
	(void)NULL;
	return b14230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14230(void)
    __attribute__((alias("gj_batch_id_14230")));
