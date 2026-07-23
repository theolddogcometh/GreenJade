/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8712: memfd MFD_CLOEXEC flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memfd_cloexec_id_8712(void);
 *     - Return soft MFD_CLOEXEC constant (0x0001).
 *   uint32_t __gj_memfd_cloexec_id_8712  (alias)
 *   __libcgj_batch8712_marker = "libcgj-batch8712"
 *
 * Exclusive continuum CREATE-ONLY (8711-8720: memfd/shm id stubs —
 * memfd_ready_u, memfd_cloexec_id, memfd_allow_seal_id, shm_open_ready_u,
 * shm_unlink_ready_u, shm_name_ok_u, seal_grow_id, seal_shrink_id,
 * seal_write_id, batch_id_8720). Unique gj_memfd_cloexec_id_8712 surface
 * only; no multi-def. Distinct from gj_mfd_cloexec_6811 (batch6811).
 * Catalog id only; not a live memfd_create(2) call. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8712_marker[] = "libcgj-batch8712";

/* Soft MFD_CLOEXEC (close-on-exec for the new memfd). */
#define B8712_MFD_CLOEXEC  ((uint32_t)0x0001u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8712_memfd_cloexec_id(void)
{
	return B8712_MFD_CLOEXEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memfd_cloexec_id_8712 - soft MFD_CLOEXEC flag catalog id.
 *
 * Always returns 0x0001. Catalog id only; does not invoke memfd_create.
 * No parent wires.
 */
uint32_t
gj_memfd_cloexec_id_8712(void)
{
	(void)NULL;
	return b8712_memfd_cloexec_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memfd_cloexec_id_8712(void)
    __attribute__((alias("gj_memfd_cloexec_id_8712")));
