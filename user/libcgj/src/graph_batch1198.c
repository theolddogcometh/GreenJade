/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1198: circuit-breaker trip (increment failure count).
 *
 * Surface (unique symbols):
 *   unsigned gj_circuit_trip(unsigned failures);
 *     — Return failures+1 with saturation at UINT_MAX (wrap-safe).
 *       Pure function: does not store state.
 *   unsigned __gj_circuit_trip  (alias)
 *   __libcgj_batch1198_marker = "libcgj-batch1198"
 *
 * Pairs with gj_circuit_open_p (batch1197); unique name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1198_marker[] = "libcgj-batch1198";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_circuit_trip — saturating failure-count increment.
 */
unsigned
gj_circuit_trip(unsigned uFailures)
{
	if (uFailures == ~0u) {
		return uFailures;
	}
	return uFailures + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_circuit_trip(unsigned uFailures)
    __attribute__((alias("gj_circuit_trip")));
