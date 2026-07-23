/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6816: fcntl F_SEAL_GROW seal stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fseal_grow_6816(void);
 *     - Returns 0x0004 (soft F_SEAL_GROW bit). Pure-data catalog stub;
 *       not a live fcntl(F_ADD_SEALS) call.
 *   uint32_t __gj_fseal_grow_6816  (alias)
 *   __libcgj_batch6816_marker = "libcgj-batch6816"
 *
 * Exclusive continuum CREATE-ONLY (6811-6820: memfd seal stubs —
 * cloexec_6811, allow_sealing_6812, hugetlb_6813, fseal_seal_6814,
 * fseal_shrink_6815, fseal_grow_6816, fseal_write_6817,
 * fseal_future_write_6818, seal_ok_6819, continuum + batch_id_6820).
 * Unique surface only; no multi-def. Distinct from fseal_shrink
 * (batch6815) and fseal_write (batch6817). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6816_marker[] = "libcgj-batch6816";

/* Soft F_SEAL_GROW (prevent file from growing). */
#define B6816_F_SEAL_GROW  0x0004u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6816_fseal_grow(void)
{
	return B6816_F_SEAL_GROW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fseal_grow_6816 - soft F_SEAL_GROW bit catalog value.
 *
 * Always returns 0x0004. Pure-data stub for product memfd seal catalogs;
 * does not invoke fcntl. No parent wires.
 */
uint32_t
gj_fseal_grow_6816(void)
{
	(void)NULL;
	return b6816_fseal_grow();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fseal_grow_6816(void)
    __attribute__((alias("gj_fseal_grow_6816")));
