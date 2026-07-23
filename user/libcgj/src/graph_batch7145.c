/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7145: shm_open O_CREAT presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_has_creat_u_7145(uint32_t flags);
 *     - Return 1 if flags has O_CREAT (0x40) set, else 0.
 *   uint32_t __gj_shm_has_creat_u_7145  (alias)
 *   __libcgj_batch7145_marker = "libcgj-batch7145"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_has_creat_u_7145 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7145_marker[] = "libcgj-batch7145";

/* O_CREAT bit (0x40 / octal 0100). */
#define B7145_O_CREAT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7145_shm_has_creat(uint32_t u32Flags)
{
	return ((u32Flags & B7145_O_CREAT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_has_creat_u_7145 - 1 if flags includes O_CREAT.
 *
 * flags: shm_open(3) oflag argument
 *
 * Returns 1 when bit 0x40 is set, else 0. Soft bit test only; does not
 * call shm_open. No parent wires.
 */
uint32_t
gj_shm_has_creat_u_7145(uint32_t u32Flags)
{
	(void)NULL;
	return b7145_shm_has_creat(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_has_creat_u_7145(uint32_t u32Flags)
    __attribute__((alias("gj_shm_has_creat_u_7145")));
