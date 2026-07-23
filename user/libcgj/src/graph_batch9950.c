/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9950: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9950(void);
 *     - Returns the compile-time graph batch number for this TU (9950).
 *   uint32_t __gj_batch_id_9950  (alias)
 *   __libcgj_batch9950_marker = "libcgj-batch9950"
 *
 * Exclusive continuum CREATE-ONLY (9941-9950: kvm soft id stubs —
 * kvm_ok_u, kvm_vcpu_ok_u, kvm_mem_ok_u, kvm_irq_ok_u, kvm_ioevent_ok_u,
 * kvm_run_ok_u, kvm_exit_ok_u, kvm_cap_ok_u, kvm_ready_u, batch_id_9950).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Soft pure-data catalog only; no KVM I/O.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9950_marker[] = "libcgj-batch9950";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9950_id(void)
{
	return 9950u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9950 - report this TU's graph batch number.
 *
 * Always returns 9950. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9950(void)
{
	(void)NULL;
	return b9950_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9950(void)
    __attribute__((alias("gj_batch_id_9950")));
