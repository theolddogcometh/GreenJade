/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7721: chdir soft PATH_MAX id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_path_max_7721(void);
 *     - Return soft PATH_MAX for chdir path-length checks (4096).
 *   uint32_t __gj_chdir_path_max_7721  (alias)
 *   __libcgj_batch7721_marker = "libcgj-batch7721"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique
 * gj_chdir_path_max_7721 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7721_marker[] = "libcgj-batch7721";

/* Soft Linux PATH_MAX for chdir path-length exclusive wave. */
#define B7721_PATH_MAX  ((uint32_t)4096u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7721_path_max(void)
{
	return B7721_PATH_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_path_max_7721 - soft PATH_MAX for chdir path length checks.
 *
 * Always returns 4096. Pure constant tag; no parent wires. Does not
 * call chdir or any libc path API.
 */
uint32_t
gj_chdir_path_max_7721(void)
{
	(void)NULL;
	return b7721_path_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chdir_path_max_7721(void)
    __attribute__((alias("gj_chdir_path_max_7721")));
