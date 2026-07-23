/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6813: memfd MFD_HUGETLB flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mfd_hugetlb_6813(void);
 *     - Returns 0x0004 (soft MFD_HUGETLB memfd_create flag). Pure-data
 *       catalog stub; not a live memfd_create(2) call.
 *   uint32_t __gj_mfd_hugetlb_6813  (alias)
 *   __libcgj_batch6813_marker = "libcgj-batch6813"
 *
 * Exclusive continuum CREATE-ONLY (6811-6820: memfd seal stubs —
 * cloexec_6811, allow_sealing_6812, hugetlb_6813, fseal_seal_6814,
 * fseal_shrink_6815, fseal_grow_6816, fseal_write_6817,
 * fseal_future_write_6818, seal_ok_6819, continuum + batch_id_6820).
 * Unique surface only; no multi-def. Distinct from allow_sealing
 * (batch6812) and fseal_seal (batch6814). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6813_marker[] = "libcgj-batch6813";

/* Soft MFD_HUGETLB (huge-page backed anonymous memory). */
#define B6813_MFD_HUGETLB  0x0004u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6813_hugetlb(void)
{
	return B6813_MFD_HUGETLB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mfd_hugetlb_6813 - soft MFD_HUGETLB flag catalog value.
 *
 * Always returns 0x0004. Pure-data stub for product memfd flag catalogs;
 * does not invoke memfd_create. No parent wires.
 */
uint32_t
gj_mfd_hugetlb_6813(void)
{
	(void)NULL;
	return b6813_hugetlb();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mfd_hugetlb_6813(void)
    __attribute__((alias("gj_mfd_hugetlb_6813")));
