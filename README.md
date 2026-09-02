# Embedded DevOps Codex Handoff

This folder is the handoff from a ChatGPT planning conversation into Codex.

Start here:

1. `AGENTS.md`
2. `PROJECT_CONTEXT.md`
3. `WHERE_WE_LEFT_OFF.md`
4. `HARDWARE_INVENTORY.md`
5. `PROJECT_IDEAS_AND_ROADMAP.md`

Then use the prompt in:

- `CODEX_START_PROMPT.md`

## Intended Repository Shape

As implementation begins, evolve toward:

```text
EmbeddedDevOps/
|
+-- AGENTS.md
+-- PROJECT_CONTEXT.md
+-- HARDWARE_INVENTORY.md
+-- WHERE_WE_LEFT_OFF.md
+-- PROJECT_IDEAS_AND_ROADMAP.md
+-- README.md
|
+-- firmware/
+-- gateway/
+-- server/
+-- infra/
+-- scripts/
+-- tests/
+-- docs/
```

Do not create empty complexity prematurely. Add folders when the relevant implementation begins.
