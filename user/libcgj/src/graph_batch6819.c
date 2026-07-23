/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6819: memfd F_SEAL_* catalog range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mfd_seal_ok_6819(uint32_t seals);
 *     - Returns 1 when seals has no bits outside the soft classic
 *       F_SEAL catalog mask 0x001f (SEAL|SHRINK|GROW|WRITE|FUTURE_WRITE),
 *       else 0. Pure-data mask stub; not a live F_GET_SEALS validation.
 *   uint32_t __gj_mfd_seal_ok_6819  (alias)
 *   __libcgj_batch6819_marker = "libcgj-batch6819"
 *
 * Exclusive continuum CREATE-ONLY (6811-6820: memfd seal stubs —
 * cloexec_6811, allow_sealing_6812, hugetlb_6813, fseal_seal_6814,
 * fseal_shrink_6815, fseal_grow_6816, fseal_write_6817,
 * fseal_future_write_6818, seal_ok_6819, continuum + batch_id_6820).
 * Unique surface only; no multi-def. Soft mask covers the five seal
 * bits stubbed in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6819_marker[] = "libcgj-batch6819";

/*
 * Soft classic F_SEAL_* catalog mask:
 *   SEAL|SHRINK|GROW|WRITE|FUTURE_WRITE = 0x001f
 */
#define B6819_FSEAL_MASK  0x001fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6819_seal_ok(uint32_t uSeals)
{
	return ((uSeals & ~B6819_FSEAL_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mfd_seal_ok_6819 - soft F_SEAL_* catalog membership.
 *
 * seals: candidate seal bitmask (e.g. from F_GET_SEALS).
 * Returns 1 if seals has no bits outside 0x001f, else 0. Does not
 * query the kernel. No parent wires.
 */
uint32_t
gj_mfd_seal_ok_6819(uint32_t seals)
{
	(void)NULL;
	return b6819_seal_ok(seals);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mfd_seal_ok_6819(uint32_t seals)
    __attribute__((alias("gj_mfd_seal_ok_6819")));
