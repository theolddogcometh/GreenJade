/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7884: C-locale strxfrm weight-length probe.
 *
 * Surface (unique symbols):
 *   size_t gj_strxfrm_need_7884(const char *src);
 *     - Return the C-locale transform length of src (octet count not
 *       including NUL). src NULL -> 0. Does not write a buffer.
 *   size_t __gj_strxfrm_need_7884  (alias)
 *   __libcgj_batch7884_marker = "libcgj-batch7884"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_strxfrm_need_7884 surface only; no multi-def. Distinct from
 * gj_strxfrm_c_7882 (which also copies). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7884_marker[] = "libcgj-batch7884";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b7884_len(const unsigned char *p)
{
	size_t n;

	n = 0u;
	while (p[n] != 0u) {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strxfrm_need_7884 - C-locale strxfrm required weight length.
 *
 * src: NUL-terminated source (NULL -> 0)
 *
 * In the C locale weights equal the source octets, so this is freestanding
 * strlen of src. Soft probe only; no buffer write. No parent wires.
 */
size_t
gj_strxfrm_need_7884(const char *src)
{
	(void)NULL;
	if (src == NULL) {
		return 0u;
	}
	return b7884_len((const unsigned char *)src);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_strxfrm_need_7884(const char *src)
    __attribute__((alias("gj_strxfrm_need_7884")));
