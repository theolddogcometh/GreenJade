/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7368: fallocate mode pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_falloc_modes_pack_u_7368(uint32_t want_keep_size,
 *                                        uint32_t want_punch_hole,
 *                                        uint32_t want_collapse,
 *                                        uint32_t want_zero);
 *     - Pack soft FALLOC_FL_* wants into a mode mask (0x01|0x02|0x08|0x10).
 *   uint32_t __gj_falloc_modes_pack_u_7368  (alias)
 *   __libcgj_batch7368_marker = "libcgj-batch7368"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Unique gj_falloc_modes_pack_u_7368 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7368_marker[] = "libcgj-batch7368";

/* Soft pack bits matching Linux FALLOC_FL_* layout (libcgj fcntl.h). */
#define B7368_FALLOC_FL_KEEP_SIZE      ((uint32_t)0x01u)
#define B7368_FALLOC_FL_PUNCH_HOLE     ((uint32_t)0x02u)
#define B7368_FALLOC_FL_COLLAPSE_RANGE ((uint32_t)0x08u)
#define B7368_FALLOC_FL_ZERO_RANGE     ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7368_falloc_modes_pack(uint32_t u32WantKeepSize,
                        uint32_t u32WantPunchHole,
                        uint32_t u32WantCollapse,
                        uint32_t u32WantZero)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantKeepSize != 0u)
		u32Pack |= B7368_FALLOC_FL_KEEP_SIZE;
	if (u32WantPunchHole != 0u)
		u32Pack |= B7368_FALLOC_FL_PUNCH_HOLE;
	if (u32WantCollapse != 0u)
		u32Pack |= B7368_FALLOC_FL_COLLAPSE_RANGE;
	if (u32WantZero != 0u)
		u32Pack |= B7368_FALLOC_FL_ZERO_RANGE;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_falloc_modes_pack_u_7368 - soft-pack fallocate mode wants.
 *
 * want_keep_size:  non-zero to set FALLOC_FL_KEEP_SIZE (0x01)
 * want_punch_hole: non-zero to set FALLOC_FL_PUNCH_HOLE (0x02)
 * want_collapse:   non-zero to set FALLOC_FL_COLLAPSE_RANGE (0x08)
 * want_zero:       non-zero to set FALLOC_FL_ZERO_RANGE (0x10)
 *
 * Returns bitmask of requested FALLOC_FL_* bits. Pure integer pack;
 * does not call fallocate. No parent wires.
 */
uint32_t
gj_falloc_modes_pack_u_7368(uint32_t u32WantKeepSize,
                            uint32_t u32WantPunchHole,
                            uint32_t u32WantCollapse,
                            uint32_t u32WantZero)
{
	(void)NULL;
	return b7368_falloc_modes_pack(u32WantKeepSize, u32WantPunchHole,
	                               u32WantCollapse, u32WantZero);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_falloc_modes_pack_u_7368(uint32_t u32WantKeepSize,
                                       uint32_t u32WantPunchHole,
                                       uint32_t u32WantCollapse,
                                       uint32_t u32WantZero)
    __attribute__((alias("gj_falloc_modes_pack_u_7368")));
