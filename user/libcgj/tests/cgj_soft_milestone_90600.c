/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90600 \
 *   user/libcgj/tests/cgj_soft_milestone_90600.c -ldl
 * Direct: -DCGJ_SOFT_M90600_DIRECT + graph_batch90591.c … graph_batch90600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90600
#define CGJ_SOFT_B0 90591
#define CGJ_SOFT_B1 90592
#define CGJ_SOFT_B2 90593
#define CGJ_SOFT_B3 90594
#define CGJ_SOFT_B4 90595
#define CGJ_SOFT_B5 90596
#define CGJ_SOFT_B6 90597
#define CGJ_SOFT_B7 90598
#define CGJ_SOFT_B8 90599
#include "_cgj_soft_m_template.inc"
