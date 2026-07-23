/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch295: secure memory wipe (volatile + barrier).
 *
 * Surface (unique symbols — does not redefine explicit_bzero):
 *   void gj_secure_wipe(void *p, size_t n);
 *     — Zero n bytes at p with volatile stores so the compiler cannot
 *       elide the wipe as dead stores. Ends with a compiler memory
 *       barrier so subsequent code cannot reorder past the wipe.
 *       NULL p or n==0 is a no-op.
 *   __gj_secure_wipe  (alias)
 *   __libcgj_batch295_marker = "libcgj-batch295"
 *
 * Does not provide explicit_bzero / __explicit_bzero (already in
 * string.c / batch30). This is a distinct gj_* surface for callers
 * that need a freestanding wipe without pulling string symbols.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch295_marker[] = "libcgj-batch295";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_secure_wipe — overwrite n bytes at p with zeros, not optimizable away.
 *
 * Uses volatile stores so each write is a real side effect. A bare
 * compiler barrier (__asm__ volatile with "memory" clobber) after the
 * loop prevents the optimizer from sinking or discarding the sequence
 * relative to later observable uses of the buffer.
 *
 * Not a CPU fence (no mfence/sfence); freestanding, integer/pointer only.
 */
void
gj_secure_wipe(void *p, size_t n)
{
	volatile unsigned char *pV;
	size_t i;

	if (p == NULL || n == 0u) {
		return;
	}

	pV = (volatile unsigned char *)p;
	for (i = 0u; i < n; i++) {
		pV[i] = 0u;
	}

	/* Compiler memory barrier: wipe may not be reordered or elided. */
	__asm__ __volatile__("" ::: "memory");
}

void __gj_secure_wipe(void *p, size_t n)
    __attribute__((alias("gj_secure_wipe")));
