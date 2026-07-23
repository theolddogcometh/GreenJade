/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14440: exclusive hda mixer soft names batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14440(void);
 *     - Returns the compile-time graph batch number for this TU (14440).
 *   uint32_t __gj_batch_id_14440  (alias)
 *   __libcgj_batch14440_marker = "libcgj-batch14440"
 *
 * MILESTONE 14440 for the exclusive continuum CREATE-ONLY wave
 * (batches 14431-14439: hda mixer soft name tags + soft_ready). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14340, gj_batch_id_14430, and sibling
 * batch_id soft stubs in this wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14440_marker[] = "libcgj-batch14440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14440_id(void)
{
	return 14440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14440 - report this TU's graph batch number.
 *
 * Always returns 14440.
 */
uint32_t
gj_batch_id_14440(void)
{
	(void)NULL;
	return b14440_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14440(void)
    __attribute__((alias("gj_batch_id_14440")));
