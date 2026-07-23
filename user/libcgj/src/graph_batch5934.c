/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5934: install image digest type SHA-1.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_img_digest_type_sha1_5934(void);
 *     - Returns 3 (soft type code for SHA-1 install-image digests;
 *       legacy / interoperability only).
 *   uint32_t __gj_install_img_digest_type_sha1_5934  (alias)
 *   __libcgj_batch5934_marker = "libcgj-batch5934"
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
 * multi-def. Distinct from gj_sha1_digest_len_u (batch2461). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5934_marker[] = "libcgj-batch5934";

/* Soft install-image digest type: SHA-1 (legacy). */
#define B5934_DIGEST_TYPE_SHA1  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5934_type_sha1(void)
{
	return B5934_DIGEST_TYPE_SHA1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_img_digest_type_sha1_5934 - digest type code for SHA-1.
 *
 * Always returns 3. Soft compile-time constant for freestanding install
 * image descriptor tables. Does not compute a hash. No parent wires.
 */
uint32_t
gj_install_img_digest_type_sha1_5934(void)
{
	(void)NULL;
	return b5934_type_sha1();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_img_digest_type_sha1_5934(void)
    __attribute__((alias("gj_install_img_digest_type_sha1_5934")));
