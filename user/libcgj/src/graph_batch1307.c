/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1307: MPMC slot state constant: empty.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mpmc_slot_state_empty(void);
 *     — Always returns 0 (slot empty / unclaimed).
 *   uint32_t __gj_mpmc_slot_state_empty  (alias)
 *   __libcgj_batch1307_marker = "libcgj-batch1307"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stdint.h>

const char __libcgj_batch1307_marker[] = "libcgj-batch1307";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mpmc_slot_state_empty — MPMC ring slot empty state tag.
 *
 * Always returns 0.
 */
uint32_t
gj_mpmc_slot_state_empty(void)
{
	return 0u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mpmc_slot_state_empty(void)
    __attribute__((alias("gj_mpmc_slot_state_empty")));

