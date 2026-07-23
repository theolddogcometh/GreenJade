/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6951: mallocate M_MXFAST flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_mxfast_id_6951(void);
 *     - Returns the soft mallopt param id for M_MXFAST (1). Catalog
 *       constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_mxfast_id_6951  (alias)
 *   __libcgj_batch6951_marker = "libcgj-batch6951"
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

const char __libcgj_batch6951_marker[] = "libcgj-batch6951";

/* Soft mallopt param id: M_MXFAST (glibc-shaped). */
#define B6951_M_MXFAST  1

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6951_mxfast_id(void)
{
	return B6951_M_MXFAST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_mxfast_id_6951 - soft M_MXFAST mallopt param id.
 *
 * Always returns 1. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_mxfast_id_6951(void)
{
	(void)NULL;
	return b6951_mxfast_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_mxfast_id_6951(void)
    __attribute__((alias("gj_mallocate_mxfast_id_6951")));
