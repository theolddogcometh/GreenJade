/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5939: install image digest type ok predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_img_digest_type_ok_5939(uint32_t t);
 *     - Return 1 when t is a known install-image digest type code
 *       (0=none, 1=sha256, 2=sha512, 3=sha1, 4=blake2b), else 0.
 *   uint32_t __gj_install_img_digest_type_ok_5939  (alias)
 *   __libcgj_batch5939_marker = "libcgj-batch5939"
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
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5939_marker[] = "libcgj-batch5939";

/* Known type codes: 0..4 inclusive (none through blake2b). */
#define B5939_TYPE_MAX  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5939_type_ok(uint32_t t)
{
	if (t <= B5939_TYPE_MAX) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_img_digest_type_ok_5939 - known digest type predicate.
 *
 * t: candidate type code from install-image descriptor tables.
 *
 * Returns 1 when t is in [0, 4], else 0. Pure integer; no I/O.
 * No parent wires.
 */
uint32_t
gj_install_img_digest_type_ok_5939(uint32_t t)
{
	(void)NULL;
	return b5939_type_ok(t);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_img_digest_type_ok_5939(uint32_t t)
    __attribute__((alias("gj_install_img_digest_type_ok_5939")));
