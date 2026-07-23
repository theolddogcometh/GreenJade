/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7724: chdir soft NAME_MAX id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_name_max_7724(void);
 *     - Return soft NAME_MAX for path component length (255).
 *   uint32_t __gj_chdir_name_max_7724  (alias)
 *   __libcgj_batch7724_marker = "libcgj-batch7724"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique
 * gj_chdir_name_max_7724 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7724_marker[] = "libcgj-batch7724";

/* Soft Linux NAME_MAX for a single path component. */
#define B7724_NAME_MAX  ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7724_name_max(void)
{
	return B7724_NAME_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_name_max_7724 - soft NAME_MAX for chdir component length.
 *
 * Always returns 255. Pure constant tag; no parent wires. Does not
 * call chdir or readdir.
 */
uint32_t
gj_chdir_name_max_7724(void)
{
	(void)NULL;
	return b7724_name_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chdir_name_max_7724(void)
    __attribute__((alias("gj_chdir_name_max_7724")));
