/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5933: install image digest type SHA-512.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_img_digest_type_sha512_5933(void);
 *     - Returns 2 (soft type code for SHA-512 install-image digests).
 *   uint32_t __gj_install_img_digest_type_sha512_5933  (alias)
 *   __libcgj_batch5933_marker = "libcgj-batch5933"
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
 * multi-def. Distinct from gj_sha512_digest_len (batch1769). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5933_marker[] = "libcgj-batch5933";

/* Soft install-image digest type: SHA-512. */
#define B5933_DIGEST_TYPE_SHA512  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5933_type_sha512(void)
{
	return B5933_DIGEST_TYPE_SHA512;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_img_digest_type_sha512_5933 - digest type code for SHA-512.
 *
 * Always returns 2. Soft compile-time constant for freestanding install
 * image descriptor tables. Does not compute a hash. No parent wires.
 */
uint32_t
gj_install_img_digest_type_sha512_5933(void)
{
	(void)NULL;
	return b5933_type_sha512();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_img_digest_type_sha512_5933(void)
    __attribute__((alias("gj_install_img_digest_type_sha512_5933")));
