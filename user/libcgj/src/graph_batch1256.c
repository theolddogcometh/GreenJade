/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1256: secure memory wipe (volatile + barrier).
 *
 * Surface (unique symbols / weak dual):
 *   void gj_secure_wipe_hint(void *p, size_t n)  [weak];
 *     — Zero n bytes at p with volatile stores so the compiler cannot
 *       elide the wipe as dead stores. Ends with a compiler memory
 *       barrier so subsequent code cannot reorder past the wipe.
 *       NULL p or n==0 is a no-op.
 *   void __gj_secure_wipe_hint  (weak alias)
 *   __libcgj_batch1256_marker = "libcgj-batch1256"
 *
 * Multi-def note: graph_batch295.c already exports a strong
 * gj_secure_wipe_hint / __gj_secure_wipe_hint. This TU provides the same API as
 * weak definitions so a full link prefers batch295 without multi-def,
 * while freestanding compile of this unit alone still resolves the
 * symbol. Marker remains unique to batch1256.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1256_marker[] = "libcgj-batch1256";

/*
 * gj_secure_wipe_hint — overwrite n bytes at p with zeros, not optimizable away.
 *
 * Marked weak: coexists with the strong definition in graph_batch295.c.
 */
__attribute__((weak)) void
gj_secure_wipe_hint(void *p, size_t cb)
{
	volatile unsigned char *pV;
	size_t i;

	if (p == NULL || cb == 0u) {
		return;
	}

	pV = (volatile unsigned char *)p;
	for (i = 0u; i < cb; i++) {
		pV[i] = 0u;
	}

	/* Compiler memory barrier: wipe may not be reordered or elided. */
	__asm__ __volatile__("" ::: "memory");
}

/* ---- underscored alias (weak) ------------------------------------------ */

void __gj_secure_wipe_hint(void *p, size_t cb)
    __attribute__((weak, alias("gj_secure_wipe_hint")));
