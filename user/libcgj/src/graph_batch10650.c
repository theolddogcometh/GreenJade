/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10650: dbus soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10650(void);
 *     - Returns the compile-time graph batch number for this TU (10650).
 *   uint32_t __gj_batch_id_10650  (alias)
 *   __libcgj_batch10650_marker = "libcgj-batch10650"
 *
 * Exclusive continuum CREATE-ONLY (10641-10650: dbus soft id stubs —
 * dbus_ok_u_10641, systemd_ok_u_10642, logind_ok_u_10643,
 * journal_ok_u_10644, resolved_ok_u_10645, networkd_ok_u_10646,
 * timedated_ok_u_10647, hostnamed_ok_u_10648,
 * dbus_soft_ready_u_10649, batch_id_10650). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10450 / gj_batch_id_10150. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10650_marker[] = "libcgj-batch10650";

#define B10650_BATCH_ID  10650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10650_id(void)
{
	return B10650_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10650 - report this TU's graph batch number.
 *
 * Always returns 10650.
 */
uint32_t
gj_batch_id_10650(void)
{
	(void)NULL;
	return b10650_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10650(void)
    __attribute__((alias("gj_batch_id_10650")));
