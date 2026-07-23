/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6769: bpf cmd catalog range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_ok_6769(uint32_t cmd);
 *     - Returns 1 when cmd is in soft bpf cmd catalog range
 *       [0, 8) (MAP_CREATE..OBJ_GET exclusive of later cmds), else 0.
 *       Pure-data range stub; not a live bpf validation.
 *   uint32_t __gj_bpf_cmd_ok_6769  (alias)
 *   __libcgj_batch6769_marker = "libcgj-batch6769"
 *
 * Exclusive continuum CREATE-ONLY (6761-6770: bpf cmd stubs —
 * map_create_6761, map_lookup_elem_6762, map_update_elem_6763,
 * map_delete_elem_6764, map_get_next_key_6765, prog_load_6766,
 * obj_pin_6767, obj_get_6768, cmd_ok_6769, continuum + batch_id_6770).
 * Unique surface only; no multi-def. Soft bound covers the eight
 * early bpf(2) commands stubbed in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6769_marker[] = "libcgj-batch6769";

/* Soft exclusive ceiling (BPF_MAP_CREATE..BPF_OBJ_GET). */
#define B6769_BPF_CMD_MAX  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6769_cmd_ok(uint32_t uCmd)
{
	return (uCmd < B6769_BPF_CMD_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_ok_6769 - soft bpf cmd catalog membership.
 *
 * cmd: candidate bpf(2) command id.
 * Returns 1 if cmd is in [0, 8), else 0. Does not query the kernel.
 * No parent wires.
 */
uint32_t
gj_bpf_cmd_ok_6769(uint32_t cmd)
{
	(void)NULL;
	return b6769_cmd_ok(cmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_ok_6769(uint32_t cmd)
    __attribute__((alias("gj_bpf_cmd_ok_6769")));
