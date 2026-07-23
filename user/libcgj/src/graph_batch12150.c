/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12150: exclusive dbus/systemd soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12150(void);
 *     - Returns the compile-time graph batch number for this TU (12150).
 *   uint32_t __gj_batch_id_12150  (alias)
 *   __libcgj_batch12150_marker = "libcgj-batch12150"
 *
 * Exclusive continuum CREATE-ONLY (12141-12150: dbus/systemd soft id
 * stubs). Unique gj_batch_id_12150 surface only; no multi-def. Does
 * NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12150_marker[] = "libcgj-batch12150";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12150_id(void)
{
	return 12150u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12150 - report this TU's graph batch number.
 *
 * Always returns 12150. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12150(void)
{
	(void)NULL;
	return b12150_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12150(void)
    __attribute__((alias("gj_batch_id_12150")));
