/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1750: cgroup/resource exclusive milestone.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1750(void);
 *     — Returns the compile-time graph batch number for this TU (1750).
 *       Milestone for the cgroup/resource pure-data exclusive series
 *       (batches 1741–1749: weight/shares clamps, mem limit ok,
 *       io/pids clamps, percent/basis shares, quota period, throttle
 *       milli-ratio).
 *   uint32_t __gj_batch_id_1750  (alias)
 *   __libcgj_batch1750_marker = "libcgj-batch1750"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1750_marker[] = "libcgj-batch1750";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1750_id(void)
{
	return 1750u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1750 — report this TU's graph batch number.
 *
 * Always returns 1750. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1750(void)
{
	return b1750_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1750(void)
    __attribute__((alias("gj_batch_id_1750")));
