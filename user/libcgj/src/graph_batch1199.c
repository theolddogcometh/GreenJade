/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1199: circuit-breaker reset (clear failure count).
 *
 * Surface (unique symbols):
 *   unsigned gj_circuit_reset(void);
 *     — Return 0 (closed / healthy failure count). Pure reset value;
 *       caller assigns into their failure counter.
 *   unsigned __gj_circuit_reset  (alias)
 *   __libcgj_batch1199_marker = "libcgj-batch1199"
 *
 * Pairs with trip/open_p (batch1197/1198); unique name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1199_marker[] = "libcgj-batch1199";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_circuit_reset — return zeroed failure count (closed circuit).
 */
unsigned
gj_circuit_reset(void)
{
	return 0u;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_circuit_reset(void)
    __attribute__((alias("gj_circuit_reset")));
