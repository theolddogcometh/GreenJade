/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7134: mq_open O_CREAT presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mq_has_creat_u_7134(uint32_t flags);
 *     - Return 1 if flags has O_CREAT (0x40) set, else 0.
 *   uint32_t __gj_mq_has_creat_u_7134  (alias)
 *   __libcgj_batch7134_marker = "libcgj-batch7134"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Unique gj_mq_has_creat_u_7134 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7134_marker[] = "libcgj-batch7134";

/* O_CREAT bit (0x40). */
#define B7134_O_CREAT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7134_mq_has_creat(uint32_t u32Flags)
{
	return ((u32Flags & B7134_O_CREAT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mq_has_creat_u_7134 - 1 if flags includes O_CREAT.
 *
 * flags: mq_open(3) oflag argument
 *
 * Returns 1 when bit 0x40 is set, else 0. Soft bit test only; does not
 * call mq_open. No parent wires.
 */
uint32_t
gj_mq_has_creat_u_7134(uint32_t u32Flags)
{
	(void)NULL;
	return b7134_mq_has_creat(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mq_has_creat_u_7134(uint32_t u32Flags)
    __attribute__((alias("gj_mq_has_creat_u_7134")));
