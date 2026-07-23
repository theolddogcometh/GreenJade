/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6766: bpf BPF_PROG_LOAD cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_prog_load_6766(void);
 *     - Returns 5 (soft BPF_PROG_LOAD bpf(2) command id). Pure-data
 *       catalog stub; not a live bpf syscall.
 *   uint32_t __gj_bpf_cmd_prog_load_6766  (alias)
 *   __libcgj_batch6766_marker = "libcgj-batch6766"
 *
 * Exclusive continuum CREATE-ONLY (6761-6770: bpf cmd stubs). Unique
 * gj_bpf_cmd_prog_load_6766 surface only; no multi-def.
 * Distinct from map_get_next_key (batch6765) and obj_pin (batch6767).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6766_marker[] = "libcgj-batch6766";

/* Soft BPF_PROG_LOAD (verify and load eBPF program). */
#define B6766_BPF_PROG_LOAD  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6766_prog_load(void)
{
	return B6766_BPF_PROG_LOAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_prog_load_6766 - soft BPF_PROG_LOAD cmd catalog id.
 *
 * Always returns 5. Pure-data stub for product bpf-cmd catalogs;
 * does not issue bpf(2) or load bytecode. No parent wires.
 */
uint32_t
gj_bpf_cmd_prog_load_6766(void)
{
	(void)NULL;
	return b6766_prog_load();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_prog_load_6766(void)
    __attribute__((alias("gj_bpf_cmd_prog_load_6766")));
