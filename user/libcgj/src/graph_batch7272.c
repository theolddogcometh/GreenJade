/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7272: accept4 SOCK_NONBLOCK flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_accept4_nonblock_id_7272(void);
 *     - Return soft SOCK_NONBLOCK constant for accept4 flags (0x800).
 *   uint32_t __gj_accept4_nonblock_id_7272  (alias)
 *   __libcgj_batch7272_marker = "libcgj-batch7272"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Unique gj_accept4_nonblock_id_7272 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7272_marker[] = "libcgj-batch7272";

/* SOCK_NONBLOCK: nonblocking accepted fd (Linux 04000 = 0x800). */
#define B7272_SOCK_NONBLOCK ((uint32_t)0x800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7272_accept4_nonblock_id(void)
{
	return B7272_SOCK_NONBLOCK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept4_nonblock_id_7272 - soft SOCK_NONBLOCK flag constant.
 *
 * Always returns 0x800 (Linux SOCK_NONBLOCK as used by accept4 flags).
 * Catalog id only; does not call accept4. No parent wires.
 */
uint32_t
gj_accept4_nonblock_id_7272(void)
{
	(void)NULL;
	return b7272_accept4_nonblock_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_accept4_nonblock_id_7272(void)
    __attribute__((alias("gj_accept4_nonblock_id_7272")));
