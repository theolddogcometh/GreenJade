/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6812: memfd MFD_ALLOW_SEALING flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mfd_allow_sealing_6812(void);
 *     - Returns 0x0002 (soft MFD_ALLOW_SEALING memfd_create flag).
 *       Pure-data catalog stub; not a live memfd_create(2) call.
 *   uint32_t __gj_mfd_allow_sealing_6812  (alias)
 *   __libcgj_batch6812_marker = "libcgj-batch6812"
 *
 * Exclusive continuum CREATE-ONLY (6811-6820: memfd seal stubs —
 * cloexec_6811, allow_sealing_6812, hugetlb_6813, fseal_seal_6814,
 * fseal_shrink_6815, fseal_grow_6816, fseal_write_6817,
 * fseal_future_write_6818, seal_ok_6819, continuum + batch_id_6820).
 * Unique surface only; no multi-def. Distinct from cloexec (batch6811)
 * and hugetlb (batch6813). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6812_marker[] = "libcgj-batch6812";

/* Soft MFD_ALLOW_SEALING (permit F_ADD_SEALS on the memfd). */
#define B6812_MFD_ALLOW_SEALING  0x0002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6812_allow_sealing(void)
{
	return B6812_MFD_ALLOW_SEALING;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mfd_allow_sealing_6812 - soft MFD_ALLOW_SEALING flag catalog value.
 *
 * Always returns 0x0002. Pure-data stub for product memfd flag catalogs;
 * does not invoke memfd_create or fcntl. No parent wires.
 */
uint32_t
gj_mfd_allow_sealing_6812(void)
{
	(void)NULL;
	return b6812_allow_sealing();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mfd_allow_sealing_6812(void)
    __attribute__((alias("gj_mfd_allow_sealing_6812")));
