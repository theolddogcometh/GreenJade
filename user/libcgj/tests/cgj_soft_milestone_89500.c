/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89500 \
 *   user/libcgj/tests/cgj_soft_milestone_89500.c -ldl
 * Direct: -DCGJ_SOFT_M89500_DIRECT + graph_batch89491.c … graph_batch89500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89500
#define CGJ_SOFT_B0 89491
#define CGJ_SOFT_B1 89492
#define CGJ_SOFT_B2 89493
#define CGJ_SOFT_B3 89494
#define CGJ_SOFT_B4 89495
#define CGJ_SOFT_B5 89496
#define CGJ_SOFT_B6 89497
#define CGJ_SOFT_B7 89498
#define CGJ_SOFT_B8 89499
#include "_cgj_soft_m_template.inc"
