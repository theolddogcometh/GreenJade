/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch544: freestanding key/value string table lookup.
 *
 * Surface (unique symbols):
 *   const char *gj_kv_lookup(const char *const *keys,
 *                            const char *const *vals,
 *                            size_t n, const char *key);
 *     — Linear scan i = 0 .. n-1; first keys[i] equal to key by
 *       byte-exact C-string compare → return vals[i]. Any of
 *       keys/vals/key NULL → NULL. Not found → NULL.
 *   const char *__gj_kv_lookup  (alias)
 *   __libcgj_batch544_marker = "libcgj-batch544"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch544_marker[] = "libcgj-batch544";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Byte-exact C-string equality (NUL-terminated). Does not call strcmp.
 * a and b must be non-NULL.
 */
static int
b544_streq(const char *a, const char *b)
{
	size_t i;

	i = 0u;
	for (;;) {
		unsigned char ca;
		unsigned char cb;

		ca = (unsigned char)a[i];
		cb = (unsigned char)b[i];
		if (ca != cb) {
			return 0;
		}
		if (ca == 0u) {
			return 1;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kv_lookup — linear key→value lookup over parallel string tables.
 *
 * keys: length-n array of C-string keys (NULL → return NULL)
 * vals: length-n array of C-string values (NULL → return NULL)
 * n:    number of entries; 0 → not found (NULL)
 * key:  probe key (NULL → return NULL)
 *
 * First i with keys[i] non-NULL and byte-exact equal to key wins;
 * returns vals[i] (which may itself be NULL). Does not call libc.
 */
const char *
gj_kv_lookup(const char *const *keys, const char *const *vals, size_t n,
    const char *key)
{
	size_t i;

	if (keys == NULL || vals == NULL || key == NULL) {
		return NULL;
	}

	for (i = 0u; i < n; i++) {
		const char *k;

		k = keys[i];
		if (k != NULL && b544_streq(k, key)) {
			return vals[i];
		}
	}
	return NULL;
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_kv_lookup(const char *const *keys, const char *const *vals,
    size_t n, const char *key)
    __attribute__((alias("gj_kv_lookup")));
