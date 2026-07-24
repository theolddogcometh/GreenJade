/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90400 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90400.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90400 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90400 \
 *   user/libcgj/tests/cgj_soft_milestone_90400.c -ldl
 * Direct: -DCGJ_SOFT_M90400_DIRECT + graph_batch90391.c … graph_batch90400.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90400_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90400
#define CGJ_SOFT_B0 90391
#define CGJ_SOFT_B1 90392
#define CGJ_SOFT_B2 90393
#define CGJ_SOFT_B3 90394
#define CGJ_SOFT_B4 90395
#define CGJ_SOFT_B5 90396
#define CGJ_SOFT_B6 90397
#define CGJ_SOFT_B7 90398
#define CGJ_SOFT_B8 90399
#include "_cgj_soft_m_template.inc"
