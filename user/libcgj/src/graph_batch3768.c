/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3768: residual capacity primitive (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_flow_cap_rem_u(uint64_t cap, uint64_t flow);
 *     - Residual capacity of a directed edge given capacity and current
 *       forward flow: returns (cap - flow) when cap >= flow, else 0
 *       (clamps underflow / over-flow to zero residual). Pure integer;
 *       no mutation.
 *   uint64_t __gj_flow_cap_rem_u  (alias)
 *   __libcgj_batch3768_marker = "libcgj-batch3768"
 *
 * Exclusive continuum CREATE-ONLY (3761-3770). Distinct from
 * gj_maxflow_ek (batch321) full Edmonds-Karp — unique residual-cap _u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3768_marker[] = "libcgj-batch3768";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flow_cap_rem_u — residual capacity cap − flow (clamped at 0).
 *
 * cap:  edge capacity
 * flow: current flow on the edge (forward)
 *
 * Returns residual residual >= 0. No parent wires.
 */
uint64_t
gj_flow_cap_rem_u(uint64_t cap, uint64_t flow)
{
	(void)NULL;

	if (flow >= cap) {
		return 0u;
	}
	return cap - flow;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_flow_cap_rem_u(uint64_t cap, uint64_t flow)
    __attribute__((alias("gj_flow_cap_rem_u")));
