/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7133: mq_open O_NONBLOCK flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mq_o_nonblock_id_7133(void);
 *     - Return soft O_NONBLOCK constant (0x800 / 04000).
 *   uint32_t __gj_mq_o_nonblock_id_7133  (alias)
 *   __libcgj_batch7133_marker = "libcgj-batch7133"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Unique gj_mq_o_nonblock_id_7133 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7133_marker[] = "libcgj-batch7133";

/* O_NONBLOCK: non-blocking mq send/receive (04000 / 0x800). */
#define B7133_O_NONBLOCK ((uint32_t)0x800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7133_mq_o_nonblock_id(void)
{
	return B7133_O_NONBLOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mq_o_nonblock_id_7133 - soft O_NONBLOCK oflag constant for mq_open.
 *
 * Always returns 0x800 (Linux O_NONBLOCK octal 04000). Catalog id only;
 * does not call mq_open. No parent wires.
 */
uint32_t
gj_mq_o_nonblock_id_7133(void)
{
	(void)NULL;
	return b7133_mq_o_nonblock_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mq_o_nonblock_id_7133(void)
    __attribute__((alias("gj_mq_o_nonblock_id_7133")));
