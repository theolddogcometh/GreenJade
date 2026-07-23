/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch896: integer online variance init (Welford M2).
 *
 * Distinct from gj_welford_init (batch309, double) and gj_online_mean_init
 * (batch498) — unique symbols. Pair with gj_online_var_add (batch897).
 *
 * Surface (unique symbols):
 *   void gj_online_var_init(uint64_t *n, int64_t *mean, int64_t *m2);
 *     — Reset sample count, mean, and M2 sum-of-squares to empty.
 *   void __gj_online_var_init  (alias)
 *   __libcgj_batch896_marker = "libcgj-batch896"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no libm. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch896_marker[] = "libcgj-batch896";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_online_var_init — reset integer online variance state.
 *
 * n, mean, m2: caller-owned. NULL fields are skipped; remaining non-NULL
 * fields are still cleared. Freestanding: no errno.
 */
void
gj_online_var_init(uint64_t *n, int64_t *mean, int64_t *m2)
{
	if (n != NULL) {
		*n = (uint64_t)0;
	}
	if (mean != NULL) {
		*mean = (int64_t)0;
	}
	if (m2 != NULL) {
		*m2 = (int64_t)0;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_online_var_init(uint64_t *n, int64_t *mean, int64_t *m2)
    __attribute__((alias("gj_online_var_init")));
