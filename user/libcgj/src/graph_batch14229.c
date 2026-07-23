/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14229: fontconfig/IM soft continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_im_soft_ready_u_14229(void);
 *     - Returns 1 (soft lamp only). Indicates the fontconfig/IM soft
 *       gate surfaces in this continuum are present; not runtime
 *       fontconfig/fcitx/ibus readiness.
 *   uint32_t __gj_fc_im_soft_ready_u_14229  (alias)
 *   __libcgj_batch14229_marker = "libcgj-batch14229"
 *
 * Exclusive continuum CREATE-ONLY (14221-14230: fontconfig/IM soft —
 * fc_config_gate_u_14221, fc_pattern_gate_u_14222, fc_size_gate_u_14223,
 * fcitx_gate_u_14224, ibus_gate_u_14225, im_context_gate_u_14226,
 * xim_gate_u_14227, fc_charset_gate_u_14228, fc_im_soft_ready_u_14229,
 * batch_id_14230). Unique surface only; no multi-def. Distinct from
 * gj_fc_im_soft_ready_u_14029 (batch14029), gj_fontconfig_ready_u_9049,
 * and gj_im_soft_ready_u_10139. No parent wires. No __int128. No
 * fontconfig / fcitx / ibus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14229_marker[] = "libcgj-batch14229";

/* Soft continuum lamp: fontconfig/IM soft-gate surfaces present. */
#define B14229_FC_IM_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14229_ready(void)
{
	return B14229_FC_IM_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_im_soft_ready_u_14229 - fontconfig/IM soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 14221-14230 surfaces are present. Does not claim fontconfig/fcitx/
 * ibus ready and does not call those libraries or libc. No parent
 * wires.
 */
uint32_t
gj_fc_im_soft_ready_u_14229(void)
{
	(void)NULL;
	return b14229_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_im_soft_ready_u_14229(void)
    __attribute__((alias("gj_fc_im_soft_ready_u_14229")));
