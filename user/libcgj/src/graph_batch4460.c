/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4460: continuum batch identity for wave 4451-4460.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4460(void);
 *     - Returns the compile-time graph batch number for this TU (4460).
 *   uint32_t __gj_batch_id_4460  (alias)
 *   __libcgj_batch4460_marker = "libcgj-batch4460"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique —
 * test/set/clear/toggle/count/any/all/none/first_set byte surfaces plus
 * this batch id). Does NOT redefine gj_batch_id or prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4460_marker[] = "libcgj-batch4460";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4460_id(void)
{
	return 4460u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4460 - report this TU's graph batch number.
 *
 * Always returns 4460. Link-time presence tags the 4451-4460 wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_4460(void)
{
	(void)NULL;
	return b4460_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4460(void)
    __attribute__((alias("gj_batch_id_4460")));
