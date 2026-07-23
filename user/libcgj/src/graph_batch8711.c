/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8711: memfd ready tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memfd_ready_u_8711(void);
 *     - Return 1 (soft memfd_create catalog readiness).
 *   uint32_t __gj_memfd_ready_u_8711  (alias)
 *   __libcgj_batch8711_marker = "libcgj-batch8711"
 *
 * Exclusive continuum CREATE-ONLY (8711-8720: memfd/shm id stubs —
 * memfd_ready_u, memfd_cloexec_id, memfd_allow_seal_id, shm_open_ready_u,
 * shm_unlink_ready_u, shm_name_ok_u, seal_grow_id, seal_shrink_id,
 * seal_write_id, batch_id_8720). Unique gj_memfd_ready_u_8711 surface
 * only; no multi-def. Soft compile-time readiness tag only. Distinct
 * from mfd_cloexec_6811 / mfd seal continuum 6811-6820. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8711_marker[] = "libcgj-batch8711";

/* Soft memfd catalog readiness lamp. */
#define B8711_MEMFD_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8711_memfd_ready(void)
{
	return B8711_MEMFD_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memfd_ready_u_8711 - report soft memfd catalog readiness.
 *
 * Always returns 1 (ready). Link-time presence of this symbol tags the
 * memfd/shm id stub wave. Does not call memfd_create. No parent wires.
 */
uint32_t
gj_memfd_ready_u_8711(void)
{
	(void)NULL;
	return b8711_memfd_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memfd_ready_u_8711(void)
    __attribute__((alias("gj_memfd_ready_u_8711")));
