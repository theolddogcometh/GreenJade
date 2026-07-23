/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7156: sem_open needs mode/value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sem_needs_mode_u_7156(uint32_t flags);
 *     - Return 1 if oflag has O_CREAT (mode+value required), else 0.
 *   uint32_t __gj_sem_needs_mode_u_7156  (alias)
 *   __libcgj_batch7156_marker = "libcgj-batch7156"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Unique gj_sem_needs_mode_u_7156 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7156_marker[] = "libcgj-batch7156";

/* O_CREAT: when set, sem_open takes mode and initial value. */
#define B7156_O_CREAT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7156_sem_needs_mode(uint32_t u32Flags)
{
	return ((u32Flags & B7156_O_CREAT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sem_needs_mode_u_7156 - 1 if oflag requires mode and value args.
 *
 * flags: sem_open(3) oflag argument
 *
 * Returns 1 when O_CREAT is set (varargs mode + value are required),
 * else 0. Soft catalog check; does not call sem_open. No parent wires.
 */
uint32_t
gj_sem_needs_mode_u_7156(uint32_t u32Flags)
{
	(void)NULL;
	return b7156_sem_needs_mode(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sem_needs_mode_u_7156(uint32_t u32Flags)
    __attribute__((alias("gj_sem_needs_mode_u_7156")));
