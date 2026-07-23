/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7132: mq_open O_EXCL flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mq_o_excl_id_7132(void);
 *     - Return soft O_EXCL constant (0x80 / 0200).
 *   uint32_t __gj_mq_o_excl_id_7132  (alias)
 *   __libcgj_batch7132_marker = "libcgj-batch7132"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Unique gj_mq_o_excl_id_7132 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7132_marker[] = "libcgj-batch7132";

/* O_EXCL: fail if queue already exists when O_CREAT is used (0200 / 0x80). */
#define B7132_O_EXCL ((uint32_t)0x80u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7132_mq_o_excl_id(void)
{
	return B7132_O_EXCL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mq_o_excl_id_7132 - soft O_EXCL oflag constant for mq_open.
 *
 * Always returns 0x80 (Linux O_EXCL octal 0200). Catalog id only; does
 * not call mq_open. No parent wires.
 */
uint32_t
gj_mq_o_excl_id_7132(void)
{
	(void)NULL;
	return b7132_mq_o_excl_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mq_o_excl_id_7132(void)
    __attribute__((alias("gj_mq_o_excl_id_7132")));
