/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1309: Clamp job priority into the fixed class range [0, 31].
 *
 * Surface (unique symbols):
 *   uint32_t gj_job_priority_clamp(uint32_t prio);
 *     — Clamp prio into [0, 31] (32 priority classes).
 *   uint32_t __gj_job_priority_clamp  (alias)
 *   __libcgj_batch1309_marker = "libcgj-batch1309"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stdint.h>

const char __libcgj_batch1309_marker[] = "libcgj-batch1309";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_priority_clamp — confine a job priority to 0 .. 31 inclusive.
 *
 * Values above 31 saturate to 31. Lower bound is already 0 for uint32_t.
 */
uint32_t
gj_job_priority_clamp(uint32_t uPrio)
{
	if (uPrio > 31u) {
		return 31u;
	}
	return uPrio;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_job_priority_clamp(uint32_t uPrio)
    __attribute__((alias("gj_job_priority_clamp")));

