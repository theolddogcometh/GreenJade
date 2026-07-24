/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91700 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91700.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91700 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91700 \
 *   user/libcgj/tests/cgj_soft_milestone_91700.c -ldl
 * Direct: -DCGJ_SOFT_M91700_DIRECT + graph_batch91691.c … graph_batch91700.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91700_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91700
#define CGJ_SOFT_B0 91691
#define CGJ_SOFT_B1 91692
#define CGJ_SOFT_B2 91693
#define CGJ_SOFT_B3 91694
#define CGJ_SOFT_B4 91695
#define CGJ_SOFT_B5 91696
#define CGJ_SOFT_B6 91697
#define CGJ_SOFT_B7 91698
#define CGJ_SOFT_B8 91699
#include "_cgj_soft_m_template.inc"
