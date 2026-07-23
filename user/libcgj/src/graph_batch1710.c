/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1710: milestone 1710 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1710(void);
 *     — Returns the compile-time graph batch number for this TU (1710).
 *   uint32_t __gj_batch_id_1710  (alias)
 *   __libcgj_batch1710_marker = "libcgj-batch1710"
 *
 * Milestone for the entropy/hash stream exclusive wave
 * (batches 1701–1709). Does NOT redefine gj_batch_id / prior
 * gj_batch_id_N symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1710_marker[] = "libcgj-batch1710";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1710_id(void)
{
	return 1710u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1710 — report this TU's graph batch number.
 *
 * Always returns 1710.
 */
uint32_t
gj_batch_id_1710(void)
{
	return b1710_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1710(void)
    __attribute__((alias("gj_batch_id_1710")));
