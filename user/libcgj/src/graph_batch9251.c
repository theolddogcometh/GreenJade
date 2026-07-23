/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9251: soft curl ok catalog id (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_curl_ok_id_9251(void);
 *     - Always returns 0. Soft curl transport ok catalog id stub;
 *       intentionally not a real libcurl/network probe.
 *   uint32_t __gj_curl_ok_id_9251  (alias)
 *   __libcgj_batch9251_marker = "libcgj-batch9251"
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

const char __libcgj_batch9251_marker[] = "libcgj-batch9251";

/* Soft curl ok catalog id: always off (not a real transport probe). */
#define B9251_CURL_OK_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9251_curl_ok_id(void)
{
	return B9251_CURL_OK_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_curl_ok_id_9251 - soft curl transport ok catalog id.
 *
 * Always returns 0. Soft continuum constant; does not call libcurl or
 * open sockets. No parent wires.
 */
uint32_t
gj_curl_ok_id_9251(void)
{
	(void)NULL;
	return b9251_curl_ok_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_curl_ok_id_9251(void)
    __attribute__((alias("gj_curl_ok_id_9251")));
