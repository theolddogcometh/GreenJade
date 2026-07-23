/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6811: memfd MFD_CLOEXEC flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mfd_cloexec_6811(void);
 *     - Returns 0x0001 (soft MFD_CLOEXEC memfd_create flag). Pure-data
 *       catalog stub; not a live memfd_create(2) call.
 *   uint32_t __gj_mfd_cloexec_6811  (alias)
 *   __libcgj_batch6811_marker = "libcgj-batch6811"
 *
 * Exclusive continuum CREATE-ONLY (6811-6820: memfd seal stubs —
 * cloexec_6811, allow_sealing_6812, hugetlb_6813, fseal_seal_6814,
 * fseal_shrink_6815, fseal_grow_6816, fseal_write_6817,
 * fseal_future_write_6818, seal_ok_6819, continuum + batch_id_6820).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6811_marker[] = "libcgj-batch6811";

/* Soft MFD_CLOEXEC (close-on-exec for the new memfd). */
#define B6811_MFD_CLOEXEC  0x0001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6811_cloexec(void)
{
	return B6811_MFD_CLOEXEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mfd_cloexec_6811 - soft MFD_CLOEXEC flag catalog value.
 *
 * Always returns 0x0001. Pure-data stub for product memfd flag catalogs;
 * does not invoke memfd_create. No parent wires.
 */
uint32_t
gj_mfd_cloexec_6811(void)
{
	(void)NULL;
	return b6811_cloexec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mfd_cloexec_6811(void)
    __attribute__((alias("gj_mfd_cloexec_6811")));
