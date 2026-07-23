/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6957: mallocate M_PERTURB flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_perturb_id_6957(void);
 *     - Returns the soft mallopt param id for M_PERTURB (-6). Catalog
 *       constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_perturb_id_6957  (alias)
 *   __libcgj_batch6957_marker = "libcgj-batch6957"
 *
 * Mallocate flags stubs exclusive CREATE-ONLY wave (6951-6960). Unique
 * batch-suffixed surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6957_marker[] = "libcgj-batch6957";

/* Soft mallopt param id: M_PERTURB (glibc-shaped). */
#define B6957_M_PERTURB  (-6)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6957_perturb_id(void)
{
	return B6957_M_PERTURB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_perturb_id_6957 - soft M_PERTURB mallopt param id.
 *
 * Always returns -6. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_perturb_id_6957(void)
{
	(void)NULL;
	return b6957_perturb_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_perturb_id_6957(void)
    __attribute__((alias("gj_mallocate_perturb_id_6957")));
