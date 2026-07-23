/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8713: memfd MFD_ALLOW_SEALING flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memfd_allow_seal_id_8713(void);
 *     - Return soft MFD_ALLOW_SEALING constant (0x0002).
 *   uint32_t __gj_memfd_allow_seal_id_8713  (alias)
 *   __libcgj_batch8713_marker = "libcgj-batch8713"
 *
 * Exclusive continuum CREATE-ONLY (8711-8720: memfd/shm id stubs —
 * memfd_ready_u, memfd_cloexec_id, memfd_allow_seal_id, shm_open_ready_u,
 * shm_unlink_ready_u, shm_name_ok_u, seal_grow_id, seal_shrink_id,
 * seal_write_id, batch_id_8720). Unique gj_memfd_allow_seal_id_8713
 * surface only; no multi-def. Distinct from gj_mfd_allow_sealing_6812
 * (batch6812). Catalog id only; not a live memfd_create(2) call. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8713_marker[] = "libcgj-batch8713";

/* Soft MFD_ALLOW_SEALING (permit F_ADD_SEALS on the memfd). */
#define B8713_MFD_ALLOW_SEALING  ((uint32_t)0x0002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8713_memfd_allow_seal_id(void)
{
	return B8713_MFD_ALLOW_SEALING;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memfd_allow_seal_id_8713 - soft MFD_ALLOW_SEALING flag catalog id.
 *
 * Always returns 0x0002. Catalog id only; does not invoke memfd_create
 * or fcntl. No parent wires.
 */
uint32_t
gj_memfd_allow_seal_id_8713(void)
{
	(void)NULL;
	return b8713_memfd_allow_seal_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memfd_allow_seal_id_8713(void)
    __attribute__((alias("gj_memfd_allow_seal_id_8713")));
