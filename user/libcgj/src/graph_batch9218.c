/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9218: HTTP method-ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_http_method_ok_u_9218(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HTTP method allowlist probe for the status soft-id continuum.
 *   uint32_t __gj_http_method_ok_u_9218  (alias)
 *   __libcgj_batch9218_marker = "libcgj-batch9218"
 *
 * Exclusive continuum CREATE-ONLY (9211-9220: http status soft id
 * stubs — http_ok_id_9211, http_created_id_9212, http_bad_id_9213,
 * http_unauth_id_9214, http_forbid_id_9215, http_notfound_id_9216,
 * http_server_err_id_9217, http_method_ok_u_9218, http_ready_u_9219,
 * batch_id_9220). Unique surface only; no multi-def. No parent wires.
 * No __int128. No HTTP implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9218_marker[] = "libcgj-batch9218";

/* Soft method-ok lamp: always off (not a real method probe). */
#define B9218_METHOD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9218_method_ok(void)
{
	return B9218_METHOD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_method_ok_u_9218 - HTTP method-ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not parse methods or
 * call libc. No parent wires.
 */
uint32_t
gj_http_method_ok_u_9218(void)
{
	(void)NULL;
	return b9218_method_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_http_method_ok_u_9218(void)
    __attribute__((alias("gj_http_method_ok_u_9218")));
