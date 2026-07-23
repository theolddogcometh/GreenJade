/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1654: WebAssembly module magic bytes.
 *
 * Surface (unique symbols):
 *   int gj_magic_is_wasm_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with WASM magic \0asm
 *       (0x00 0x61 0x73 0x6D) and len >= 4; else 0 (FAIL).
 *   int __gj_magic_is_wasm_bytes  (alias)
 *   __libcgj_batch1654_marker = "libcgj-batch1654"
 *
 * Distinct from gj_wasm_is_magic (batch1147) — avoid multi-def.
 * Exclusive file-magic wave (1651–1659).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1654_marker[] = "libcgj-batch1654";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..3] is \0 'a' 's' 'm'. Caller guarantees p and length. */
static int
b1654_match_wasm(const unsigned char *p)
{
	return (p[0] == 0x00u && p[1] == 0x61u && p[2] == 0x73u &&
	        p[3] == 0x6du)
	           ? 1
	           : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_wasm_bytes — 1 if magic is \0asm.
 */
int
gj_magic_is_wasm_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	return b1654_match_wasm((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_wasm_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_wasm_bytes")));
