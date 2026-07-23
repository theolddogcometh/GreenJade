/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3769: residual-edge flow push primitive (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_flow_push_u(uint64_t *res_fwd, uint64_t *res_rev,
 *                           uint64_t amount);
 *     - Push up to amount units along a residual edge:
 *         pushed = min(amount, *res_fwd)
 *         *res_fwd -= pushed
 *         *res_rev += pushed   (saturates at UINT64_MAX on reverse)
 *       Used by Ford-Fulkerson / Edmonds-Karp style augment steps on a
 *       residual graph. NULL pointers → returns 0 with no writes.
 *       amount == 0 or *res_fwd == 0 → returns 0, no mutation of reverse
 *       when pushed is 0.
 *     Returns the amount actually pushed.
 *   uint64_t __gj_flow_push_u  (alias)
 *   __libcgj_batch3769_marker = "libcgj-batch3769"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Distinct from
 * gj_maxflow_ek (batch321) full algorithm — unique single-edge push _u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3769_marker[] = "libcgj-batch3769";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturated add: min(a + b, UINT64_MAX). */
static uint64_t
b3769_add_sat(uint64_t uA, uint64_t uB)
{
	if (uB > (uint64_t)UINT64_MAX - uA) {
		return (uint64_t)UINT64_MAX;
	}
	return uA + uB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flow_push_u — push flow on one residual edge (forward/reverse pair).
 *
 * res_fwd: residual capacity forward (decreased)
 * res_rev: residual capacity reverse (increased, saturated)
 * amount:  requested push volume
 *
 * Returns units pushed. No parent wires.
 */
uint64_t
gj_flow_push_u(uint64_t *res_fwd, uint64_t *res_rev, uint64_t amount)
{
	uint64_t uPush;

	(void)NULL;

	if (res_fwd == NULL || res_rev == NULL || amount == 0u) {
		return 0u;
	}

	uPush = *res_fwd;
	if (uPush == 0u) {
		return 0u;
	}
	if (amount < uPush) {
		uPush = amount;
	}

	*res_fwd = *res_fwd - uPush;
	*res_rev = b3769_add_sat(*res_rev, uPush);
	return uPush;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_flow_push_u(uint64_t *res_fwd, uint64_t *res_rev, uint64_t amount)
    __attribute__((alias("gj_flow_push_u")));
