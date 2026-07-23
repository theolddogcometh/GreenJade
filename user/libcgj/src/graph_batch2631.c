/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2631: product rootfs marker soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_rootfs_marker_ok(const void *buf, size_t len);
 *     - Return 1 if buf begins with the product rootfs marker
 *       "GJROOT1" and len >= 7; else 0. NULL / short / mismatch → 0.
 *       Soft pure-data identity check; no filesystem access.
 *   int __gj_rootfs_marker_ok  (alias)
 *   __libcgj_batch2631_marker = "libcgj-batch2631"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_rootfs_path_ok (batch2363 absolute path shape) — unique
 * gj_rootfs_marker_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2631_marker[] = "libcgj-batch2631";

/* Product rootfs handoff marker: "GJROOT1" (7 octets, no NUL required). */
#define B2631_MARKER_LEN  7u

static const unsigned char b2631_magic[B2631_MARKER_LEN] = {
	(unsigned char)'G', (unsigned char)'J',
	(unsigned char)'R', (unsigned char)'O',
	(unsigned char)'O', (unsigned char)'T',
	(unsigned char)'1'
};

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if buf begins with GJROOT1 and has at least 7 octets. */
static int
b2631_marker_ok(const unsigned char *p, size_t cb)
{
	size_t i;

	if (p == NULL || cb < (size_t)B2631_MARKER_LEN) {
		return 0;
	}
	for (i = 0u; i < (size_t)B2631_MARKER_LEN; i++) {
		if (p[i] != b2631_magic[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rootfs_marker_ok - 1 if buf begins with product rootfs marker.
 *
 * buf: candidate marker bytes; len: available octet count
 *
 * Returns 1 when len >= 7 and the first seven octets are "GJROOT1",
 * else 0. Does not call libc or touch the filesystem.
 */
int
gj_rootfs_marker_ok(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL) {
		return 0;
	}
	return b2631_marker_ok((const unsigned char *)pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rootfs_marker_ok(const void *pBuf, size_t cb)
    __attribute__((alias("gj_rootfs_marker_ok")));
