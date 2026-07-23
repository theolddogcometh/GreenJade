/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9121: soft XML element-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xml_element_ok_u_9121(void);
 *     - Returns 1 (XML element soft-id continuum ok). Pure-data product
 *       tag; does not parse or emit XML elements.
 *   uint32_t __gj_xml_element_ok_u_9121  (alias)
 *   __libcgj_batch9121_marker = "libcgj-batch9121"
 *
 * Exclusive continuum CREATE-ONLY (9121-9130: xml soft id stubs —
 * xml_element_ok_u_9121, xml_attr_ok_u_9122, xml_text_ok_u_9123,
 * xml_comment_ok_u_9124, xml_cdata_ok_u_9125, xml_pi_ok_u_9126,
 * xml_ns_ok_u_9127, xml_depth_ok_u_9128, xml_ready_u_9129,
 * batch_id_9130). Unique surface only; no multi-def. Distinct from
 * gj_xml_is_name_start (batch1236). No parent wires. No __int128.
 * No XML parser implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9121_marker[] = "libcgj-batch9121";

#define B9121_ELEMENT_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9121_element_ok(void)
{
	return B9121_ELEMENT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_element_ok_u_9121 - XML element soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not parse elements
 * or call libc. No parent wires.
 */
uint32_t
gj_xml_element_ok_u_9121(void)
{
	(void)NULL;
	return b9121_element_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xml_element_ok_u_9121(void)
    __attribute__((alias("gj_xml_element_ok_u_9121")));
