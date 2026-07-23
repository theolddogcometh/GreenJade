/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1651: ELF identity magic bytes (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_elf_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with ELF magic 0x7F 'E' 'L' 'F'
 *       and len >= 4; else 0 (FAIL). NULL / short → 0.
 *   int __gj_magic_is_elf_bytes  (alias)
 *   __libcgj_batch1651_marker = "libcgj-batch1651"
 *
 * Distinct from gj_elf_is_magic (batch1141) / gj_elf64_validate — avoid
 * multi-def. Exclusive file-magic wave (1651–1659).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1651_marker[] = "libcgj-batch1651";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..3] is EI_MAG0..3. Caller guarantees p != NULL and cb >= 4. */
static int
b1651_match_elf(const unsigned char *p)
{
	return (p[0] == 0x7fu && p[1] == (unsigned char)'E' &&
	        p[2] == (unsigned char)'L' && p[3] == (unsigned char)'F')
	           ? 1
	           : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_elf_bytes — 1 if first four octets are 0x7F 'E' 'L' 'F'.
 */
int
gj_magic_is_elf_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	return b1651_match_elf((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_elf_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_elf_bytes")));
