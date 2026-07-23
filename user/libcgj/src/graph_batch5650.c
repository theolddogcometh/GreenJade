/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5650: installer overall progress + milestone.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_overall_progress_5650(uint32_t part_pct,
 *       uint32_t fmt_pct, uint32_t copy_pct, uint32_t boot_pct,
 *       uint32_t cfg_pct);
 *     - Weighted overall install progress 0..100 using stage weights
 *       15/15/40/15/15 (partition/format/copy/bootloader/config).
 *       Each input pct is clamped to 0..100 first.
 *   uint32_t gj_install_weights_sum_5650(void);
 *     - Returns 100 (sum of soft stage weights for this wave).
 *   uint32_t gj_batch_id_5650(void);
 *     - Returns 5650 (this TU's graph batch number).
 *   uint32_t gj_graph_milestone_5650(void);
 *     - Returns 5650 (current graph milestone revision).
 *   uint32_t __gj_install_overall_progress_5650  (alias)
 *   uint32_t __gj_install_weights_sum_5650  (alias)
 *   uint32_t __gj_batch_id_5650  (alias)
 *   uint32_t __gj_graph_milestone_5650  (alias)
 *   __libcgj_batch5650_marker = "libcgj-batch5650"
 *
 * MILESTONE 5650 for the exclusive continuum CREATE-ONLY wave
 * (batches 5641-5649: partition/format/copy/bootloader/config stage
 * identity and progress integers). Does NOT redefine bare gj_batch_id
 * / gj_graph_milestone. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5650_marker[] = "libcgj-batch5650";

/* Soft stage weights matching 5641/5643/5645/5647/5649. */
#define B5650_W_PART  15u
#define B5650_W_FMT   15u
#define B5650_W_COPY  40u
#define B5650_W_BOOT  15u
#define B5650_W_CFG   15u
#define B5650_W_SUM   100u
#define B5650_BATCH   5650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5650_clamp_pct(uint32_t u32Pct)
{
	if (u32Pct > 100u) {
		return 100u;
	}
	return u32Pct;
}

static uint32_t
b5650_overall(uint32_t u32Part, uint32_t u32Fmt, uint32_t u32Copy,
    uint32_t u32Boot, uint32_t u32Cfg)
{
	uint32_t u32Acc;

	u32Part = b5650_clamp_pct(u32Part);
	u32Fmt = b5650_clamp_pct(u32Fmt);
	u32Copy = b5650_clamp_pct(u32Copy);
	u32Boot = b5650_clamp_pct(u32Boot);
	u32Cfg = b5650_clamp_pct(u32Cfg);

	/*
	 * Weighted average:
	 *   (p*15 + f*15 + c*40 + b*15 + k*15) / 100
	 * Max product per term is 100*40=4000; sum max 10000 — fits u32.
	 */
	u32Acc = (u32Part * B5650_W_PART)
	    + (u32Fmt * B5650_W_FMT)
	    + (u32Copy * B5650_W_COPY)
	    + (u32Boot * B5650_W_BOOT)
	    + (u32Cfg * B5650_W_CFG);
	return u32Acc / B5650_W_SUM;
}

static uint32_t
b5650_id(void)
{
	return B5650_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_overall_progress_5650 - weighted overall install progress.
 *
 * part_pct: partition stage 0..100 (clamped)
 * fmt_pct:  format stage 0..100 (clamped)
 * copy_pct: copy stage 0..100 (clamped)
 * boot_pct: bootloader stage 0..100 (clamped)
 * cfg_pct:  config stage 0..100 (clamped)
 *
 * Returns weighted percent 0..100 using weights 15/15/40/15/15.
 * Soft pure integer; does not probe disks. No parent wires.
 */
uint32_t
gj_install_overall_progress_5650(uint32_t u32PartPct, uint32_t u32FmtPct,
    uint32_t u32CopyPct, uint32_t u32BootPct, uint32_t u32CfgPct)
{
	(void)NULL;
	return b5650_overall(u32PartPct, u32FmtPct, u32CopyPct, u32BootPct,
	    u32CfgPct);
}

/*
 * gj_install_weights_sum_5650 - sum of soft installer stage weights.
 *
 * Always returns 100.
 */
uint32_t
gj_install_weights_sum_5650(void)
{
	return B5650_W_SUM;
}

/*
 * gj_batch_id_5650 - report this TU's graph batch number.
 *
 * Always returns 5650.
 */
uint32_t
gj_batch_id_5650(void)
{
	return b5650_id();
}

/*
 * gj_graph_milestone_5650 - report this TU's graph milestone revision.
 *
 * Always returns 5650 (MILESTONE 5650). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5650(void)
{
	return b5650_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_overall_progress_5650(uint32_t u32PartPct,
    uint32_t u32FmtPct, uint32_t u32CopyPct, uint32_t u32BootPct,
    uint32_t u32CfgPct)
    __attribute__((alias("gj_install_overall_progress_5650")));

uint32_t __gj_install_weights_sum_5650(void)
    __attribute__((alias("gj_install_weights_sum_5650")));

uint32_t __gj_batch_id_5650(void)
    __attribute__((alias("gj_batch_id_5650")));

uint32_t __gj_graph_milestone_5650(void)
    __attribute__((alias("gj_graph_milestone_5650")));
