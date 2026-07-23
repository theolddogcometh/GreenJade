/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14024: fcitx status soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcitx_gate_u_14024(uint32_t status);
 *     - Soft gate: status is 0 or 1 → 1, else 0. fcitx-shaped soft
 *       status lamp only; does not probe fcitx daemon.
 *   uint32_t __gj_fcitx_gate_u_14024  (alias)
 *   __libcgj_batch14024_marker = "libcgj-batch14024"
 *
 * Exclusive continuum CREATE-ONLY (14021-14030: fontconfig/IM soft —
 * fc_config_gate_u_14021, fc_pattern_gate_u_14022, fc_size_gate_u_14023,
 * fcitx_gate_u_14024, ibus_gate_u_14025, im_context_gate_u_14026,
 * xim_gate_u_14027, fc_charset_gate_u_14028, fc_im_soft_ready_u_14029,
 * batch_id_14030). Unique surface only; no multi-def. Distinct from
 * gj_fcitx_ok_u_10132. No parent wires. No __int128. No fcitx
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14024_marker[] = "libcgj-batch14024";

/* Soft fcitx status band: boolean 0/1 only. */
#define B14024_STATUS_MAX  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14024_gate(uint32_t uStatus)
{
	if (uStatus > B14024_STATUS_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcitx_gate_u_14024 - soft fcitx status gate.
 *
 * status: soft fcitx-shaped status (0 or 1 accepted).
 * Returns 1 when soft gate passes, else 0. Does not contact fcitx
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_fcitx_gate_u_14024(uint32_t status)
{
	(void)NULL;
	return b14024_gate(status);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcitx_gate_u_14024(uint32_t status)
    __attribute__((alias("gj_fcitx_gate_u_14024")));
