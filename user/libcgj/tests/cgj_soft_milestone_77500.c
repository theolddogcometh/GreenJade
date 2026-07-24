/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77500 \
 *   user/libcgj/tests/cgj_soft_milestone_77500.c -ldl
 * Direct: -DCGJ_SOFT_M77500_DIRECT + graph_batch77491.c … graph_batch77500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77500
#define CGJ_SOFT_B0 77491
#define CGJ_SOFT_B1 77492
#define CGJ_SOFT_B2 77493
#define CGJ_SOFT_B3 77494
#define CGJ_SOFT_B4 77495
#define CGJ_SOFT_B5 77496
#define CGJ_SOFT_B6 77497
#define CGJ_SOFT_B7 77498
#define CGJ_SOFT_B8 77499
#include "_cgj_soft_m_template.inc"
