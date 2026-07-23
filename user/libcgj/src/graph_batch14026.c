/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14026: IM context flags soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_im_context_gate_u_14026(uint32_t ctx_flags);
 *     - Soft gate: ctx_flags low 4 bits only (mask 0xf) → 1, else 0.
 *       IM-context soft flag gate; does not open GTK/Qt IM modules.
 *   uint32_t __gj_im_context_gate_u_14026  (alias)
 *   __libcgj_batch14026_marker = "libcgj-batch14026"
 *
 * Exclusive continuum CREATE-ONLY (14021-14030: fontconfig/IM soft —
 * fc_config_gate_u_14021, fc_pattern_gate_u_14022, fc_size_gate_u_14023,
 * fcitx_gate_u_14024, ibus_gate_u_14025, im_context_gate_u_14026,
 * xim_gate_u_14027, fc_charset_gate_u_14028, fc_im_soft_ready_u_14029,
 * batch_id_14030). Unique surface only; no multi-def. Distinct from
 * gj_im_module_ok_u_10135. No parent wires. No __int128. No IM
 * framework implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14026_marker[] = "libcgj-batch14026";

/* Soft IM context flag band: low 4 bits only. */
#define B14026_CTX_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14026_gate(uint32_t uCtxFlags)
{
	if ((uCtxFlags & ~B14026_CTX_MASK) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_im_context_gate_u_14026 - soft IM context flags gate.
 *
 * ctx_flags: soft context flags (only low 4 bits permitted).
 * Returns 1 when soft gate passes, else 0. Does not load IM modules
 * or call libc. No parent wires.
 */
uint32_t
gj_im_context_gate_u_14026(uint32_t ctx_flags)
{
	(void)NULL;
	return b14026_gate(ctx_flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_im_context_gate_u_14026(uint32_t ctx_flags)
    __attribute__((alias("gj_im_context_gate_u_14026")));
