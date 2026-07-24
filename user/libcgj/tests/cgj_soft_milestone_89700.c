/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89700 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89700.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89700 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89700 \
 *   user/libcgj/tests/cgj_soft_milestone_89700.c -ldl
 * Direct: -DCGJ_SOFT_M89700_DIRECT + graph_batch89691.c … graph_batch89700.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89700_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89700
#define CGJ_SOFT_B0 89691
#define CGJ_SOFT_B1 89692
#define CGJ_SOFT_B2 89693
#define CGJ_SOFT_B3 89694
#define CGJ_SOFT_B4 89695
#define CGJ_SOFT_B5 89696
#define CGJ_SOFT_B6 89697
#define CGJ_SOFT_B7 89698
#define CGJ_SOFT_B8 89699
#include "_cgj_soft_m_template.inc"
