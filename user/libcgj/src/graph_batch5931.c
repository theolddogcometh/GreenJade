/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5931: install image digest type none.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_img_digest_type_none_5931(void);
 *     - Returns 0 (soft type code for unset / no digest on an install
 *       image descriptor).
 *   uint32_t __gj_install_img_digest_type_none_5931  (alias)
 *   __libcgj_batch5931_marker = "libcgj-batch5931"
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

const char __libcgj_batch5931_marker[] = "libcgj-batch5931";

/* Soft install-image digest type: none / unset. */
#define B5931_DIGEST_TYPE_NONE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5931_type_none(void)
{
	return B5931_DIGEST_TYPE_NONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_img_digest_type_none_5931 - digest type code for no digest.
 *
 * Always returns 0. Soft compile-time constant for freestanding install
 * image descriptor tables. No parent wires.
 */
uint32_t
gj_install_img_digest_type_none_5931(void)
{
	(void)NULL;
	return b5931_type_none();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_img_digest_type_none_5931(void)
    __attribute__((alias("gj_install_img_digest_type_none_5931")));
