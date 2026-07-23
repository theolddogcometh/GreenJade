/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14225: ibus engine soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ibus_gate_u_14225(uint32_t engine_id);
 *     - Soft gate: engine_id non-zero and in [1, 0xffff] → 1, else 0.
 *       ibus-shaped soft engine id gate; does not probe ibus daemon.
 *   uint32_t __gj_ibus_gate_u_14225  (alias)
 *   __libcgj_batch14225_marker = "libcgj-batch14225"
 *
 * Exclusive continuum CREATE-ONLY (14221-14230: fontconfig/IM soft —
 * fc_config_gate_u_14221, fc_pattern_gate_u_14222, fc_size_gate_u_14223,
 * fcitx_gate_u_14224, ibus_gate_u_14225, im_context_gate_u_14226,
 * xim_gate_u_14227, fc_charset_gate_u_14228, fc_im_soft_ready_u_14229,
 * batch_id_14230). Unique surface only; no multi-def. Distinct from
 * gj_ibus_gate_u_14025 (batch14025) and gj_ibus_ok_u_10131. No parent
 * wires. No __int128. No ibus implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14225_marker[] = "libcgj-batch14225";

#define B14225_ENGINE_MIN  1u
#define B14225_ENGINE_MAX  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14225_gate(uint32_t uEngineId)
{
	if (uEngineId < B14225_ENGINE_MIN || uEngineId > B14225_ENGINE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ibus_gate_u_14225 - soft ibus engine-id gate.
 *
 * engine_id: soft ibus-shaped engine identifier (1..0xffff).
 * Returns 1 when soft gate passes, else 0. Does not contact ibus
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_ibus_gate_u_14225(uint32_t engine_id)
{
	(void)NULL;
	return b14225_gate(engine_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ibus_gate_u_14225(uint32_t engine_id)
    __attribute__((alias("gj_ibus_gate_u_14225")));
