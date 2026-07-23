/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9123: soft XML text-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xml_text_ok_u_9123(void);
 *     - Returns 1 (XML text soft-id continuum ok). Pure-data product
 *       tag; does not parse or emit XML character data.
 *   uint32_t __gj_xml_text_ok_u_9123  (alias)
 *   __libcgj_batch9123_marker = "libcgj-batch9123"
 *
 * Exclusive continuum CREATE-ONLY (9121-9130: xml soft id stubs —
 * xml_element_ok_u_9121, xml_attr_ok_u_9122, xml_text_ok_u_9123,
 * xml_comment_ok_u_9124, xml_cdata_ok_u_9125, xml_pi_ok_u_9126,
 * xml_ns_ok_u_9127, xml_depth_ok_u_9128, xml_ready_u_9129,
 * batch_id_9130). Unique surface only; no multi-def. No parent wires.
 * No __int128. No XML parser implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9123_marker[] = "libcgj-batch9123";

#define B9123_TEXT_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9123_text_ok(void)
{
	return B9123_TEXT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_text_ok_u_9123 - XML text soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not parse text or
 * call libc. No parent wires.
 */
uint32_t
gj_xml_text_ok_u_9123(void)
{
	(void)NULL;
	return b9123_text_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xml_text_ok_u_9123(void)
    __attribute__((alias("gj_xml_text_ok_u_9123")));
