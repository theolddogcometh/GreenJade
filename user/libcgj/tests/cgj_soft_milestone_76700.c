/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76700 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_76700.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76700 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76700 \
 *   user/libcgj/tests/cgj_soft_milestone_76700.c -ldl
 * Direct: -DCGJ_SOFT_M76700_DIRECT + graph_batch76691.c … graph_batch76700.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M76700_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 76700
#define CGJ_SOFT_B0 76691
#define CGJ_SOFT_B1 76692
#define CGJ_SOFT_B2 76693
#define CGJ_SOFT_B3 76694
#define CGJ_SOFT_B4 76695
#define CGJ_SOFT_B5 76696
#define CGJ_SOFT_B6 76697
#define CGJ_SOFT_B7 76698
#define CGJ_SOFT_B8 76699
#include "_cgj_soft_m_template.inc"
