/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14040: soft dbus/systemd path gate batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14040(void);
 *     - Returns the compile-time graph batch number for this TU (14040).
 *   uint32_t __gj_batch_id_14040  (alias)
 *   __libcgj_batch14040_marker = "libcgj-batch14040"
 *
 * Exclusive continuum CREATE-ONLY (14031-14040: soft dbus/systemd path
 * gates — dbus_path_gate_u_14031, systemd_path_gate_u_14032,
 * logind_path_gate_u_14033, journal_path_gate_u_14034,
 * resolved_path_gate_u_14035, networkd_path_gate_u_14036,
 * timedated_path_gate_u_14037, hostnamed_path_gate_u_14038,
 * dbus_path_soft_ready_u_14039, batch_id_14040). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_13950 / gj_batch_id_14000. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14040_marker[] = "libcgj-batch14040";

#define B14040_BATCH_ID  14040u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14040_id(void)
{
	return B14040_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14040 - report this TU's graph batch number.
 *
 * Always returns 14040. Closes the 14031-14040 soft dbus/systemd path
 * gate continuum. No parent wires.
 */
uint32_t
gj_batch_id_14040(void)
{
	(void)NULL;
	return b14040_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14040(void)
    __attribute__((alias("gj_batch_id_14040")));
