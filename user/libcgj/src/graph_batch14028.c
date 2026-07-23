/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14028: fontconfig charset soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_charset_gate_u_14028(uint32_t codepoint);
 *     - Soft gate: codepoint in [0x20, 0x10ffff] → 1, else 0. Soft
 *       Unicode plane band only; does not build FcCharSet.
 *   uint32_t __gj_fc_charset_gate_u_14028  (alias)
 *   __libcgj_batch14028_marker = "libcgj-batch14028"
 *
 * Exclusive continuum CREATE-ONLY (14021-14030: fontconfig/IM soft —
 * fc_config_gate_u_14021, fc_pattern_gate_u_14022, fc_size_gate_u_14023,
 * fcitx_gate_u_14024, ibus_gate_u_14025, im_context_gate_u_14026,
 * xim_gate_u_14027, fc_charset_gate_u_14028, fc_im_soft_ready_u_14029,
 * batch_id_14030). Unique surface only; no multi-def. No parent wires.
 * No __int128. No fontconfig / FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14028_marker[] = "libcgj-batch14028";

/* Soft printable/Unicode plane band (space .. max Unicode). */
#define B14028_CP_MIN  0x20u
#define B14028_CP_MAX  0x10ffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14028_gate(uint32_t uCodepoint)
{
	if (uCodepoint < B14028_CP_MIN || uCodepoint > B14028_CP_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_charset_gate_u_14028 - soft FcCharSet codepoint gate.
 *
 * codepoint: soft Unicode scalar value (0x20..0x10ffff).
 * Returns 1 when soft gate passes, else 0. Does not build FcCharSet
 * or call fontconfig/libc. No parent wires.
 */
uint32_t
gj_fc_charset_gate_u_14028(uint32_t codepoint)
{
	(void)NULL;
	return b14028_gate(codepoint);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_charset_gate_u_14028(uint32_t codepoint)
    __attribute__((alias("gj_fc_charset_gate_u_14028")));
