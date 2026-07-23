/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5739: NVMe namespace id soft bounds.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_nvme_nsid_ok_5739(uint32_t nsid);
 *     - Soft NSID gate: 1..256 accepted (0 and >256 rejected). Matches
 *       common single-namespace Deck SSD layout expectations.
 *   uint32_t gj_pci_nvme_nsid_max_5739(void);
 *     - Returns soft max NSID (256).
 *   uint32_t __gj_pci_nvme_nsid_ok_5739  (alias)
 *   uint32_t __gj_pci_nvme_nsid_max_5739  (alias)
 *   __libcgj_batch5739_marker = "libcgj-batch5739"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique *_5739 surfaces only; no
 * multi-def. Distinct from gj_pci_nvme_class_ok_5738. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5739_marker[] = "libcgj-batch5739";

/* Soft NSID range for enum/install stubs (not a CAP read). */
#define B5739_NSID_MIN  1u
#define B5739_NSID_MAX  256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5739_nsid_max(void)
{
	return B5739_NSID_MAX;
}

static uint32_t
b5739_nsid_ok(uint32_t u32Nsid)
{
	if (u32Nsid < B5739_NSID_MIN) {
		return 0u;
	}
	if (u32Nsid > B5739_NSID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_nvme_nsid_max_5739 - soft max namespace id.
 *
 * Always returns 256. Pure-data bound only.
 */
uint32_t
gj_pci_nvme_nsid_max_5739(void)
{
	(void)NULL;
	return b5739_nsid_max();
}

/*
 * gj_pci_nvme_nsid_ok_5739 - validate soft NVMe namespace id.
 *
 * nsid: namespace identifier (1..256 accepted)
 * Returns 1 in range, else 0. Does not issue Identify. No parent wires.
 */
uint32_t
gj_pci_nvme_nsid_ok_5739(uint32_t u32Nsid)
{
	return b5739_nsid_ok(u32Nsid);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_nvme_nsid_max_5739(void)
    __attribute__((alias("gj_pci_nvme_nsid_max_5739")));

uint32_t __gj_pci_nvme_nsid_ok_5739(uint32_t u32Nsid)
    __attribute__((alias("gj_pci_nvme_nsid_ok_5739")));
