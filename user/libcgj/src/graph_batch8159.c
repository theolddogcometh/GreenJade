/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8159: strtok separator-ok presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_sep_ok_u_8159(uint32_t sep);
 *     - Return 1 if low octet of sep is non-NUL (usable delimiter).
 *   uint32_t __gj_strtok_sep_ok_u_8159  (alias)
 *   __libcgj_batch8159_marker = "libcgj-batch8159"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_sep_ok_u_8159
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8159_marker[] = "libcgj-batch8159";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8159_sep_ok(uint32_t u32Sep)
{
	return (((u32Sep & 0xffu) != 0u) ? 1u : 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_sep_ok_u_8159 - 1 if sep is a non-NUL delimiter octet.
 *
 * sep: candidate delimiter (only low 8 bits inspected)
 *
 * Returns 1 when low octet is non-zero (usable as a strtok delimiter
 * byte); 0 when NUL (empty delimiter set edge). Does not call strtok.
 * No parent wires.
 */
uint32_t
gj_strtok_sep_ok_u_8159(uint32_t u32Sep)
{
	(void)NULL;
	return b8159_sep_ok(u32Sep);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_sep_ok_u_8159(uint32_t u32Sep)
    __attribute__((alias("gj_strtok_sep_ok_u_8159")));
