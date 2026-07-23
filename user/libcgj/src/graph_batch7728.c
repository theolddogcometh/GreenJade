/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7728: chdir path buffer room (NUL) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_path_room_u_7728(uint32_t len);
 *     - Return 1 if len + 1 <= PATH_MAX (room for trailing NUL), else 0.
 *   uint32_t __gj_chdir_path_room_u_7728  (alias)
 *   __libcgj_batch7728_marker = "libcgj-batch7728"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique
 * gj_chdir_path_room_u_7728 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7728_marker[] = "libcgj-batch7728";

/* Soft Linux PATH_MAX including trailing NUL capacity. */
#define B7728_PATH_MAX  ((uint32_t)4096u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7728_path_room(uint32_t u32Len)
{
	/* len + 1 must not overflow; reject len == UINT32_MAX. */
	if (u32Len == 0xFFFFFFFFu)
		return 0u;
	if ((u32Len + 1u) > B7728_PATH_MAX)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_path_room_u_7728 - 1 if path bytes + NUL fit soft PATH_MAX.
 *
 * len: soft path length in bytes (excluding trailing NUL)
 *
 * Returns 1 when len + 1 <= 4096, else 0. Zero length still has room
 * for NUL alone. Pure capacity test; no parent wires. Does not call
 * chdir.
 */
uint32_t
gj_chdir_path_room_u_7728(uint32_t u32Len)
{
	(void)NULL;
	return b7728_path_room(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chdir_path_room_u_7728(uint32_t u32Len)
    __attribute__((alias("gj_chdir_path_room_u_7728")));
