/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7726: chdir absolute path first-byte stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_is_abs_u_7726(uint32_t first);
 *     - Return 1 if first path byte is '/', else 0.
 *   uint32_t __gj_chdir_is_abs_u_7726  (alias)
 *   __libcgj_batch7726_marker = "libcgj-batch7726"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique
 * gj_chdir_is_abs_u_7726 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7726_marker[] = "libcgj-batch7726";

/* ASCII solidus: absolute-path marker on Unix. */
#define B7726_SLASH  ((uint32_t)0x2Fu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7726_is_abs(uint32_t u32First)
{
	return (u32First == B7726_SLASH) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_is_abs_u_7726 - 1 if first path byte marks an absolute path.
 *
 * first: soft first path byte (0..255 meaningful)
 *
 * Returns 1 when first == '/', else 0. Pure byte test; no parent wires.
 * Does not call chdir.
 */
uint32_t
gj_chdir_is_abs_u_7726(uint32_t u32First)
{
	(void)NULL;
	return b7726_is_abs(u32First);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chdir_is_abs_u_7726(uint32_t u32First)
    __attribute__((alias("gj_chdir_is_abs_u_7726")));
