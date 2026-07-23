/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7131: mq_open O_CREAT flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mq_o_creat_id_7131(void);
 *     - Return soft O_CREAT constant (0x40 / 0100).
 *   uint32_t __gj_mq_o_creat_id_7131  (alias)
 *   __libcgj_batch7131_marker = "libcgj-batch7131"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Unique gj_mq_o_creat_id_7131 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7131_marker[] = "libcgj-batch7131";

/* O_CREAT: create queue if it does not exist (0100 / 0x40). */
#define B7131_O_CREAT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7131_mq_o_creat_id(void)
{
	return B7131_O_CREAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mq_o_creat_id_7131 - soft O_CREAT oflag constant for mq_open.
 *
 * Always returns 0x40 (Linux O_CREAT octal 0100). Catalog id only; does
 * not call mq_open. No parent wires.
 */
uint32_t
gj_mq_o_creat_id_7131(void)
{
	(void)NULL;
	return b7131_mq_o_creat_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mq_o_creat_id_7131(void)
    __attribute__((alias("gj_mq_o_creat_id_7131")));
