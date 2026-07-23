/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14035: resolved soft path gate (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resolved_path_gate_u_14035(void);
 *     - Always returns 0. Soft path gate only; intentionally not a real
 *       systemd-resolved runtime or stub-resolver path probe.
 *   uint32_t __gj_resolved_path_gate_u_14035  (alias)
 *   __libcgj_batch14035_marker = "libcgj-batch14035"
 *
 * Exclusive continuum CREATE-ONLY (14031-14040: soft dbus/systemd path
 * gates — dbus_path_gate_u_14031, systemd_path_gate_u_14032,
 * logind_path_gate_u_14033, journal_path_gate_u_14034,
 * resolved_path_gate_u_14035, networkd_path_gate_u_14036,
 * timedated_path_gate_u_14037, hostnamed_path_gate_u_14038,
 * dbus_path_soft_ready_u_14039, batch_id_14040). Unique surface only;
 * no multi-def. Distinct from gj_resolved_ok_u_13945. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14035_marker[] = "libcgj-batch14035";

/* Soft resolved path gate: always closed (not a real runtime probe). */
#define B14035_RESOLVED_PATH_GATE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14035_gate(void)
{
	return B14035_RESOLVED_PATH_GATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resolved_path_gate_u_14035 - resolved soft path gate flag.
 *
 * Always returns 0. Soft continuum stub; does not probe
 * systemd-resolved, /run/systemd/resolve, or call libc. No parent wires.
 */
uint32_t
gj_resolved_path_gate_u_14035(void)
{
	(void)NULL;
	return b14035_gate();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resolved_path_gate_u_14035(void)
    __attribute__((alias("gj_resolved_path_gate_u_14035")));
