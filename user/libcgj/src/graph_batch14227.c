/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14227: XIM style soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xim_gate_u_14227(uint32_t style);
 *     - Soft gate: style is one of {1, 2, 4} (preedit/status style
 *       bits soft band) → 1, else 0. Does not open XIM.
 *   uint32_t __gj_xim_gate_u_14227  (alias)
 *   __libcgj_batch14227_marker = "libcgj-batch14227"
 *
 * Exclusive continuum CREATE-ONLY (14221-14230: fontconfig/IM soft —
 * fc_config_gate_u_14221, fc_pattern_gate_u_14222, fc_size_gate_u_14223,
 * fcitx_gate_u_14224, ibus_gate_u_14225, im_context_gate_u_14226,
 * xim_gate_u_14227, fc_charset_gate_u_14228, fc_im_soft_ready_u_14229,
 * batch_id_14230). Unique surface only; no multi-def. Distinct from
 * gj_xim_gate_u_14027 (batch14027). No parent wires. No __int128. No
 * XIM / libX11 implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14227_marker[] = "libcgj-batch14227";

/* Soft XIM style singles: preedit/status-shaped bit values. */
#define B14227_STYLE_A  1u
#define B14227_STYLE_B  2u
#define B14227_STYLE_C  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14227_gate(uint32_t uStyle)
{
	if (uStyle == B14227_STYLE_A || uStyle == B14227_STYLE_B ||
	    uStyle == B14227_STYLE_C) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xim_gate_u_14227 - soft XIM style gate.
 *
 * style: soft XIM-shaped style bit (1, 2, or 4 accepted).
 * Returns 1 when soft gate passes, else 0. Does not open XIM or call
 * libc. No parent wires.
 */
uint32_t
gj_xim_gate_u_14227(uint32_t style)
{
	(void)NULL;
	return b14227_gate(style);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xim_gate_u_14227(uint32_t style)
    __attribute__((alias("gj_xim_gate_u_14227")));
