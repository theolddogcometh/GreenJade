/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6765: bpf BPF_MAP_GET_NEXT_KEY cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_map_get_next_key_6765(void);
 *     - Returns 4 (soft BPF_MAP_GET_NEXT_KEY bpf(2) command id).
 *       Pure-data catalog stub; not a live bpf syscall.
 *   uint32_t __gj_bpf_cmd_map_get_next_key_6765  (alias)
 *   __libcgj_batch6765_marker = "libcgj-batch6765"
 *
 * Exclusive continuum CREATE-ONLY (6761-6770: bpf cmd stubs). Unique
 * gj_bpf_cmd_map_get_next_key_6765 surface only; no multi-def.
 * Distinct from delete (batch6764) and prog_load (batch6766).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6765_marker[] = "libcgj-batch6765";

/* Soft BPF_MAP_GET_NEXT_KEY (iterate map keys). */
#define B6765_BPF_MAP_GET_NEXT_KEY  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6765_map_get_next_key(void)
{
	return B6765_BPF_MAP_GET_NEXT_KEY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_map_get_next_key_6765 - soft BPF_MAP_GET_NEXT_KEY cmd id.
 *
 * Always returns 4. Pure-data stub for product bpf-cmd catalogs;
 * does not issue bpf(2) or touch map fds. No parent wires.
 */
uint32_t
gj_bpf_cmd_map_get_next_key_6765(void)
{
	(void)NULL;
	return b6765_map_get_next_key();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_map_get_next_key_6765(void)
    __attribute__((alias("gj_bpf_cmd_map_get_next_key_6765")));
