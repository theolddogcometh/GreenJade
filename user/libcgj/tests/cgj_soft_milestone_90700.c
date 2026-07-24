/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90700 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90700.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90700 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90700 \
 *   user/libcgj/tests/cgj_soft_milestone_90700.c -ldl
 * Direct: -DCGJ_SOFT_M90700_DIRECT + graph_batch90691.c … graph_batch90700.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90700_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90700
#define CGJ_SOFT_B0 90691
#define CGJ_SOFT_B1 90692
#define CGJ_SOFT_B2 90693
#define CGJ_SOFT_B3 90694
#define CGJ_SOFT_B4 90695
#define CGJ_SOFT_B5 90696
#define CGJ_SOFT_B6 90697
#define CGJ_SOFT_B7 90698
#define CGJ_SOFT_B8 90699
#include "_cgj_soft_m_template.inc"
