/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7276: accept4 zero-flags (plain) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_accept4_is_zero_u_7276(uint32_t flags);
 *     - Return 1 if flags is 0 (plain accept-like), else 0.
 *   uint32_t __gj_accept4_is_zero_u_7276  (alias)
 *   __libcgj_batch7276_marker = "libcgj-batch7276"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Unique gj_accept4_is_zero_u_7276 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7276_marker[] = "libcgj-batch7276";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7276_accept4_is_zero(uint32_t u32Flags)
{
	return (u32Flags == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept4_is_zero_u_7276 - 1 if flags is zero (plain accept-like).
 *
 * flags: accept4(2) flags argument
 *
 * Returns 1 when flags == 0 (no SOCK_CLOEXEC / SOCK_NONBLOCK), else 0.
 * Soft equality test only; does not call accept4. No parent wires.
 */
uint32_t
gj_accept4_is_zero_u_7276(uint32_t u32Flags)
{
	(void)NULL;
	return b7276_accept4_is_zero(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_accept4_is_zero_u_7276(uint32_t u32Flags)
    __attribute__((alias("gj_accept4_is_zero_u_7276")));
