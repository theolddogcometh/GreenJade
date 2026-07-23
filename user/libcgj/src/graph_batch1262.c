/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1262: HKDF OKM length range check (SHA-256).
 *
 * Surface (unique symbols):
 *   int gj_hkdf_okm_len_ok(size_t len);
 *     — Return 1 if len is a valid HKDF-Expand output length for
 *       SHA-256 (RFC 5869): inclusive range [1, 255*32] (1..8160).
 *       Else 0 (zero or above max).
 *   int __gj_hkdf_okm_len_ok  (alias)
 *   __libcgj_batch1262_marker = "libcgj-batch1262"
 *
 * Distinct from hkdf_sha256_expand (batch71) — predicate only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1262_HASH_LEN = 32, B1262_OKM_MAX = 255 * B1262_HASH_LEN };

const char __libcgj_batch1262_marker[] = "libcgj-batch1262";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hkdf_okm_len_ok — 1 if len is in [1, 255*32] for SHA-256 HKDF.
 */
int
gj_hkdf_okm_len_ok(size_t cbLen)
{
	if (cbLen < 1u || cbLen > (size_t)B1262_OKM_MAX) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hkdf_okm_len_ok(size_t cbLen)
    __attribute__((alias("gj_hkdf_okm_len_ok")));
