/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14221: fontconfig FcConfig soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_config_gate_u_14221(uint32_t cfg_flags);
 *     - Soft gate: cfg_flags in [0, 0xff] → 1, else 0. Does not load
 *       FcConfig or call fontconfig.
 *   uint32_t __gj_fc_config_gate_u_14221  (alias)
 *   __libcgj_batch14221_marker = "libcgj-batch14221"
 *
 * Exclusive continuum CREATE-ONLY (14221-14230: fontconfig/IM soft —
 * fc_config_gate_u_14221, fc_pattern_gate_u_14222, fc_size_gate_u_14223,
 * fcitx_gate_u_14224, ibus_gate_u_14225, im_context_gate_u_14226,
 * xim_gate_u_14227, fc_charset_gate_u_14228, fc_im_soft_ready_u_14229,
 * batch_id_14230). Unique surface only; no multi-def. Distinct from
 * gj_fc_config_gate_u_14021 (batch14021). No parent wires. No __int128.
 * No fontconfig / fcitx / ibus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14221_marker[] = "libcgj-batch14221";

/* Soft FcConfig flag band: low 8 bits only. */
#define B14221_CFG_MASK  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14221_gate(uint32_t uCfgFlags)
{
	if ((uCfgFlags & ~B14221_CFG_MASK) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_config_gate_u_14221 - soft FcConfig flags gate.
 *
 * cfg_flags: soft config flag word (only low 8 bits permitted).
 * Returns 1 when soft gate passes, else 0. Does not load FcConfig or
 * call fontconfig/libc. No parent wires.
 */
uint32_t
gj_fc_config_gate_u_14221(uint32_t cfg_flags)
{
	(void)NULL;
	return b14221_gate(cfg_flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_config_gate_u_14221(uint32_t cfg_flags)
    __attribute__((alias("gj_fc_config_gate_u_14221")));
