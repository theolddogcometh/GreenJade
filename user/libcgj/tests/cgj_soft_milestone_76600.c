/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_76600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76600 \
 *   user/libcgj/tests/cgj_soft_milestone_76600.c -ldl
 * Direct: -DCGJ_SOFT_M76600_DIRECT + graph_batch76591.c … graph_batch76600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M76600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 76600
#define CGJ_SOFT_B0 76591
#define CGJ_SOFT_B1 76592
#define CGJ_SOFT_B2 76593
#define CGJ_SOFT_B3 76594
#define CGJ_SOFT_B4 76595
#define CGJ_SOFT_B5 76596
#define CGJ_SOFT_B6 76597
#define CGJ_SOFT_B7 76598
#define CGJ_SOFT_B8 76599
#include "_cgj_soft_m_template.inc"
