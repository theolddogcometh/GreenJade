/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9216: HTTP 404 Not Found soft status id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_http_notfound_id_9216(void);
 *     - Returns 404 (HTTP Not Found soft catalog value). Pure-data
 *       status-code id stub; does not open sockets or speak HTTP.
 *   uint32_t __gj_http_notfound_id_9216  (alias)
 *   __libcgj_batch9216_marker = "libcgj-batch9216"
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

const char __libcgj_batch9216_marker[] = "libcgj-batch9216";

/* Soft HTTP Not Found status catalog id (404). */
#define B9216_HTTP_NOTFOUND  404u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9216_http_notfound_id(void)
{
	return B9216_HTTP_NOTFOUND;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_notfound_id_9216 - soft HTTP 404 Not Found status-code id.
 *
 * Always returns 404. Soft continuum constant; does not open sockets
 * or speak HTTP. No parent wires.
 */
uint32_t
gj_http_notfound_id_9216(void)
{
	(void)NULL;
	return b9216_http_notfound_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_http_notfound_id_9216(void)
    __attribute__((alias("gj_http_notfound_id_9216")));
