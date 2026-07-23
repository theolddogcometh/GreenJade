/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1257: secure wipe of a PIN / password string.
 *
 * Surface (unique symbols):
 *   void gj_pin_string_wipe(char *s, size_t cap);
 *     — Zero up to cap bytes of the PIN/password buffer with volatile
 *       stores + compiler barrier. Intended for fixed-capacity PIN
 *       buffers (wipe whole capacity, not just strlen). NULL s or
 *       cap==0 is a no-op.
 *   void __gj_pin_string_wipe  (alias)
 *   __libcgj_batch1257_marker = "libcgj-batch1257"
 *
 * Distinct from gj_secure_wipe / gj_memzero_s — unique PIN surface.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1257_marker[] = "libcgj-batch1257";

/*
 * gj_pin_string_wipe — secure wipe of a capacity-bounded PIN buffer.
 */
void
gj_pin_string_wipe(char *szPin, size_t cbCap)
{
	volatile unsigned char *pV;
	size_t i;

	if (szPin == NULL || cbCap == 0u) {
		return;
	}

	pV = (volatile unsigned char *)(void *)szPin;
	for (i = 0u; i < cbCap; i++) {
		pV[i] = 0u;
	}

	__asm__ __volatile__("" ::: "memory");
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pin_string_wipe(char *szPin, size_t cbCap)
    __attribute__((alias("gj_pin_string_wipe")));
