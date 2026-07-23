/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2974: ELF magic identity check (_u exclusive).
 *
 * Surface (unique symbols):
 *   unsigned gj_elf_is_elf_u(const void *buf, size_t len);
 *     - Return 1u if buf begins with ELF magic 0x7F 'E' 'L' 'F' and
 *       len >= 4; else 0u. NULL / short → 0u.
 *   unsigned __gj_elf_is_elf_u  (alias)
 *   __libcgj_batch2974_marker = "libcgj-batch2974"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_elf_is_magic (batch1141) / gj_magic_is_elf_bytes (batch1651) —
 * unique gj_elf_is_elf_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2974_marker[] = "libcgj-batch2974";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..3] is EI_MAG0..3. Caller guarantees p != NULL, cb >= 4. */
static unsigned
b2974_match_elf(const unsigned char *p)
{
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_is_elf_u - 1u if first four octets are 0x7F 'E' 'L' 'F'.
 *
 * buf: image start (NULL → 0u)
 * len: byte length of the buffer (need >= 4)
 */
unsigned
gj_elf_is_elf_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL || cb < 4u) {
		return 0u;
	}
	return b2974_match_elf((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_elf_is_elf_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_is_elf_u")));
