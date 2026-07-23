/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9252: soft curl HTTP GET method catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_curl_http_get_id_9252(void);
 *     - Returns 0x47455420 (fourCC "GET "). Soft HTTP GET method
 *       catalog id stub; does not issue network requests or call
 *       libcurl.
 *   uint32_t __gj_curl_http_get_id_9252  (alias)
 *   __libcgj_batch9252_marker = "libcgj-batch9252"
 *
 * Exclusive continuum CREATE-ONLY (9251-9260: curl soft id stubs —
 * curl_ok_id_9251, curl_http_get_id_9252, curl_http_post_id_9253,
 * curl_timeout_ok_u_9254, curl_url_ok_u_9255, curl_header_ok_u_9256,
 * curl_body_ok_u_9257, curl_ssl_ok_u_9258, curl_ready_u_9259,
 * batch_id_9260). Unique surface only; no multi-def. No parent wires.
 * No __int128. No curl/libcurl implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9252_marker[] = "libcgj-batch9252";

/* Soft HTTP GET method fourCC "GET ". */
#define B9252_HTTP_GET_ID  0x47455420u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9252_http_get_id(void)
{
	return B9252_HTTP_GET_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_curl_http_get_id_9252 - soft curl HTTP GET method catalog id.
 *
 * Always returns 0x47455420 ("GET "). Soft continuum constant; does not
 * call libcurl or open sockets. No parent wires.
 */
uint32_t
gj_curl_http_get_id_9252(void)
{
	(void)NULL;
	return b9252_http_get_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_curl_http_get_id_9252(void)
    __attribute__((alias("gj_curl_http_get_id_9252")));
