/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7271: accept4 SOCK_CLOEXEC flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_accept4_cloexec_id_7271(void);
 *     - Return soft SOCK_CLOEXEC constant for accept4 flags (0x80000).
 *   uint32_t __gj_accept4_cloexec_id_7271  (alias)
 *   __libcgj_batch7271_marker = "libcgj-batch7271"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Unique gj_accept4_cloexec_id_7271 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7271_marker[] = "libcgj-batch7271";

/* SOCK_CLOEXEC: close-on-exec on accepted fd (Linux 02000000 = 0x80000). */
#define B7271_SOCK_CLOEXEC ((uint32_t)0x80000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7271_accept4_cloexec_id(void)
{
	return B7271_SOCK_CLOEXEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept4_cloexec_id_7271 - soft SOCK_CLOEXEC flag constant.
 *
 * Always returns 0x80000 (Linux SOCK_CLOEXEC as used by accept4 flags).
 * Catalog id only; does not call accept4. No parent wires.
 */
uint32_t
gj_accept4_cloexec_id_7271(void)
{
	(void)NULL;
	return b7271_accept4_cloexec_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_accept4_cloexec_id_7271(void)
    __attribute__((alias("gj_accept4_cloexec_id_7271")));
