/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8156: strtok is-separator predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_is_sep_u_8156(uint32_t ch, uint32_t sep);
 *     - Return 1 if low octet of ch equals low octet of sep, else 0.
 *   uint32_t __gj_strtok_is_sep_u_8156  (alias)
 *   __libcgj_batch8156_marker = "libcgj-batch8156"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_is_sep_u_8156
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8156_marker[] = "libcgj-batch8156";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8156_is_sep(uint32_t u32Ch, uint32_t u32Sep)
{
	return (((u32Ch & 0xffu) == (u32Sep & 0xffu)) ? 1u : 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_is_sep_u_8156 - 1 if ch matches sep (low 8 bits).
 *
 * ch:  candidate octet (only low 8 bits compared)
 * sep: delimiter octet (only low 8 bits compared)
 *
 * Returns 1 when the low octets match, else 0. Does not call strtok.
 * No parent wires.
 */
uint32_t
gj_strtok_is_sep_u_8156(uint32_t u32Ch, uint32_t u32Sep)
{
	(void)NULL;
	return b8156_is_sep(u32Ch, u32Sep);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_is_sep_u_8156(uint32_t u32Ch, uint32_t u32Sep)
    __attribute__((alias("gj_strtok_is_sep_u_8156")));
