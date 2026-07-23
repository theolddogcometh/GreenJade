/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9943: KVM memory soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kvm_mem_ok_u_9943(void);
 *     - Return 0 (soft lamp: KVM guest memory region not claimed present
 *       for this freestanding continuum). Catalog only; not memslot I/O.
 *   uint32_t __gj_kvm_mem_ok_u_9943  (alias)
 *   __libcgj_batch9943_marker = "libcgj-batch9943"
 *
 * Exclusive continuum CREATE-ONLY (9941-9950: kvm soft id stubs —
 * kvm_ok_u, kvm_vcpu_ok_u, kvm_mem_ok_u, kvm_irq_ok_u, kvm_ioevent_ok_u,
 * kvm_run_ok_u, kvm_exit_ok_u, kvm_cap_ok_u, kvm_ready_u, batch_id_9950).
 * Unique gj_kvm_mem_ok_u_9943 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9943_marker[] = "libcgj-batch9943";

/* Soft KVM memory lamp (not claimed / soft-off). */
#define B9943_KVM_MEM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9943_mem_ok(void)
{
	return B9943_KVM_MEM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kvm_mem_ok_u_9943 - soft KVM guest-memory lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not set user memory
 * regions or issue KVM_SET_USER_MEMORY_REGION. No parent wires.
 */
uint32_t
gj_kvm_mem_ok_u_9943(void)
{
	(void)NULL;
	return b9943_mem_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kvm_mem_ok_u_9943(void)
    __attribute__((alias("gj_kvm_mem_ok_u_9943")));
