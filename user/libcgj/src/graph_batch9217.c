/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9217: HTTP 500 Server Error soft status id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_http_server_err_id_9217(void);
 *     - Returns 500 (HTTP Internal Server Error soft catalog value).
 *       Pure-data status-code id stub; does not open sockets or speak
 *       HTTP.
 *   uint32_t __gj_http_server_err_id_9217  (alias)
 *   __libcgj_batch9217_marker = "libcgj-batch9217"
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

const char __libcgj_batch9217_marker[] = "libcgj-batch9217";

/* Soft HTTP Internal Server Error status catalog id (500). */
#define B9217_HTTP_SERVER_ERR  500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9217_http_server_err_id(void)
{
	return B9217_HTTP_SERVER_ERR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_server_err_id_9217 - soft HTTP 500 Server Error status-code id.
 *
 * Always returns 500. Soft continuum constant; does not open sockets
 * or speak HTTP. No parent wires.
 */
uint32_t
gj_http_server_err_id_9217(void)
{
	(void)NULL;
	return b9217_http_server_err_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_http_server_err_id_9217(void)
    __attribute__((alias("gj_http_server_err_id_9217")));
