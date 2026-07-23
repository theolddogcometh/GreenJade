/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9215: HTTP 403 Forbidden soft status id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_http_forbid_id_9215(void);
 *     - Returns 403 (HTTP Forbidden soft catalog value). Pure-data
 *       status-code id stub; does not open sockets or speak HTTP.
 *   uint32_t __gj_http_forbid_id_9215  (alias)
 *   __libcgj_batch9215_marker = "libcgj-batch9215"
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

const char __libcgj_batch9215_marker[] = "libcgj-batch9215";

/* Soft HTTP Forbidden status catalog id (403). */
#define B9215_HTTP_FORBID  403u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9215_http_forbid_id(void)
{
	return B9215_HTTP_FORBID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_forbid_id_9215 - soft HTTP 403 Forbidden status-code id.
 *
 * Always returns 403. Soft continuum constant; does not open sockets
 * or speak HTTP. No parent wires.
 */
uint32_t
gj_http_forbid_id_9215(void)
{
	(void)NULL;
	return b9215_http_forbid_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_http_forbid_id_9215(void)
    __attribute__((alias("gj_http_forbid_id_9215")));
