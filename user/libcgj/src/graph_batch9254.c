/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9254: curl timeout ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_curl_timeout_ok_u_9254(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       curl timeout/option probe for the curl soft-id continuum.
 *   uint32_t __gj_curl_timeout_ok_u_9254  (alias)
 *   __libcgj_batch9254_marker = "libcgj-batch9254"
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

const char __libcgj_batch9254_marker[] = "libcgj-batch9254";

/* Soft timeout-ok lamp: always off (not a real timeout probe). */
#define B9254_TIMEOUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9254_timeout_ok(void)
{
	return B9254_TIMEOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_curl_timeout_ok_u_9254 - curl timeout ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not configure curl
 * timeouts or call libc. No parent wires.
 */
uint32_t
gj_curl_timeout_ok_u_9254(void)
{
	(void)NULL;
	return b9254_timeout_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_curl_timeout_ok_u_9254(void)
    __attribute__((alias("gj_curl_timeout_ok_u_9254")));
