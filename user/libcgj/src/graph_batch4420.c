/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4420: batch identity for gray-code continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4420(void);
 *     - Returns the compile-time graph batch number for this TU (4420).
 *   uint32_t __gj_batch_id_4420  (alias)
 *   __libcgj_batch4420_marker = "libcgj-batch4420"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique —
 * u32/u64 to/from gray, gray next/prev (u2 renames), gray distance,
 * bin/onehot, batch_id_4420). Does NOT redefine prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4420_marker[] = "libcgj-batch4420";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4420_id(void)
{
	return 4420u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4420 - report this TU's graph batch number.
 *
 * Always returns 4420.
 */
uint32_t
gj_batch_id_4420(void)
{
	(void)NULL;
	return b4420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4420(void)
    __attribute__((alias("gj_batch_id_4420")));
