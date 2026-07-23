/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5936: install image SHA-256 digest length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_img_digest_len_sha256_5936(void);
 *     - Returns 32 (SHA-256 digest size in octets for install images).
 *   uint32_t __gj_install_img_digest_len_sha256_5936  (alias)
 *   __libcgj_batch5936_marker = "libcgj-batch5936"
 *
 * Exclusive continuum CREATE-ONLY (5931-5940: install image digest
 * length/type stubs —
 * install_img_digest_type_none_5931,
 * install_img_digest_type_sha256_5932,
 * install_img_digest_type_sha512_5933,
 * install_img_digest_type_sha1_5934,
 * install_img_digest_type_blake2b_5935,
 * install_img_digest_len_sha256_5936,
 * install_img_digest_len_sha512_5937,
 * install_img_digest_len_sha1_5938,
 * install_img_digest_type_ok_5939,
 * install_img_digest_len_for_type_5940). Unique surface only; no
 * multi-def. Distinct from gj_sha256_digest_len (batch1768) and
 * gj_install_img_digest_type_sha256_5932 (batch5932). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5936_marker[] = "libcgj-batch5936";

/* SHA-256 digest length in octets. */
#define B5936_SHA256_DIGEST_LEN  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5936_len_sha256(void)
{
	return B5936_SHA256_DIGEST_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_img_digest_len_sha256_5936 - SHA-256 digest length (octets).
 *
 * Always returns 32. Size constant only; does not hash. No parent wires.
 */
uint32_t
gj_install_img_digest_len_sha256_5936(void)
{
	(void)NULL;
	return b5936_len_sha256();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_img_digest_len_sha256_5936(void)
    __attribute__((alias("gj_install_img_digest_len_sha256_5936")));
