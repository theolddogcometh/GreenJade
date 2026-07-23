/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7211: poll POLLIN event bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_in_id_7211(void);
 *     - Return soft POLLIN constant (0x001).
 *   uint32_t __gj_poll_in_id_7211  (alias)
 *   __libcgj_batch7211_marker = "libcgj-batch7211"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_in_id_7211 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7211_marker[] = "libcgj-batch7211";

/* POLLIN: data other than high-priority is readable (0x001). */
#define B7211_POLLIN ((uint32_t)0x001u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7211_poll_in_id(void)
{
	return B7211_POLLIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_in_id_7211 - soft POLLIN event bit constant.
 *
 * Always returns 0x001 (POSIX/glibc POLLIN). Catalog id only; does not
 * call poll. No parent wires.
 */
uint32_t
gj_poll_in_id_7211(void)
{
	(void)NULL;
	return b7211_poll_in_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_in_id_7211(void)
    __attribute__((alias("gj_poll_in_id_7211")));
