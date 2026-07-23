/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4960: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4960(void);
 *     - Returns the compile-time graph batch number for this TU (4960).
 *   uint32_t __gj_batch_id_4960  (alias)
 *   __libcgj_batch4960_marker = "libcgj-batch4960"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique —
 * content_appdata_u, content_handshake_u, content_alert_u,
 * content_ccs_u, is_appdata_u, rec_overhead_u, max_plain_u,
 * version_12_u, version_13_u, batch_id_4960). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4960_marker[] = "libcgj-batch4960";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4960_id(void)
{
	return 4960u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4960 - report this TU's graph batch number.
 *
 * Always returns 4960. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4960(void)
{
	(void)NULL;
	return b4960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4960(void)
    __attribute__((alias("gj_batch_id_4960")));
