/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6135: PipeWire node-state normalize stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_node_state_norm_6135(uint32_t state);
 *     - Normalize a soft node-state code into a closed set:
 *         0 = error, 1 = creating, 2 = suspended, 3 = idle, 4 = running
 *       Values above 4 clamp to 0 (error). Soft pure-data only.
 *   uint32_t __gj_pw_node_state_norm_6135  (alias)
 *   __libcgj_batch6135_marker = "libcgj-batch6135"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_node_state_norm_6135 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6135_marker[] = "libcgj-batch6135";

#define B6135_STATE_MAX  4u /* running */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6135_node_state_norm(uint32_t uState)
{
	if (uState > B6135_STATE_MAX) {
		return 0u;
	}
	return uState;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_node_state_norm_6135 - normalize PipeWire-shaped node state.
 *
 * state: soft state code (0..4 valid; others → 0 error).
 * Returns normalized state in [0, 4]. Soft pure-data stub.
 * No parent wires.
 */
uint32_t
gj_pw_node_state_norm_6135(uint32_t uState)
{
	(void)NULL;
	return b6135_node_state_norm(uState);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_node_state_norm_6135(uint32_t uState)
    __attribute__((alias("gj_pw_node_state_norm_6135")));
