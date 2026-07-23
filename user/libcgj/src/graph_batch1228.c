/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1228: 26-child lowercase trie edge index helper.
 *
 * Surface (unique symbols):
 *   int gj_trie_next_ascii(int ch);
 *     — Map ASCII letter ch to child slot 0..25 for 'a'..'z' (case
 *       sensitive; uppercase → -1). Non-letter / out of 0..255 → -1.
 *       Simple index helper for fixed-alphabet tries (see batch219).
 *   int __gj_trie_next_ascii  (alias)
 *   __libcgj_batch1228_marker = "libcgj-batch1228"
 *
 * Distinct from gj_trie_insert / gj_trie_contains — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1228_marker[] = "libcgj-batch1228";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trie_next_ascii — 'a'..'z' → 0..25, else -1.
 */
int
gj_trie_next_ascii(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return -1;
	}
	if (nCh >= 'a' && nCh <= 'z') {
		return nCh - 'a';
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_trie_next_ascii(int nCh)
    __attribute__((alias("gj_trie_next_ascii")));
