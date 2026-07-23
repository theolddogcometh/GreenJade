/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4788: config key FNV-1a 32-bit hash (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfg_hash_key_u(const char *s);
 *     - FNV-1a 32-bit hash of the NUL-terminated key string s.
 *       NULL or empty string → FNV offset basis (0x811c9dc5).
 *   uint32_t __gj_cfg_hash_key_u  (alias)
 *   __libcgj_batch4788_marker = "libcgj-batch4788"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_hash_key_u
 * surface only; no multi-def. Distinct from fnv1a_32 / gj_fnv1a64 /
 * gj_wang_hash32 — config-key one-shot surface. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4788_marker[] = "libcgj-batch4788";

/* FNV-1a 32-bit parameters (public constants; clean-room reimplementation). */
#define B4788_FNV_OFF  0x811c9dc5u
#define B4788_FNV_PRIME 0x01000193u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4788_hash_key — FNV-1a 32 over the full C string (bytes as unsigned).
 */
static uint32_t
b4788_hash_key(const char *sz)
{
	uint32_t uH;
	size_t i;

	uH = B4788_FNV_OFF;
	if (sz == NULL) {
		return uH;
	}
	i = 0u;
	while (sz[i] != '\0') {
		uH ^= (uint32_t)(unsigned char)sz[i];
		uH *= B4788_FNV_PRIME;
		i++;
	}
	return uH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfg_hash_key_u - FNV-1a 32-bit hash of a config key string.
 *
 * s: NUL-terminated key (may be NULL)
 *
 * Returns the FNV-1a 32-bit digest of the bytes of s up to (not including)
 * the terminating NUL. NULL and empty both yield the offset basis
 * 0x811c9dc5. Case-sensitive (no folding).
 *
 * No parent wires.
 */
uint32_t
gj_cfg_hash_key_u(const char *sz)
{
	(void)NULL;
	return b4788_hash_key(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfg_hash_key_u(const char *sz)
    __attribute__((alias("gj_cfg_hash_key_u")));
