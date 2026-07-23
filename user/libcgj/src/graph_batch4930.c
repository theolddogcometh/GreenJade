/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4930: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4930(void);
 *     - Returns the compile-time graph batch number for this TU (4930).
 *   uint32_t __gj_batch_id_4930  (alias)
 *   __libcgj_batch4930_marker = "libcgj-batch4930"
 *
 * Exclusive continuum CREATE-ONLY (4921-4930: http_is_1xx_u, http_is_2xx_u,
 * http_is_3xx_u, http_is_4xx_u, http_is_5xx_u, http_is_success_u,
 * http_is_error_u, http_is_redirect_u, http_class_u, batch_id_4930).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4930_marker[] = "libcgj-batch4930";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4930_id(void)
{
	return 4930u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4930 - report this TU's graph batch number.
 *
 * Always returns 4930. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4930(void)
{
	(void)NULL;
	return b4930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4930(void)
    __attribute__((alias("gj_batch_id_4930")));
