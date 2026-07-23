/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6188: cgroup io.max bytes/s clamp stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cg_io_max_bps_clamp_6188(uint64_t bps);
 *     - Clamp an io.max-style bytes-per-second budget into
 *       [1, (1ull << 60)]. Below 1 → 1; above 2^60 → 2^60.
 *       Unlimited/"max" encodings are out of scope for this clamp.
 *   uint64_t __gj_cg_io_max_bps_clamp_6188  (alias)
 *   __libcgj_batch6188_marker = "libcgj-batch6188"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_io_max_bps_clamp_6188 surface only; no multi-def.
 * Pure data — not an io.max writer. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6188_marker[] = "libcgj-batch6188";

#define B6188_BPS_MIN  1ull
#define B6188_BPS_MAX  (1ull << 60)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6188_clamp(uint64_t uBps)
{
	if (uBps < B6188_BPS_MIN) {
		return B6188_BPS_MIN;
	}
	if (uBps > B6188_BPS_MAX) {
		return B6188_BPS_MAX;
	}
	return uBps;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_io_max_bps_clamp_6188 - clamp io.max-style bytes/s budget.
 *
 * Suitable for sanitizing parsed rbps/wbps fields before container
 * runtime IO policy math. No blkcg, no cgroupfs. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_cg_io_max_bps_clamp_6188(uint64_t uBps)
{
	(void)NULL;
	return b6188_clamp(uBps);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cg_io_max_bps_clamp_6188(uint64_t uBps)
    __attribute__((alias("gj_cg_io_max_bps_clamp_6188")));
