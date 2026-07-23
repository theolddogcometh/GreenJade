/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8530: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8530(void);
 *     - Returns the compile-time graph batch number for this TU (8530).
 *   uint32_t __gj_batch_id_8530  (alias)
 *   __libcgj_batch8530_marker = "libcgj-batch8530"
 *
 * Exclusive continuum CREATE-ONLY (8521-8530: atomic op id stubs —
 * atomic_load_id, atomic_store_id, atomic_add_u, atomic_sub_u,
 * atomic_or_u, atomic_and_u, atomic_xor_u, atomic_cas_ok_u,
 * atomic_fence_id, batch_id_8530). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8530_marker[] = "libcgj-batch8530";

#define B8530_BATCH_ID  8530u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8530_id(void)
{
	return B8530_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8530 - report this TU's graph batch number.
 *
 * Always returns 8530. Soft pure-data product tag that the atomic op
 * id stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_batch_id_8530(void)
{
	(void)NULL;
	return b8530_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8530(void)
    __attribute__((alias("gj_batch_id_8530")));
