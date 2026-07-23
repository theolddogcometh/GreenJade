/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7060: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7060(void);
 *     - Returns the compile-time graph batch number for this TU (7060).
 *   uint32_t __gj_batch_id_7060  (alias)
 *   __libcgj_batch7060_marker = "libcgj-batch7060"
 *
 * Exclusive continuum CREATE-ONLY (7051-7060: keyctl op stubs —
 * get_keyring_id, join_session, update, revoke, chown, setperm,
 * describe, clear, op_ok, batch_id_7060). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7060_marker[] = "libcgj-batch7060";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7060_id(void)
{
	return 7060u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7060 - report this TU's graph batch number.
 *
 * Always returns 7060. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7060(void)
{
	(void)NULL;
	return b7060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7060(void)
    __attribute__((alias("gj_batch_id_7060")));
