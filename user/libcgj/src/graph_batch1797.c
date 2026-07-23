/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1797: unpack u64 version tuple into three u16.
 *
 * Surface (unique symbols):
 *   int gj_version_tuple_unpack(uint64_t packed, uint16_t *major,
 *                               uint16_t *minor, uint16_t *patch);
 *     — Inverse of gj_version_tuple_pack: extract major [47:32],
 *       minor [31:16], patch [15:0] into the out pointers. Returns 0 on
 *       success, -1 if any out pointer is NULL. Upper 16 bits of packed
 *       are ignored.
 *   int __gj_version_tuple_unpack  (alias)
 *   __libcgj_batch1797_marker = "libcgj-batch1797"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1797_marker[] = "libcgj-batch1797";

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract major / minor / patch from packed layout. */
static void
b1797_unpack(uint64_t uPacked, uint16_t *pMaj, uint16_t *pMin,
             uint16_t *pPat)
{
	*pMaj = (uint16_t)((uPacked >> 32) & 0xffffu);
	*pMin = (uint16_t)((uPacked >> 16) & 0xffffu);
	*pPat = (uint16_t)(uPacked & 0xffffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_version_tuple_unpack — unpack u64 into three u16; 0 / -1.
 */
int
gj_version_tuple_unpack(uint64_t uPacked, uint16_t *pMaj, uint16_t *pMin,
                        uint16_t *pPat)
{
	if (pMaj == NULL || pMin == NULL || pPat == NULL) {
		return -1;
	}
	b1797_unpack(uPacked, pMaj, pMin, pPat);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_version_tuple_unpack(uint64_t uPacked, uint16_t *pMaj,
                              uint16_t *pMin, uint16_t *pPat)
    __attribute__((alias("gj_version_tuple_unpack")));
