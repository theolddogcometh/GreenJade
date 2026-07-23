/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14224: fcitx status soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcitx_gate_u_14224(uint32_t status);
 *     - Soft gate: status is 0 or 1 → 1, else 0. fcitx-shaped soft
 *       status lamp only; does not probe fcitx daemon.
 *   uint32_t __gj_fcitx_gate_u_14224  (alias)
 *   __libcgj_batch14224_marker = "libcgj-batch14224"
 *
 * Exclusive continuum CREATE-ONLY (14221-14230: fontconfig/IM soft —
 * fc_config_gate_u_14221, fc_pattern_gate_u_14222, fc_size_gate_u_14223,
 * fcitx_gate_u_14224, ibus_gate_u_14225, im_context_gate_u_14226,
 * xim_gate_u_14227, fc_charset_gate_u_14228, fc_im_soft_ready_u_14229,
 * batch_id_14230). Unique surface only; no multi-def. Distinct from
 * gj_fcitx_gate_u_14024 (batch14024) and gj_fcitx_ok_u_10132. No parent
 * wires. No __int128. No fcitx implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14224_marker[] = "libcgj-batch14224";

/* Soft fcitx status band: boolean 0/1 only. */
#define B14224_STATUS_MAX  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14224_gate(uint32_t uStatus)
{
	if (uStatus > B14224_STATUS_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcitx_gate_u_14224 - soft fcitx status gate.
 *
 * status: soft fcitx-shaped status (0 or 1 accepted).
 * Returns 1 when soft gate passes, else 0. Does not contact fcitx
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_fcitx_gate_u_14224(uint32_t status)
{
	(void)NULL;
	return b14224_gate(status);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcitx_gate_u_14224(uint32_t status)
    __attribute__((alias("gj_fcitx_gate_u_14224")));
