/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7213: poll POLLERR event bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_poll_err_id_7213(void);
 *     - Return soft POLLERR constant (0x008).
 *   uint32_t __gj_poll_err_id_7213  (alias)
 *   __libcgj_batch7213_marker = "libcgj-batch7213"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Unique gj_poll_err_id_7213 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7213_marker[] = "libcgj-batch7213";

/* POLLERR: error condition (revents only; 0x008). */
#define B7213_POLLERR ((uint32_t)0x008u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7213_poll_err_id(void)
{
	return B7213_POLLERR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poll_err_id_7213 - soft POLLERR event bit constant.
 *
 * Always returns 0x008 (POSIX/glibc POLLERR). Catalog id only; does not
 * call poll. No parent wires.
 */
uint32_t
gj_poll_err_id_7213(void)
{
	(void)NULL;
	return b7213_poll_err_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_poll_err_id_7213(void)
    __attribute__((alias("gj_poll_err_id_7213")));
