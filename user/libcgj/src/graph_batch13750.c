/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13750: dbus soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13750(void);
 *     - Returns the compile-time graph batch number for this TU (13750).
 *   uint32_t __gj_batch_id_13750  (alias)
 *   __libcgj_batch13750_marker = "libcgj-batch13750"
 *
 * Exclusive continuum CREATE-ONLY (13741-13750: dbus soft id stubs —
 * dbus_ok_u_13741, systemd_ok_u_13742, logind_ok_u_13743,
 * journal_ok_u_13744, resolved_ok_u_13745, networkd_ok_u_13746,
 * timedated_ok_u_13747, hostnamed_ok_u_13748,
 * dbus_soft_ready_u_13749, batch_id_13750). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10850 / gj_batch_id_10650 / gj_batch_id_10450 /
 * gj_batch_id_10150. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13750_marker[] = "libcgj-batch13750";

#define B13750_BATCH_ID  13750u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13750_id(void)
{
	return B13750_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13750 - report this TU's graph batch number.
 *
 * Always returns 13750.
 */
uint32_t
gj_batch_id_13750(void)
{
	(void)NULL;
	return b13750_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13750(void)
    __attribute__((alias("gj_batch_id_13750")));
