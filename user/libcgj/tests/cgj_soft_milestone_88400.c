/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88400 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88400.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88400 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88400 \
 *   user/libcgj/tests/cgj_soft_milestone_88400.c -ldl
 * Direct: -DCGJ_SOFT_M88400_DIRECT + graph_batch88391.c … graph_batch88400.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88400_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88400
#define CGJ_SOFT_B0 88391
#define CGJ_SOFT_B1 88392
#define CGJ_SOFT_B2 88393
#define CGJ_SOFT_B3 88394
#define CGJ_SOFT_B4 88395
#define CGJ_SOFT_B5 88396
#define CGJ_SOFT_B6 88397
#define CGJ_SOFT_B7 88398
#define CGJ_SOFT_B8 88399
#include "_cgj_soft_m_template.inc"
