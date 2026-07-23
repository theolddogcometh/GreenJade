/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2961: freestanding octet fill (_u).
 *
 * Surface (unique symbols):
 *   void *gj_mem_set_u(void *p, int c, size_t n);
 *     - Fill n octets at p with (unsigned char)c. Returns p.
 *       p == NULL -> return NULL (no write). n == 0 -> return p unchanged.
 *   void *__gj_mem_set_u  (alias)
 *   __libcgj_batch2961_marker = "libcgj-batch2961"
 *
 * Post-2960 mem exclusive wave (2961-2970). Distinct from gj_mem_set
 * (batch396) / gj_mem_fill_n_u (batch2612) / gj_mem_set_u16/u32/u64 -
 * unique gj_mem_set_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2961_marker[] = "libcgj-batch2961";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write ch into pU[0..n). Caller guarantees pU non-NULL when n > 0. */
static void
b2961_set(unsigned char *pU, unsigned char ch, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		pU[i] = ch;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_set_u - fill n octets at p with byte value c.
 *
 * p: destination region of at least n bytes when n > 0 (NULL -> NULL)
 * c: fill byte (truncated to unsigned char)
 * n: number of octets to write (0 -> no-op, return p)
 *
 * Does not call libc memset. Returns p (or NULL when p is NULL).
 */
void *
gj_mem_set_u(void *p, int c, size_t n)
{
	(void)NULL;
	if (p == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return p;
	}
	b2961_set((unsigned char *)p, (unsigned char)c, n);
	return p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_set_u(void *p, int c, size_t n)
    __attribute__((alias("gj_mem_set_u")));
