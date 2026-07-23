/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2465: BLAKE2b maximum digest length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_blake2b_digest_max_u(void);
 *     - Returns the maximum BLAKE2b digest length in octets (64).
 *   uint32_t __gj_blake2b_digest_max_u  (alias)
 *   __libcgj_batch2465_marker = "libcgj-batch2465"
 *
 * Crypto constants exclusive wave (2461-2470). Unique
 * gj_blake2b_digest_max_u surface only; no multi-def. Size constant only —
 * no hash implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2465_marker[] = "libcgj-batch2465";

/* BLAKE2b maximum digest length in octets (RFC 7693). */
#define B2465_BLAKE2B_DIGEST_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2465_blake2b_digest_max(void)
{
	return B2465_BLAKE2B_DIGEST_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_blake2b_digest_max_u - report BLAKE2b maximum digest length in octets.
 *
 * Always returns 64. Soft compile-time crypto size tag; does not hash.
 */
uint32_t
gj_blake2b_digest_max_u(void)
{
	(void)NULL;
	return b2465_blake2b_digest_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_blake2b_digest_max_u(void)
    __attribute__((alias("gj_blake2b_digest_max_u")));
