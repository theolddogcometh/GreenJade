/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14460: exclusive pmm order soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14460(void);
 *     - Returns the compile-time graph batch number for this TU (14460).
 *   uint32_t __gj_batch_id_14460  (alias)
 *   __libcgj_batch14460_marker = "libcgj-batch14460"
 *
 * MILESTONE 14460 for the exclusive continuum CREATE-ONLY wave
 * (batches 14451-14459: pmm order soft tags + soft_ready). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14360, gj_batch_id_14450, and sibling
 * batch_id soft stubs in this wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14460_marker[] = "libcgj-batch14460";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14460_id(void)
{
	return 14460u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14460 - report this TU's graph batch number.
 *
 * Always returns 14460.
 */
uint32_t
gj_batch_id_14460(void)
{
	(void)NULL;
	return b14460_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14460(void)
    __attribute__((alias("gj_batch_id_14460")));
