/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89400 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89400.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89400 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89400 \
 *   user/libcgj/tests/cgj_soft_milestone_89400.c -ldl
 * Direct: -DCGJ_SOFT_M89400_DIRECT + graph_batch89391.c … graph_batch89400.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89400_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89400
#define CGJ_SOFT_B0 89391
#define CGJ_SOFT_B1 89392
#define CGJ_SOFT_B2 89393
#define CGJ_SOFT_B3 89394
#define CGJ_SOFT_B4 89395
#define CGJ_SOFT_B5 89396
#define CGJ_SOFT_B6 89397
#define CGJ_SOFT_B7 89398
#define CGJ_SOFT_B8 89399
#include "_cgj_soft_m_template.inc"
