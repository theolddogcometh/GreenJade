/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14033: logind soft path gate (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_logind_path_gate_u_14033(void);
 *     - Always returns 0. Soft path gate only; intentionally not a real
 *       systemd-logind session/seat runtime probe.
 *   uint32_t __gj_logind_path_gate_u_14033  (alias)
 *   __libcgj_batch14033_marker = "libcgj-batch14033"
 *
 * Exclusive continuum CREATE-ONLY (14031-14040: soft dbus/systemd path
 * gates — dbus_path_gate_u_14031, systemd_path_gate_u_14032,
 * logind_path_gate_u_14033, journal_path_gate_u_14034,
 * resolved_path_gate_u_14035, networkd_path_gate_u_14036,
 * timedated_path_gate_u_14037, hostnamed_path_gate_u_14038,
 * dbus_path_soft_ready_u_14039, batch_id_14040). Unique surface only;
 * no multi-def. Distinct from gj_logind_ok_u_13943 /
 * gj_logind_stub_present_5781. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14033_marker[] = "libcgj-batch14033";

/* Soft logind path gate: always closed (not a real runtime probe). */
#define B14033_LOGIND_PATH_GATE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14033_gate(void)
{
	return B14033_LOGIND_PATH_GATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_logind_path_gate_u_14033 - logind soft path gate flag.
 *
 * Always returns 0. Soft continuum stub; does not probe logind seats,
 * /run/systemd/sessions, or call libc. No parent wires.
 */
uint32_t
gj_logind_path_gate_u_14033(void)
{
	(void)NULL;
	return b14033_gate();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_logind_path_gate_u_14033(void)
    __attribute__((alias("gj_logind_path_gate_u_14033")));
