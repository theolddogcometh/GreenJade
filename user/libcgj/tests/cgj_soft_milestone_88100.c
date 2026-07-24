/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88100 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88100.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88100 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88100 \
 *   user/libcgj/tests/cgj_soft_milestone_88100.c -ldl
 * Direct: -DCGJ_SOFT_M88100_DIRECT + graph_batch88091.c … graph_batch88100.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88100_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88100
#define CGJ_SOFT_B0 88091
#define CGJ_SOFT_B1 88092
#define CGJ_SOFT_B2 88093
#define CGJ_SOFT_B3 88094
#define CGJ_SOFT_B4 88095
#define CGJ_SOFT_B5 88096
#define CGJ_SOFT_B6 88097
#define CGJ_SOFT_B7 88098
#define CGJ_SOFT_B8 88099
#include "_cgj_soft_m_template.inc"
