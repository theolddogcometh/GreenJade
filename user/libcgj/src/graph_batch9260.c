/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9260: curl soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9260(void);
 *     - Returns the compile-time graph batch number for this TU (9260).
 *   uint32_t __gj_batch_id_9260  (alias)
 *   __libcgj_batch9260_marker = "libcgj-batch9260"
 *
 * Exclusive continuum CREATE-ONLY (9251-9260: curl soft id stubs —
 * curl_ok_id_9251, curl_http_get_id_9252, curl_http_post_id_9253,
 * curl_timeout_ok_u_9254, curl_url_ok_u_9255, curl_header_ok_u_9256,
 * curl_body_ok_u_9257, curl_ssl_ok_u_9258, curl_ready_u_9259,
 * batch_id_9260). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128. No curl/libcurl implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9260_marker[] = "libcgj-batch9260";

#define B9260_BATCH_ID  9260u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9260_id(void)
{
	return B9260_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9260 - report this TU's graph batch number.
 *
 * Always returns 9260.
 */
uint32_t
gj_batch_id_9260(void)
{
	(void)NULL;
	return b9260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9260(void)
    __attribute__((alias("gj_batch_id_9260")));
