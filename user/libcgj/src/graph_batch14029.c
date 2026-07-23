/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14029: fontconfig/IM soft continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_im_soft_ready_u_14029(void);
 *     - Returns 1 (soft lamp only). Indicates the fontconfig/IM soft
 *       gate surfaces in this continuum are present; not runtime
 *       fontconfig/fcitx/ibus readiness.
 *   uint32_t __gj_fc_im_soft_ready_u_14029  (alias)
 *   __libcgj_batch14029_marker = "libcgj-batch14029"
 *
 * Exclusive continuum CREATE-ONLY (14021-14030: fontconfig/IM soft —
 * fc_config_gate_u_14021, fc_pattern_gate_u_14022, fc_size_gate_u_14023,
 * fcitx_gate_u_14024, ibus_gate_u_14025, im_context_gate_u_14026,
 * xim_gate_u_14027, fc_charset_gate_u_14028, fc_im_soft_ready_u_14029,
 * batch_id_14030). Unique surface only; no multi-def. Distinct from
 * gj_fontconfig_ready_u_9049 and gj_im_soft_ready_u_10139. No parent
 * wires. No __int128. No fontconfig / fcitx / ibus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14029_marker[] = "libcgj-batch14029";

/* Soft continuum lamp: fontconfig/IM soft-gate surfaces present. */
#define B14029_FC_IM_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14029_ready(void)
{
	return B14029_FC_IM_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_im_soft_ready_u_14029 - fontconfig/IM soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 14021-14030 surfaces are present. Does not claim fontconfig/fcitx/
 * ibus ready and does not call those libraries or libc. No parent
 * wires.
 */
uint32_t
gj_fc_im_soft_ready_u_14029(void)
{
	(void)NULL;
	return b14029_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_im_soft_ready_u_14029(void)
    __attribute__((alias("gj_fc_im_soft_ready_u_14029")));
