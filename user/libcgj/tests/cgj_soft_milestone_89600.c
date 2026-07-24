/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89600 \
 *   user/libcgj/tests/cgj_soft_milestone_89600.c -ldl
 * Direct: -DCGJ_SOFT_M89600_DIRECT + graph_batch89591.c … graph_batch89600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89600
#define CGJ_SOFT_B0 89591
#define CGJ_SOFT_B1 89592
#define CGJ_SOFT_B2 89593
#define CGJ_SOFT_B3 89594
#define CGJ_SOFT_B4 89595
#define CGJ_SOFT_B5 89596
#define CGJ_SOFT_B6 89597
#define CGJ_SOFT_B7 89598
#define CGJ_SOFT_B8 89599
#include "_cgj_soft_m_template.inc"
