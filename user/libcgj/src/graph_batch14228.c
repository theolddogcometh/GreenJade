/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14228: fontconfig charset soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_charset_gate_u_14228(uint32_t codepoint);
 *     - Soft gate: codepoint in [0x20, 0x10ffff] → 1, else 0. Soft
 *       Unicode plane band only; does not build FcCharSet.
 *   uint32_t __gj_fc_charset_gate_u_14228  (alias)
 *   __libcgj_batch14228_marker = "libcgj-batch14228"
 *
 * Exclusive continuum CREATE-ONLY (14221-14230: fontconfig/IM soft —
 * fc_config_gate_u_14221, fc_pattern_gate_u_14222, fc_size_gate_u_14223,
 * fcitx_gate_u_14224, ibus_gate_u_14225, im_context_gate_u_14226,
 * xim_gate_u_14227, fc_charset_gate_u_14228, fc_im_soft_ready_u_14229,
 * batch_id_14230). Unique surface only; no multi-def. Distinct from
 * gj_fc_charset_gate_u_14028 (batch14028). No parent wires. No __int128.
 * No fontconfig / FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14228_marker[] = "libcgj-batch14228";

/* Soft printable/Unicode plane band (space .. max Unicode). */
#define B14228_CP_MIN  0x20u
#define B14228_CP_MAX  0x10ffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14228_gate(uint32_t uCodepoint)
{
	if (uCodepoint < B14228_CP_MIN || uCodepoint > B14228_CP_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_charset_gate_u_14228 - soft FcCharSet codepoint gate.
 *
 * codepoint: soft Unicode scalar value (0x20..0x10ffff).
 * Returns 1 when soft gate passes, else 0. Does not build FcCharSet
 * or call fontconfig/libc. No parent wires.
 */
uint32_t
gj_fc_charset_gate_u_14228(uint32_t codepoint)
{
	(void)NULL;
	return b14228_gate(codepoint);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_charset_gate_u_14228(uint32_t codepoint)
    __attribute__((alias("gj_fc_charset_gate_u_14228")));
