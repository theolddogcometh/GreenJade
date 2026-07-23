/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5940: install image digest length for type.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_img_digest_len_for_type_5940(uint32_t t);
 *     - Map soft digest type code to digest length in octets:
 *         0 (none)     → 0
 *         1 (sha256)   → 32
 *         2 (sha512)   → 64
 *         3 (sha1)     → 20
 *         4 (blake2b)  → 64  (default full output)
 *       Unknown t → 0.
 *   uint32_t __gj_install_img_digest_len_for_type_5940  (alias)
 *   __libcgj_batch5940_marker = "libcgj-batch5940"
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
 * multi-def. Distinct from per-algorithm length stubs in 5936-5938.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5940_marker[] = "libcgj-batch5940";

#define B5940_TYPE_NONE     0u
#define B5940_TYPE_SHA256   1u
#define B5940_TYPE_SHA512   2u
#define B5940_TYPE_SHA1     3u
#define B5940_TYPE_BLAKE2B  4u

#define B5940_LEN_NONE      0u
#define B5940_LEN_SHA256    32u
#define B5940_LEN_SHA512    64u
#define B5940_LEN_SHA1      20u
#define B5940_LEN_BLAKE2B   64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5940_len_for_type(uint32_t t)
{
	if (t == B5940_TYPE_NONE) {
		return B5940_LEN_NONE;
	}
	if (t == B5940_TYPE_SHA256) {
		return B5940_LEN_SHA256;
	}
	if (t == B5940_TYPE_SHA512) {
		return B5940_LEN_SHA512;
	}
	if (t == B5940_TYPE_SHA1) {
		return B5940_LEN_SHA1;
	}
	if (t == B5940_TYPE_BLAKE2B) {
		return B5940_LEN_BLAKE2B;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_img_digest_len_for_type_5940 - map type code to octet length.
 *
 * t: soft type code (0=none, 1=sha256, 2=sha512, 3=sha1, 4=blake2b).
 *
 * Returns digest size in octets, or 0 for none/unknown. Size table only;
 * does not hash. No parent wires.
 */
uint32_t
gj_install_img_digest_len_for_type_5940(uint32_t t)
{
	(void)NULL;
	return b5940_len_for_type(t);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_img_digest_len_for_type_5940(uint32_t t)
    __attribute__((alias("gj_install_img_digest_len_for_type_5940")));
