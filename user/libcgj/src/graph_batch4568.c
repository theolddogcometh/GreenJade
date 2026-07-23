/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4568: base64 4-char group count for n octets (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_b64_group_count_u(size_t n);
 *     - Number of output quartets for n raw input octets: (n + 2) / 3.
 *       Matches enc_len / 4 when enc_len is 4 * ((n + 2) / 3).
 *   size_t __gj_b64_group_count_u  (alias)
 *   __libcgj_batch4568_marker = "libcgj-batch4568"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570). Distinct from
 * gj_b64_enc_len_u (batch4561) — group count only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4568_marker[] = "libcgj-batch4568";

/* ---- freestanding helpers ---------------------------------------------- */

/* ceil(n / 3) via (n + 2) / 3; n==0 → 0. */
static size_t
b4568_group_count(size_t cbN)
{
	return (cbN + 2u) / 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_group_count_u - number of base64 quartets for n raw octets.
 *
 * n: raw input byte count
 *
 * Returns (n + 2) / 3. Self-contained; no parent wires.
 */
size_t
gj_b64_group_count_u(size_t n)
{
	(void)NULL;
	return b4568_group_count(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_b64_group_count_u(size_t n)
    __attribute__((alias("gj_b64_group_count_u")));
