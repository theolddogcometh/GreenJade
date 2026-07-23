/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5806: install media finalize code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_media_finalize_5806(void);
 *     - Returns the soft install-media finalize code tag for this
 *       continuum (always 0x494D4631, fourCC "IMF1"). Not a live
 *       image-write or finalize action.
 *   uint32_t __gj_install_media_finalize_5806  (alias)
 *   __libcgj_batch5806_marker = "libcgj-batch5806"
 *
 * Exclusive continuum CREATE-ONLY (5801-5810: product bar3 install
 * media finalize). Unique gj_install_media_finalize_5806 surface only;
 * no multi-def. Distinct from gj_steam_bootstrap_finalize_5816 and
 * gj_bar3_media_finalize_ok_5805. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5806_marker[] = "libcgj-batch5806";

/* Install media finalize fourCC "IMF1". */
#define B5806_FINALIZE_CODE  0x494D4631u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5806_finalize(void)
{
	return B5806_FINALIZE_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_media_finalize_5806 - install-media finalize code tag.
 *
 * Always returns 0x494D4631 ("IMF1"). Soft pure-data finalize tag for
 * the product bar3 install media continuum. Does not call libc or
 * finalize a live image. No parent wires.
 */
uint32_t
gj_install_media_finalize_5806(void)
{
	(void)NULL;
	return b5806_finalize();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_media_finalize_5806(void)
    __attribute__((alias("gj_install_media_finalize_5806")));
