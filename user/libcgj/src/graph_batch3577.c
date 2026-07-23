/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3577: freestanding uint64_t array fill (_u).
 *
 * Surface (unique symbols):
 *   void *gj_mem_fill_u64_u(uint64_t *p, size_t n, uint64_t v);
 *     - Write v into each of the first n elements of p. Returns p.
 *       p == NULL -> return NULL (no write). n == 0 -> return p.
 *   void *__gj_mem_fill_u64_u  (alias)
 *   __libcgj_batch3577_marker = "libcgj-batch3577"
 *
 * CREATE-ONLY exclusive continuum wave (3571-3580). Unique
 * gj_mem_fill_u64_u surface only; no multi-def. Distinct from
 * gj_u64_fill / gj_arr_u64_fill_u / gj_mem_set_u64 (LE byte pattern) /
 * gj_mem_fill_u32_u (batch3576). Element-wise word fill; n is element
 * count. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3577_marker[] = "libcgj-batch3577";

/* ---- freestanding helpers ---------------------------------------------- */

/* Set p[i] = v for i in [0, n). p non-NULL when n > 0. */
static void
b3577_fill(uint64_t *p, size_t n, uint64_t v)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		p[i] = v;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_fill_u64_u - set p[i] = v for i in [0, n); return p.
 *
 * p: base array (NULL -> NULL)
 * n: element count (not byte count)
 * v: fill word
 *
 * Edge cases: p == NULL -> NULL; n == 0 -> p unchanged.
 * No parent wires.
 */
void *
gj_mem_fill_u64_u(uint64_t *p, size_t n, uint64_t v)
{
	(void)NULL;
	if (p == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return (void *)p;
	}
	b3577_fill(p, n, v);
	return (void *)p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_fill_u64_u(uint64_t *p, size_t n, uint64_t v)
    __attribute__((alias("gj_mem_fill_u64_u")));
