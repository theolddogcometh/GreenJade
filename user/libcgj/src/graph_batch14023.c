/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14023: fontconfig pixel-size soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_size_gate_u_14023(uint32_t px_size);
 *     - Soft gate: px_size in [1, 512] → 1, else 0. Does not set
 *       FcPattern size or call fontconfig.
 *   uint32_t __gj_fc_size_gate_u_14023  (alias)
 *   __libcgj_batch14023_marker = "libcgj-batch14023"
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

const char __libcgj_batch14023_marker[] = "libcgj-batch14023";

#define B14023_SIZE_MIN  1u
#define B14023_SIZE_MAX  512u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14023_gate(uint32_t uPxSize)
{
	if (uPxSize < B14023_SIZE_MIN || uPxSize > B14023_SIZE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_size_gate_u_14023 - soft font pixel-size gate.
 *
 * px_size: soft pixel size (1..512).
 * Returns 1 when soft gate passes, else 0. Does not touch FcPattern
 * size/pixel_size or call fontconfig/libc. No parent wires.
 */
uint32_t
gj_fc_size_gate_u_14023(uint32_t px_size)
{
	(void)NULL;
	return b14023_gate(px_size);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_size_gate_u_14023(uint32_t px_size)
    __attribute__((alias("gj_fc_size_gate_u_14023")));
