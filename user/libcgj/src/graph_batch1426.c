/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1426: gj_sysv_checksum — historic System V sum.
 *
 * Surface (unique symbols):
 *   uint16_t gj_sysv_checksum(const void *data, size_t n);
 *     — SYSV / "sum -s" style checksum over n bytes:
 *         s = sum of all bytes
 *         s = (s & 0xffff) + (s >> 16); fold again if needed
 *       Returns low 16 bits. data == NULL or n == 0 → 0.
 *   uint16_t __gj_sysv_checksum  (alias)
 *   __libcgj_batch1426_marker = "libcgj-batch1426"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1426_marker[] = "libcgj-batch1426";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold a wide sum down to 16 bits (SYSV end-around carry style). */
static uint16_t
b1426_fold_sysv(uint32_t u32S)
{
	u32S = (u32S & 0xffffu) + (u32S >> 16);
	if (u32S > 0xffffu) {
		u32S = (u32S & 0xffffu) + 1u;
	}
	return (uint16_t)(u32S & 0xffffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sysv_checksum — System V sum algorithm (checksum field only).
 */
uint16_t
gj_sysv_checksum(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32S;
	size_t i;

	if (pData == NULL || cb == 0u) {
		return 0u;
	}

	p = (const unsigned char *)pData;
	u32S = 0u;
	for (i = 0u; i < cb; i++) {
		u32S += (uint32_t)p[i];
	}
	return b1426_fold_sysv(u32S);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_sysv_checksum(const void *pData, size_t cb)
    __attribute__((alias("gj_sysv_checksum")));
