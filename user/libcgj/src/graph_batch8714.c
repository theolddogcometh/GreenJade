/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8714: shm_open ready tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_open_ready_u_8714(void);
 *     - Return 1 (soft shm_open catalog readiness).
 *   uint32_t __gj_shm_open_ready_u_8714  (alias)
 *   __libcgj_batch8714_marker = "libcgj-batch8714"
 *
 * Exclusive continuum CREATE-ONLY (8711-8720: memfd/shm id stubs —
 * memfd_ready_u, memfd_cloexec_id, memfd_allow_seal_id, shm_open_ready_u,
 * shm_unlink_ready_u, shm_name_ok_u, seal_grow_id, seal_shrink_id,
 * seal_write_id, batch_id_8720). Unique gj_shm_open_ready_u_8714 surface
 * only; no multi-def. Soft compile-time readiness tag only. Distinct
 * from shm_open flag stubs 7141-7150. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8714_marker[] = "libcgj-batch8714";

/* Soft shm_open catalog readiness lamp. */
#define B8714_SHM_OPEN_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8714_shm_open_ready(void)
{
	return B8714_SHM_OPEN_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_open_ready_u_8714 - report soft shm_open catalog readiness.
 *
 * Always returns 1 (ready). Link-time presence of this symbol tags the
 * memfd/shm id stub wave. Does not call shm_open. No parent wires.
 */
uint32_t
gj_shm_open_ready_u_8714(void)
{
	(void)NULL;
	return b8714_shm_open_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_open_ready_u_8714(void)
    __attribute__((alias("gj_shm_open_ready_u_8714")));
