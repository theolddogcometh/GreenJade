/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7348: msync flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ms_sync_flags_pack_u_7348(uint32_t want_async,
 *                                         uint32_t want_sync,
 *                                         uint32_t want_invalidate);
 *     - Pack soft MS_* wants into a flags mask (0x1|0x4|0x2).
 *   uint32_t __gj_ms_sync_flags_pack_u_7348  (alias)
 *   __libcgj_batch7348_marker = "libcgj-batch7348"
 *
 * Exclusive continuum CREATE-ONLY (7341-7350: fadvise sync flags stubs —
 * fadv_normal_id, fadv_random_id, fadv_sequential_id, fadv_willneed_id,
 * fadv_dontneed_id, fadv_advice_ok, o_sync_id, ms_sync_flags_pack,
 * ms_sync_flags_ok, batch_id_7350).
 * Unique gj_ms_sync_flags_pack_u_7348 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7348_marker[] = "libcgj-batch7348";

/* Soft pack bits matching POSIX/Linux msync(2) MS_* layout. */
#define B7348_MS_ASYNC      ((uint32_t)0x1u)
#define B7348_MS_INVALIDATE ((uint32_t)0x2u)
#define B7348_MS_SYNC       ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7348_ms_sync_flags_pack(uint32_t u32WantAsync,
                         uint32_t u32WantSync,
                         uint32_t u32WantInvalidate)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantAsync != 0u)
		u32Pack |= B7348_MS_ASYNC;
	if (u32WantSync != 0u)
		u32Pack |= B7348_MS_SYNC;
	if (u32WantInvalidate != 0u)
		u32Pack |= B7348_MS_INVALIDATE;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_sync_flags_pack_u_7348 - soft-pack msync flag wants.
 *
 * want_async:      non-zero to set MS_ASYNC (0x1)
 * want_sync:       non-zero to set MS_SYNC (0x4)
 * want_invalidate: non-zero to set MS_INVALIDATE (0x2)
 *
 * Returns bitmask of requested MS_* bits. Pure integer pack; does not
 * call msync. No parent wires.
 */
uint32_t
gj_ms_sync_flags_pack_u_7348(uint32_t u32WantAsync,
                             uint32_t u32WantSync,
                             uint32_t u32WantInvalidate)
{
	(void)NULL;
	return b7348_ms_sync_flags_pack(u32WantAsync, u32WantSync,
	                                u32WantInvalidate);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ms_sync_flags_pack_u_7348(uint32_t u32WantAsync,
                                        uint32_t u32WantSync,
                                        uint32_t u32WantInvalidate)
    __attribute__((alias("gj_ms_sync_flags_pack_u_7348")));
