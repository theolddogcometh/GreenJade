/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8360: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8360(void);
 *     - Returns the compile-time graph batch number for this TU (8360).
 *   uint32_t __gj_batch_id_8360  (alias)
 *   __libcgj_batch8360_marker = "libcgj-batch8360"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8360_marker[] = "libcgj-batch8360";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8360_id(void)
{
	return 8360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8360 - report this TU's graph batch number.
 *
 * Always returns 8360. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8360(void)
{
	(void)NULL;
	return b8360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8360(void)
    __attribute__((alias("gj_batch_id_8360")));
