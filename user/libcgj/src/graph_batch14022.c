/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14022: fontconfig FcPattern soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pattern_gate_u_14022(uint32_t prop_count);
 *     - Soft gate: prop_count in [1, 64] → 1, else 0. Does not build
 *       FcPattern or call fontconfig.
 *   uint32_t __gj_fc_pattern_gate_u_14022  (alias)
 *   __libcgj_batch14022_marker = "libcgj-batch14022"
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

const char __libcgj_batch14022_marker[] = "libcgj-batch14022";

#define B14022_PROP_MIN  1u
#define B14022_PROP_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14022_gate(uint32_t uPropCount)
{
	if (uPropCount < B14022_PROP_MIN || uPropCount > B14022_PROP_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pattern_gate_u_14022 - soft FcPattern property-count gate.
 *
 * prop_count: number of soft pattern properties (1..64).
 * Returns 1 when soft gate passes, else 0. Does not allocate FcPattern
 * or call fontconfig/libc. No parent wires.
 */
uint32_t
gj_fc_pattern_gate_u_14022(uint32_t prop_count)
{
	(void)NULL;
	return b14022_gate(prop_count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pattern_gate_u_14022(uint32_t prop_count)
    __attribute__((alias("gj_fc_pattern_gate_u_14022")));
