/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7959: qsort stable index tie-break compare stub.
 *
 * Surface (unique symbols):
 *   int gj_qsort_cmp_stable_idx_7959(uint32_t a, uint32_t b,
 *                                    uint32_t ia, uint32_t ib);
 *     - Compare keys a,b ascending; on tie compare indices ia,ib asc.
 *   int __gj_qsort_cmp_stable_idx_7959  (alias)
 *   __libcgj_batch7959_marker = "libcgj-batch7959"
 *
 * Exclusive continuum CREATE-ONLY (7951-7960: qsort compare stubs —
 * u32_asc, u32_desc, i32_asc, i32_desc, eq_u, lt_u, sign_norm,
 * result_ok, stable_idx, continuum + batch_id_7960). Unique
 * gj_qsort_cmp_stable_idx_7959 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7959_marker[] = "libcgj-batch7959";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b7959_stable_idx(uint32_t u32A, uint32_t u32B, uint32_t u32Ia, uint32_t u32Ib)
{
	if (u32A < u32B)
		return -1;
	if (u32A > u32B)
		return 1;
	/* Key tie: original index order for soft stability. */
	if (u32Ia < u32Ib)
		return -1;
	if (u32Ia > u32Ib)
		return 1;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_cmp_stable_idx_7959 - key asc with original-index tie-break.
 *
 * a:  left key
 * b:  right key
 * ia: left original index
 * ib: right original index
 *
 * Returns -1 / 0 / 1. When keys equal, lower index sorts first (soft
 * stable qsort tie-break). Does not call libc. No parent wires.
 */
int
gj_qsort_cmp_stable_idx_7959(uint32_t u32A, uint32_t u32B,
    uint32_t u32Ia, uint32_t u32Ib)
{
	(void)NULL;
	return b7959_stable_idx(u32A, u32B, u32Ia, u32Ib);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_qsort_cmp_stable_idx_7959(uint32_t u32A, uint32_t u32B,
    uint32_t u32Ia, uint32_t u32Ib)
    __attribute__((alias("gj_qsort_cmp_stable_idx_7959")));
