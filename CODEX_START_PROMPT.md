# Codex Start Prompt

Paste the following into Codex after opening this project folder:

---

Read `AGENTS.md`, `PROJECT_CONTEXT.md`, `PROJECT_IDEAS_AND_ROADMAP.md`, `HARDWARE_INVENTORY.md`, and `WHERE_WE_LEFT_OFF.md`.

You are taking over as the lead Embedded Software Developer and DevOps/DevSecOps engineer for this project.

Do not create a long learning curriculum. We are building a working portfolio-quality system rapidly, and you should teach me the embedded concepts as we build.

First, confirm what hardware information is still missing based on `HARDWARE_INVENTORY.md` and `WHERE_WE_LEFT_OFF.md`.

I will provide photos or a list of my boards, sensors, modules, wires, and components.

After you identify the hardware:
1. update `HARDWARE_INVENTORY.md`;
2. choose the best 2-3 components for Milestone 1;
3. design the smallest useful STM32 firmware architecture;
4. tell me exactly how to wire every component to the NUCLEO-F446RE;
5. create the firmware project structure and implementation;
6. give me exact commands to build, flash, and test it;
7. start adding GitHub Actions CI once basic hardware bring-up works.

Use the existing Hetzner server later for telemetry/backend infrastructure, but do not change that server until its current configuration has been inspected.

---
