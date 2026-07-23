/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14686: bar3 gpu checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_gpu_check_u_14686(void);
 *     - Returns 0 (soft stub: gpu checklist slot is soft/unprobed;
 *       not a runtime install-path probe).
 *   uint32_t __gj_bar3_gpu_check_u_14686  (alias)
 *   __libcgj_batch14686_marker = "libcgj-batch14686"
 *
 * Exclusive continuum CREATE-ONLY (14681-14690: bar3 checklist soft
 * stubs — check slots all→0; checklist_soft→0; batch_id→14690). Unique surface
 * only; no multi-def. Distinct from gj_bar3_*_ready_u_* readiness soft stubs and
 * milestone bar3 lamps. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14686_marker[] = "libcgj-batch14686";

/* Bar3 checklist soft lamp: always off. */
#define B14686_BAR3_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14686_check(void)
{
	return B14686_BAR3_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_gpu_check_u_14686 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bar3_gpu_check_u_14686(void)
{
	(void)NULL;
	return b14686_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_gpu_check_u_14686(void)
    __attribute__((alias("gj_bar3_gpu_check_u_14686")));
