/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7890: strxfrm_l-style stub + wave identity.
 *
 * Surface (unique symbols):
 *   size_t gj_strxfrm_l_stub_7890(char *dst, const char *src, size_t n,
 *                                 uint32_t loc_tag);
 *     - C-locale identity transform; loc_tag ignored. Semantics match
 *       gj_strxfrm_c_7882 for dst/src/n.
 *   uint32_t gj_batch_id_7890(void);
 *     - Returns 7890 (end of 7881-7890 strcoll/strxfrm wave).
 *   uint32_t gj_strcoll_xfrm_wave_7890(void);
 *     - Wave fingerprint: (7881 << 16) | 7890.
 *   size_t __gj_strxfrm_l_stub_7890  (alias)
 *   uint32_t __gj_batch_id_7890  (alias)
 *   uint32_t __gj_strcoll_xfrm_wave_7890  (alias)
 *   __libcgj_batch7890_marker = "libcgj-batch7890"
 *
 * MILESTONE 7890 for exclusive continuum CREATE-ONLY wave
 * (7881-7889: strcoll_c, strxfrm_c, strncoll_c, strxfrm_need,
 * strcoll_ascii_ci, strcoll_eq, memcoll_c, strxfrm_into, strcoll_l_stub).
 * Unique *_7890 surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7890_marker[] = "libcgj-batch7890";

#define B7890_BATCH_ID    7890u
#define B7890_WAVE_START  7881u
#define B7890_WAVE_END    7890u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b7890_len(const unsigned char *p)
{
	size_t n;

	n = 0u;
	while (p[n] != 0u) {
		n++;
	}
	return n;
}

static size_t
b7890_xfrm(char *dst, const char *src, size_t n)
{
	const unsigned char *pSrc;
	size_t nLen;
	size_t nCopy;
	size_t i;

	if (src == NULL) {
		return 0u;
	}

	pSrc = (const unsigned char *)src;
	nLen = b7890_len(pSrc);

	if (dst != NULL && n > 0u) {
		nCopy = nLen;
		if (nCopy > (n - 1u)) {
			nCopy = n - 1u;
		}
		for (i = 0u; i < nCopy; i++) {
			dst[i] = (char)pSrc[i];
		}
		dst[nCopy] = '\0';
	}
	return nLen;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strxfrm_l_stub_7890 - strxfrm_l-shaped C-locale identity stub.
 *
 * loc_tag ignored. Soft freestanding only. No parent wires.
 */
size_t
gj_strxfrm_l_stub_7890(char *dst, const char *src, size_t n,
    uint32_t loc_tag)
{
	(void)NULL;
	(void)loc_tag;
	return b7890_xfrm(dst, src, n);
}

/*
 * gj_batch_id_7890 - report this TU's graph batch number.
 *
 * Always returns 7890.
 */
uint32_t
gj_batch_id_7890(void)
{
	return B7890_BATCH_ID;
}

/*
 * gj_strcoll_xfrm_wave_7890 - wave fingerprint for 7881-7890.
 *
 * Returns (7881 << 16) | 7890. Soft pure-data fingerprint only.
 */
uint32_t
gj_strcoll_xfrm_wave_7890(void)
{
	return (B7890_WAVE_START << 16) | B7890_WAVE_END;
}

/* ---- underscored aliases ----------------------------------------------- */

size_t __gj_strxfrm_l_stub_7890(char *dst, const char *src, size_t n,
    uint32_t loc_tag)
    __attribute__((alias("gj_strxfrm_l_stub_7890")));

uint32_t __gj_batch_id_7890(void)
    __attribute__((alias("gj_batch_id_7890")));

uint32_t __gj_strcoll_xfrm_wave_7890(void)
    __attribute__((alias("gj_strcoll_xfrm_wave_7890")));
