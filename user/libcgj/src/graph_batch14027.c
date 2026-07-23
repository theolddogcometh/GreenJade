/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14027: XIM style soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xim_gate_u_14027(uint32_t style);
 *     - Soft gate: style is one of {1, 2, 4} (preedit/status style
 *       bits soft band) → 1, else 0. Does not open XIM.
 *   uint32_t __gj_xim_gate_u_14027  (alias)
 *   __libcgj_batch14027_marker = "libcgj-batch14027"
 *
 * Exclusive continuum CREATE-ONLY (14021-14030: fontconfig/IM soft —
 * fc_config_gate_u_14021, fc_pattern_gate_u_14022, fc_size_gate_u_14023,
 * fcitx_gate_u_14024, ibus_gate_u_14025, im_context_gate_u_14026,
 * xim_gate_u_14027, fc_charset_gate_u_14028, fc_im_soft_ready_u_14029,
 * batch_id_14030). Unique surface only; no multi-def. No parent wires.
 * No __int128. No XIM / libX11 implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14027_marker[] = "libcgj-batch14027";

/* Soft XIM style singles: preedit/status-shaped bit values. */
#define B14027_STYLE_A  1u
#define B14027_STYLE_B  2u
#define B14027_STYLE_C  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14027_gate(uint32_t uStyle)
{
	if (uStyle == B14027_STYLE_A || uStyle == B14027_STYLE_B ||
	    uStyle == B14027_STYLE_C) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xim_gate_u_14027 - soft XIM style gate.
 *
 * style: soft XIM-shaped style bit (1, 2, or 4 accepted).
 * Returns 1 when soft gate passes, else 0. Does not open XIM or call
 * libc. No parent wires.
 */
uint32_t
gj_xim_gate_u_14027(uint32_t style)
{
	(void)NULL;
	return b14027_gate(style);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xim_gate_u_14027(uint32_t style)
    __attribute__((alias("gj_xim_gate_u_14027")));
