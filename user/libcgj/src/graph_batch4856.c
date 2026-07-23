/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4856: workgroup axis edge predicate.
 *
 * Surface (unique symbols):
 *   int gj_wg_is_edge_u(uint32_t x, uint32_t lx);
 *     - 1 when local id x sits on the axis edge of a workgroup of
 *       extent lx (x == 0 or x == lx - 1). lx == 0 → 1 (empty/edge).
 *       lx == 1 → 1 for x == 0 only as the sole lane.
 *   int __gj_wg_is_edge_u  (alias)
 *   __libcgj_batch4856_marker = "libcgj-batch4856"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_is_edge_u
 * surface only; no multi-def. Halo / boundary lane probe for GPU-ish
 * workgroup helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4856_marker[] = "libcgj-batch4856";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4856_is_edge(uint32_t u32X, uint32_t u32Lx)
{
	if (u32Lx == 0u) {
		return 1;
	}
	if (u32X == 0u) {
		return 1;
	}
	if (u32X == (u32Lx - 1u)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_is_edge_u - test whether local id x is on a workgroup edge.
 *
 * x:  local coordinate along one axis
 * lx: workgroup extent along that axis
 *
 * Returns 1 on edge (including empty extent lx == 0), else 0.
 * Self-contained; no parent wires.
 */
int
gj_wg_is_edge_u(uint32_t u32X, uint32_t u32Lx)
{
	(void)NULL;
	return b4856_is_edge(u32X, u32Lx);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wg_is_edge_u(uint32_t u32X, uint32_t u32Lx)
    __attribute__((alias("gj_wg_is_edge_u")));
