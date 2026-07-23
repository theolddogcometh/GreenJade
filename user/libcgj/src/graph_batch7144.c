/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7144: shm_open O_RDWR presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_has_rdwr_u_7144(uint32_t flags);
 *     - Return 1 if flags has O_RDWR (0x2) set, else 0.
 *   uint32_t __gj_shm_has_rdwr_u_7144  (alias)
 *   __libcgj_batch7144_marker = "libcgj-batch7144"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_has_rdwr_u_7144 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7144_marker[] = "libcgj-batch7144";

/* O_RDWR bit (0x2). */
#define B7144_O_RDWR ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7144_shm_has_rdwr(uint32_t u32Flags)
{
	return ((u32Flags & B7144_O_RDWR) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_has_rdwr_u_7144 - 1 if flags includes O_RDWR.
 *
 * flags: shm_open(3) oflag argument
 *
 * Returns 1 when bit 0x2 is set, else 0. Soft bit test only; does not
 * call shm_open. No parent wires.
 */
uint32_t
gj_shm_has_rdwr_u_7144(uint32_t u32Flags)
{
	(void)NULL;
	return b7144_shm_has_rdwr(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_has_rdwr_u_7144(uint32_t u32Flags)
    __attribute__((alias("gj_shm_has_rdwr_u_7144")));
