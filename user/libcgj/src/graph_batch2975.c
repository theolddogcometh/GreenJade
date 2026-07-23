/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2975: ELFCLASS64 predicate (_u exclusive).
 *
 * Surface (unique symbols):
 *   unsigned gj_elf_is_64_u(const void *buf, size_t len);
 *     - Return 1u if buf is ELF magic and EI_CLASS is ELFCLASS64 (2);
 *       else 0u. Requires len >= 5. NULL / short / non-ELF → 0u.
 *   unsigned __gj_elf_is_64_u  (alias)
 *   __libcgj_batch2975_marker = "libcgj-batch2975"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_elf_class / gj_elf_class_from_ei — unique gj_elf_is_64_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2975_marker[] = "libcgj-batch2975";

#define B2975_EI_CLASS    4u
#define B2975_ELFCLASS64  2u

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b2975_is_elf_magic(const unsigned char *p)
{
	if (p[0] != 0x7fu || p[1] != (unsigned char)'E' ||
	    p[2] != (unsigned char)'L' || p[3] != (unsigned char)'F') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_is_64_u - 1u if ELF magic and EI_CLASS == ELFCLASS64.
 *
 * buf: image start (NULL → 0u)
 * len: byte length (need >= 5 for EI_CLASS)
 */
unsigned
gj_elf_is_64_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	(void)NULL;
	if (pBuf == NULL || cb < 5u) {
		return 0u;
	}
	p = (const unsigned char *)pBuf;
	if (b2975_is_elf_magic(p) == 0u) {
		return 0u;
	}
	if (p[B2975_EI_CLASS] != (unsigned char)B2975_ELFCLASS64) {
		return 0u;
	}
	return 1u;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_elf_is_64_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_elf_is_64_u")));
