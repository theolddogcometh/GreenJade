/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1963: Vulkan sample-count flag soft ok.
 *
 * Surface (unique symbols):
 *   int gj_vk_sample_count_ok(uint32_t sc);
 *     — Return 1 if sc is a core power-of-two VkSampleCountFlagBits
 *       value in {1, 2, 4, 8, 16}, else 0. Soft MSAA policy hint;
 *       does not query physical-device limits (32/64 excluded here).
 *   int __gj_vk_sample_count_ok  (alias)
 *   __libcgj_batch1963_marker = "libcgj-batch1963"
 *
 * Vulkan/GL enum exclusive pure-data wave (1961–1970). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1963_marker[] = "libcgj-batch1963";

/*
 * VkSampleCountFlagBits (subset accepted by this soft gate):
 *   VK_SAMPLE_COUNT_1_BIT  = 0x01
 *   VK_SAMPLE_COUNT_2_BIT  = 0x02
 *   VK_SAMPLE_COUNT_4_BIT  = 0x04
 *   VK_SAMPLE_COUNT_8_BIT  = 0x08
 *   VK_SAMPLE_COUNT_16_BIT = 0x10
 */

/* ---- freestanding helpers ---------------------------------------------- */

/* Linear scan of the five accepted sample-count bit values. */
static int
b1963_sc_ok(uint32_t uSc)
{
	static const uint32_t s_aOk[] = {
		1u, 2u, 4u, 8u, 16u
	};
	size_t iSc;
	size_t cSc;

	cSc = sizeof(s_aOk) / sizeof(s_aOk[0]);
	for (iSc = 0u; iSc < cSc; iSc++) {
		if (s_aOk[iSc] == uSc) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vk_sample_count_ok — 1 if sc ∈ {1, 2, 4, 8, 16}.
 *
 * sc: VkSampleCountFlagBits as uint32_t (single bit expected)
 * Returns 1 for the five core MSAA counts, else 0.
 */
int
gj_vk_sample_count_ok(uint32_t uSc)
{
	(void)NULL;
	return b1963_sc_ok(uSc);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_vk_sample_count_ok(uint32_t uSc)
    __attribute__((alias("gj_vk_sample_count_ok")));
