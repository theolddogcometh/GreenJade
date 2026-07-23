/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1147: WebAssembly module magic check.
 *
 * Surface (unique symbols):
 *   int gj_wasm_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with WASM magic \0asm
 *       (0x00 0x61 0x73 0x6D) and len >= 4; else 0 (FAIL).
 *   int __gj_wasm_is_magic  (alias)
 *   __libcgj_batch1147_marker = "libcgj-batch1147"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1147_marker[] = "libcgj-batch1147";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wasm_is_magic — 1 if magic is \0asm.
 */
int
gj_wasm_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0x00u || p[1] != 0x61u || p[2] != 0x73u ||
	    p[3] != 0x6du) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wasm_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_wasm_is_magic")));
