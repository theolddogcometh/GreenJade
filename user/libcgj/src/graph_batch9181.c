/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9181: install root soft layout tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_root_tag_9181(void);
 *     - Returns the soft install-image root path layout tag for the
 *       real-hw install path soft continuum (always 0x49524f54, fourCC
 *       "IROT"). Not a filesystem path probe.
 *   uint32_t __gj_install_root_tag_9181  (alias)
 *   __libcgj_batch9181_marker = "libcgj-batch9181"
 *
 * Exclusive continuum CREATE-ONLY (9181-9190: install/image soft stubs —
 * install_root_tag_9181, install_efi_tag_9182, install_boot_tag_9183,
 * install_rootfs_tag_9184, install_partition_ok_u_9185,
 * install_write_ready_u_9186, install_verify_ready_u_9187,
 * install_reboot_ready_u_9188, install_soft_ready_u_9189,
 * batch_id_9190). Unique surface only; no multi-def. Distinct from
 * gj_install_prefix_abs_p (batch1954) and gj_install_media_ok
 * (batch2638). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9181_marker[] = "libcgj-batch9181";

/* Soft install root path fourCC "IROT". */
#define B9181_INSTALL_ROOT_TAG  0x49524f54u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9181_root_tag(void)
{
	return B9181_INSTALL_ROOT_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_root_tag_9181 - soft install root path layout fourCC tag.
 *
 * Always returns 0x49524f54 ("IROT"). Soft pure-data layout tag for the
 * 9181-9190 continuum. Does not probe install media or call libc.
 * No parent wires.
 */
uint32_t
gj_install_root_tag_9181(void)
{
	(void)NULL;
	return b9181_root_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_root_tag_9181(void)
    __attribute__((alias("gj_install_root_tag_9181")));
