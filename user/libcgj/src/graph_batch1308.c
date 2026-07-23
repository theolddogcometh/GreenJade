/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1308: MPMC slot state constant: ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mpmc_slot_state_ready(void);
 *     — Always returns 1 (slot published / ready to consume).
 *   uint32_t __gj_mpmc_slot_state_ready  (alias)
 *   __libcgj_batch1308_marker = "libcgj-batch1308"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stdint.h>

const char __libcgj_batch1308_marker[] = "libcgj-batch1308";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mpmc_slot_state_ready — MPMC ring slot ready state tag.
 *
 * Always returns 1.
 */
uint32_t
gj_mpmc_slot_state_ready(void)
{
	return 1u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mpmc_slot_state_ready(void)
    __attribute__((alias("gj_mpmc_slot_state_ready")));

