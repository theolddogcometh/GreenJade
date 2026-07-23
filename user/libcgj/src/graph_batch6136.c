/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6136: PipeWire quantum clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_quantum_clamp_6136(uint32_t quantum);
 *     - Clamp a soft graph quantum (period size in frames) into the
 *       conventional stub range [16, 8192]. Values below 16 become 16;
 *       values above 8192 become 8192.
 *   uint32_t __gj_pw_quantum_clamp_6136  (alias)
 *   __libcgj_batch6136_marker = "libcgj-batch6136"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_quantum_clamp_6136 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6136_marker[] = "libcgj-batch6136";

#define B6136_QUANTUM_MIN  16u
#define B6136_QUANTUM_MAX  8192u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6136_quantum_clamp(uint32_t uQ)
{
	if (uQ < B6136_QUANTUM_MIN) {
		return B6136_QUANTUM_MIN;
	}
	if (uQ > B6136_QUANTUM_MAX) {
		return B6136_QUANTUM_MAX;
	}
	return uQ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_quantum_clamp_6136 - clamp PipeWire-shaped graph quantum.
 *
 * quantum: soft period size in frames.
 * Returns quantum clamped to [16, 8192]. Soft pure-data stub.
 * No parent wires.
 */
uint32_t
gj_pw_quantum_clamp_6136(uint32_t uQuantum)
{
	(void)NULL;
	return b6136_quantum_clamp(uQuantum);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_quantum_clamp_6136(uint32_t uQuantum)
    __attribute__((alias("gj_pw_quantum_clamp_6136")));
