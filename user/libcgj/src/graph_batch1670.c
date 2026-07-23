/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1670: PID/process-math milestone marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1670(void);
 *     — Returns the compile-time graph batch number for this TU (1670).
 *       Milestone for the PID/process pure-data exclusive series
 *       (batches 1661–1669: pid/tid hash, state char, nice/oom clamp,
 *       rlimit soft<=hard, fd/uid/gid predicates).
 *   uint32_t __gj_batch_id_1670  (alias)
 *   __libcgj_batch1670_marker = "libcgj-batch1670"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1670_marker[] = "libcgj-batch1670";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1670 — report this TU's graph batch number.
 *
 * Always returns 1670. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1670(void)
{
	return 1670u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1670(void)
    __attribute__((alias("gj_batch_id_1670")));
