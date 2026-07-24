/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90500 \
 *   user/libcgj/tests/cgj_soft_milestone_90500.c -ldl
 * Direct: -DCGJ_SOFT_M90500_DIRECT + graph_batch90491.c … graph_batch90500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90500
#define CGJ_SOFT_B0 90491
#define CGJ_SOFT_B1 90492
#define CGJ_SOFT_B2 90493
#define CGJ_SOFT_B3 90494
#define CGJ_SOFT_B4 90495
#define CGJ_SOFT_B5 90496
#define CGJ_SOFT_B6 90497
#define CGJ_SOFT_B7 90498
#define CGJ_SOFT_B8 90499
#include "_cgj_soft_m_template.inc"
