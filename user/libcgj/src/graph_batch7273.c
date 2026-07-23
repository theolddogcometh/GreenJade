/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7273: accept4 known flags mask id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_accept4_known_mask_id_7273(void);
 *     - Return soft known accept4 flags mask (SOCK_CLOEXEC|SOCK_NONBLOCK).
 *   uint32_t __gj_accept4_known_mask_id_7273  (alias)
 *   __libcgj_batch7273_marker = "libcgj-batch7273"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Unique gj_accept4_known_mask_id_7273 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7273_marker[] = "libcgj-batch7273";

/*
 * Known accept4(2) flags mask:
 *   SOCK_CLOEXEC (0x80000) | SOCK_NONBLOCK (0x800) = 0x80800
 */
#define B7273_ACCEPT4_KNOWN_MASK ((uint32_t)0x80800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7273_accept4_known_mask_id(void)
{
	return B7273_ACCEPT4_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept4_known_mask_id_7273 - soft known accept4 flags mask.
 *
 * Always returns 0x80800 (SOCK_CLOEXEC | SOCK_NONBLOCK). Catalog id
 * only; does not call accept4. No parent wires.
 */
uint32_t
gj_accept4_known_mask_id_7273(void)
{
	(void)NULL;
	return b7273_accept4_known_mask_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_accept4_known_mask_id_7273(void)
    __attribute__((alias("gj_accept4_known_mask_id_7273")));
