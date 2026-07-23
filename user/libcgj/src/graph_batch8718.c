/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8718: fcntl F_SEAL_SHRINK seal id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seal_shrink_id_8718(void);
 *     - Return soft F_SEAL_SHRINK constant (0x0002).
 *   uint32_t __gj_seal_shrink_id_8718  (alias)
 *   __libcgj_batch8718_marker = "libcgj-batch8718"
 *
 * Exclusive continuum CREATE-ONLY (8711-8720: memfd/shm id stubs —
 * memfd_ready_u, memfd_cloexec_id, memfd_allow_seal_id, shm_open_ready_u,
 * shm_unlink_ready_u, shm_name_ok_u, seal_grow_id, seal_shrink_id,
 * seal_write_id, batch_id_8720). Unique gj_seal_shrink_id_8718 surface
 * only; no multi-def. Distinct from gj_fseal_shrink_6815 (batch6815)
 * and gj_seal_grow_id_8717. Catalog id only; not a live
 * fcntl(F_ADD_SEALS) call. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8718_marker[] = "libcgj-batch8718";

/* Soft F_SEAL_SHRINK (prevent file from shrinking). */
#define B8718_F_SEAL_SHRINK  ((uint32_t)0x0002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8718_seal_shrink_id(void)
{
	return B8718_F_SEAL_SHRINK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seal_shrink_id_8718 - soft F_SEAL_SHRINK bit catalog id.
 *
 * Always returns 0x0002. Catalog id only; does not invoke fcntl. No
 * parent wires.
 */
uint32_t
gj_seal_shrink_id_8718(void)
{
	(void)NULL;
	return b8718_seal_shrink_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seal_shrink_id_8718(void)
    __attribute__((alias("gj_seal_shrink_id_8718")));
