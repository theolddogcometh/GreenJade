/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14021: fontconfig FcConfig soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_config_gate_u_14021(uint32_t cfg_flags);
 *     - Soft gate: cfg_flags in [0, 0xff] → 1, else 0. Does not load
 *       FcConfig or call fontconfig.
 *   uint32_t __gj_fc_config_gate_u_14021  (alias)
 *   __libcgj_batch14021_marker = "libcgj-batch14021"
 *
 * Exclusive continuum CREATE-ONLY (14021-14030: fontconfig/IM soft —
 * fc_config_gate_u_14021, fc_pattern_gate_u_14022, fc_size_gate_u_14023,
 * fcitx_gate_u_14024, ibus_gate_u_14025, im_context_gate_u_14026,
 * xim_gate_u_14027, fc_charset_gate_u_14028, fc_im_soft_ready_u_14029,
 * batch_id_14030). Unique surface only; no multi-def. No parent wires.
 * No __int128. No fontconfig / fcitx / ibus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14021_marker[] = "libcgj-batch14021";

/* Soft FcConfig flag band: low 8 bits only. */
#define B14021_CFG_MASK  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14021_gate(uint32_t uCfgFlags)
{
	if ((uCfgFlags & ~B14021_CFG_MASK) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_config_gate_u_14021 - soft FcConfig flags gate.
 *
 * cfg_flags: soft config flag word (only low 8 bits permitted).
 * Returns 1 when soft gate passes, else 0. Does not load FcConfig or
 * call fontconfig/libc. No parent wires.
 */
uint32_t
gj_fc_config_gate_u_14021(uint32_t cfg_flags)
{
	(void)NULL;
	return b14021_gate(cfg_flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_config_gate_u_14021(uint32_t cfg_flags)
    __attribute__((alias("gj_fc_config_gate_u_14021")));
