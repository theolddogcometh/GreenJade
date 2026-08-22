---
name: pr-post
description: >
  Draft Patreon and X only for GreenJade major milestones (Dual DoD A/B
  close, live dash, named 0.2.0). Do not use for routine fly cuts, glass
  photos, or hardware-test pack notes unless the user says treat this as
  a major milestone. Draft only — never publish.
disable-model-invocation: true
---

# /pr-post — major-milestone drafts only

This skill is **slash-only** (`/pr-post`). Do not auto-run it.

## Gate

Read `AGENTS.md` § **Major milestones**. Proceed only if:

- that table has a **new** proof this turn, or
- the user wrote **treat this as a major milestone**.

Otherwise refuse in one sentence. Do not draft.

Do not reprint M0 OK or laptop L3 unless asked.

## After the gate

1. Read `GJ_IMAGE_VERSION` and `docs/HWTEST_TOMORROW.md` fly-this-cut.
2. Evidence list + **will not claim** list.
3. **Media** (required):
   - Read `media/README.md` and list `media/`.
   - Attach existing brand stills when they fit (`GreenJade_Logo.jpg` default for X).
   - New art: `image_edit` from those files only. Write `media/pr/<slug>.jpg`.
   - Do not `image_gen` the G wordmark or CRT mascot from text. Do not fake STATUS text.
   - DUT glass photos stay evidence, not restyled logos.
4. Patreon (title + 3–8 short paragraphs, M0-OK) + still paths.
5. X (≤260 chars if possible) + still path.
6. Stop. Save prose to `docs/pr/` only if asked.

## Forbidden claims

Same as `AGENTS.md`. Banner / ARP / ping / PK_OK / SUCCESS are not login and not Dual DoD B.
