/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch219: fixed alphabet (lowercase a-z) trie as
 * caller-owned node arrays. Clean-room freestanding pure C —
 * integer/pointer only. Compiles with -ffreestanding -msse2. No malloc,
 * no errno. No third-party source copied.
 *
 * Surface (unique symbols):
 *   int gj_trie_insert(unsigned *next, unsigned *term, size_t cap,
 *                      size_t *nnodes, const char *s);
 *     — Insert NUL-terminated word s (letters a-z only). Returns 0 on
 *       success (including re-insert of an existing word); -1 on NULL
 *       args, non a-z character, or when a new node would exceed cap.
 *   int gj_trie_contains(const unsigned *next, const unsigned *term,
 *                        size_t cap, size_t nnodes, const char *s);
 *     — 1 if s is a stored terminal word, 0 if not present / non a-z
 *       char mid-walk, -1 on NULL args or inconsistent nnodes/cap.
 *   __gj_trie_insert / __gj_trie_contains  (aliases)
 *   __libcgj_batch219_marker = "libcgj-batch219"
 *
 * Node-array layout (document carefully — this is the whole API):
 *   - Alphabet size A = 26. Letter c ('a'..'z') maps to index c - 'a'.
 *   - Nodes are dense indices in [0, nnodes). Node 0 is always the root.
 *   - next: length at least cap * 26. For node u, the child for letter
 *     index k is next[u * 26 + k]. Value 0 means "no edge" (null).
 *     Because root is node 0, live children are always indices >= 1.
 *   - term: length at least cap. term[u] != 0 marks u as end-of-word.
 *   - cap: maximum number of nodes the buffers can hold.
 *   - nnodes / *nnodes: live node count. Caller zero-initializes next
 *     and term before first use. *nnodes may be 0 on first insert; the
 *     root is then allocated (*nnodes becomes 1) if cap >= 1.
 *   - Empty string: insert sets term[0]; contains("") is true iff term[0].
 *
 * Storage (caller provides):
 *   unsigned next[cap * 26];
 *   unsigned term[cap];
 *   size_t   nnodes;   // 0 or 1 initially
 */

#include <stddef.h>

/* Fixed lowercase alphabet a..z */
#define B219_ALPHA 26u

const char __libcgj_batch219_marker[] = "libcgj-batch219";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map ch to alphabet index 0..25, or return (unsigned)-1 if not 'a'..'z'.
 */
static unsigned
b219_idx(int ch)
{
	if (ch >= 'a' && ch <= 'z') {
		return (unsigned)(ch - 'a');
	}
	return (unsigned)-1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trie_insert — walk/create path for s; mark terminal at the end.
 *
 * next/term/nnodes are modified in place. On capacity failure the trie
 * may retain partially allocated path nodes from this call (still a
 * valid prefix trie; the word is simply not marked terminal unless the
 * path already existed). Returns 0 on full success.
 */
int
gj_trie_insert(unsigned *next, unsigned *term, size_t cap, size_t *nnodes,
    const char *s)
{
	size_t cur;
	size_t n;
	const char *p;
	unsigned k;
	unsigned child;

	if (next == NULL || term == NULL || nnodes == NULL || s == NULL) {
		return -1;
	}
	if (cap == 0u) {
		return -1;
	}

	n = *nnodes;
	if (n > cap) {
		return -1;
	}

	/* Ensure root exists. */
	if (n == 0u) {
		n = 1u;
		*nnodes = 1u;
		/* next[0..25] and term[0] already 0 from caller zero-init. */
	}

	cur = 0u; /* root */
	for (p = s; *p != '\0'; p++) {
		k = b219_idx((int)(unsigned char)*p);
		if (k == (unsigned)-1) {
			return -1;
		}

		child = next[cur * B219_ALPHA + (size_t)k];
		if (child == 0u) {
			/* Allocate a fresh node. */
			if (n >= cap) {
				return -1;
			}
			child = (unsigned)n;
			n++;
			*nnodes = n;
			next[cur * B219_ALPHA + (size_t)k] = child;
			/* New node edges/term are 0 if caller zeroed buffers. */
		} else if ((size_t)child >= n || (size_t)child >= cap) {
			/* Corrupt / out-of-range child pointer. */
			return -1;
		}

		cur = (size_t)child;
	}

	term[cur] = 1u;
	return 0;
}

/*
 * gj_trie_contains — true if s is a terminal word in the trie.
 *
 * Walks next[] from the root; requires term[end] != 0. Does not modify
 * storage. nnodes is the live node count (pass *nnodes from insert).
 */
int
gj_trie_contains(const unsigned *next, const unsigned *term, size_t cap,
    size_t nnodes, const char *s)
{
	size_t cur;
	const char *p;
	unsigned k;
	unsigned child;

	if (next == NULL || term == NULL || s == NULL) {
		return -1;
	}
	if (cap == 0u || nnodes > cap) {
		return -1;
	}
	/* No root yet → nothing has been inserted. */
	if (nnodes == 0u) {
		return 0;
	}

	cur = 0u;
	for (p = s; *p != '\0'; p++) {
		k = b219_idx((int)(unsigned char)*p);
		if (k == (unsigned)-1) {
			return 0; /* cannot be a stored a-z word */
		}

		child = next[cur * B219_ALPHA + (size_t)k];
		if (child == 0u) {
			return 0;
		}
		if ((size_t)child >= nnodes || (size_t)child >= cap) {
			return -1;
		}
		cur = (size_t)child;
	}

	return (term[cur] != 0u) ? 1 : 0;
}

int __gj_trie_insert(unsigned *next, unsigned *term, size_t cap,
    size_t *nnodes, const char *s)
    __attribute__((alias("gj_trie_insert")));

int __gj_trie_contains(const unsigned *next, const unsigned *term, size_t cap,
    size_t nnodes, const char *s)
    __attribute__((alias("gj_trie_contains")));
