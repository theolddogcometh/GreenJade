/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5740: Deck PCI enum readiness gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_deck_enum_ready_5740(uint32_t apu_ok, uint32_t usb_ok,
 *                                        uint32_t nvme_ok);
 *     - Combined soft gate: all three non-zero → 1 (Deck APU root,
 *       USB xHCI, and NVMe class matches present in an enum snapshot).
 *   uint32_t gj_batch_id_5740(void);
 *     - Returns 5740 (wave end marker for 5731-5740 continuum).
 *   uint32_t __gj_pci_deck_enum_ready_5740  (alias)
 *   uint32_t __gj_batch_id_5740  (alias)
 *   __libcgj_batch5740_marker = "libcgj-batch5740"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe — pci_bdf_pack_5731, cfg_hdr_off_ok_5732,
 * class_host_bridge_5733, deck_apu_vendor_5734, deck_gpu_match_5735,
 * usb_xhci_class_5736, usb_hc_slot_5737, nvme_class_5738,
 * nvme_nsid_5739, deck_enum_ready_5740). Unique *_5740 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5740_marker[] = "libcgj-batch5740";

#define B5740_BATCH_ID  5740u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5740_enum_ready(uint32_t u32ApuOk, uint32_t u32UsbOk, uint32_t u32NvmeOk)
{
	if (u32ApuOk == 0u) {
		return 0u;
	}
	if (u32UsbOk == 0u) {
		return 0u;
	}
	if (u32NvmeOk == 0u) {
		return 0u;
	}
	return 1u;
}

static uint32_t
b5740_id(void)
{
	return B5740_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_deck_enum_ready_5740 - combined Deck PCI enum readiness gate.
 *
 * apu_ok:  non-zero if APU host/vendor match soft-passed
 * usb_ok:  non-zero if USB xHCI class match soft-passed
 * nvme_ok: non-zero if NVMe class match soft-passed
 *
 * Returns 1 only when all three lamps are non-zero, else 0. Pure
 * integer combine; does not probe hardware. No parent wires.
 */
uint32_t
gj_pci_deck_enum_ready_5740(uint32_t u32ApuOk, uint32_t u32UsbOk,
    uint32_t u32NvmeOk)
{
	(void)NULL;
	return b5740_enum_ready(u32ApuOk, u32UsbOk, u32NvmeOk);
}

/*
 * gj_batch_id_5740 - report this TU's graph batch number.
 *
 * Always returns 5740.
 */
uint32_t
gj_batch_id_5740(void)
{
	return b5740_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_deck_enum_ready_5740(uint32_t u32ApuOk, uint32_t u32UsbOk,
    uint32_t u32NvmeOk)
    __attribute__((alias("gj_pci_deck_enum_ready_5740")));

uint32_t __gj_batch_id_5740(void)
    __attribute__((alias("gj_batch_id_5740")));
