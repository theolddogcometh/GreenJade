/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8132: strcat destination-NULL error stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcat_dst_null_u_8132(const char *dst);
 *     - Return 1 if dst is NULL (invalid strcat destination); else 0.
 *   uint32_t __gj_strcat_dst_null_u_8132  (alias)
 *   __libcgj_batch8132_marker = "libcgj-batch8132"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strcat_dst_null_u_8132 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8132_marker[] = "libcgj-batch8132";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8132_dst_null(const char *dst)
{
	return (dst == NULL) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcat_dst_null_u_8132 - 1 if strcat destination pointer is NULL.
 *
 * dst: candidate strcat destination
 *
 * Returns 1 when dst is NULL (invalid), else 0. Soft catalog test; does
 * not call strcat(3). No parent wires.
 */
uint32_t
gj_strcat_dst_null_u_8132(const char *dst)
{
	(void)NULL;
	return b8132_dst_null(dst);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcat_dst_null_u_8132(const char *dst)
    __attribute__((alias("gj_strcat_dst_null_u_8132")));
