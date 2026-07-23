/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1197: circuit-breaker open predicate.
 *
 * Surface (unique symbols):
 *   int gj_circuit_open_p(unsigned failures, unsigned threshold);
 *     — Return 1 if failures >= threshold and threshold > 0
 *       (circuit is open / tripped). threshold==0 → always closed (0).
 *   int __gj_circuit_open_p  (alias)
 *   __libcgj_batch1197_marker = "libcgj-batch1197"
 *
 * Circuit-breaker open test; unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1197_marker[] = "libcgj-batch1197";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_circuit_open_p — 1 if failure count has reached the trip threshold.
 */
int
gj_circuit_open_p(unsigned uFailures, unsigned uThreshold)
{
	if (uThreshold == 0u) {
		return 0;
	}
	return (uFailures >= uThreshold) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_circuit_open_p(unsigned uFailures, unsigned uThreshold)
    __attribute__((alias("gj_circuit_open_p")));
