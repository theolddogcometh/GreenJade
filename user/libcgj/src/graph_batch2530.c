/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2530: milestone batch identity (ELF wave end).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2530(void);
 *     - Returns the compile-time graph batch number for this TU (2530).
 *   uint32_t __gj_batch_id_2530  (alias)
 *   __libcgj_batch2530_marker = "libcgj-batch2530"
 *
 * Milestone for the exclusive ELF helpers wave (batches 2521-2529:
 * magic_ok_u, class_is_64/32_u, data_is_le/be_u, type_exec/dyn/rel_u,
 * machine_x86_64_u). Does NOT redefine gj_batch_id / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2530_marker[] = "libcgj-batch2530";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2530_id(void)
{
	return 2530u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2530 - report this TU's graph batch number.
 *
 * Always returns 2530. Soft compile-time graph revision tag for the
 * ELF exclusive wave close-out.
 */
uint32_t
gj_batch_id_2530(void)
{
	(void)NULL;
	return b2530_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2530(void)
    __attribute__((alias("gj_batch_id_2530")));
