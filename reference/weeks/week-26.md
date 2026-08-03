# Week 26 — Unit 5：Pinned Async Transfer 与 Orin Unified-memory Caveat

## Unit / 课程角色

- **Unit 5 · Asynchronous Pipeline，第二周。** 建立真正可异步的 host↔device transfer 条件，并避免把 Orin 的共享物理内存误解为“所有 memory types 都零成本等价”。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：把 Week 25 memory selection转成 durable、可恢复且版本化的 task state。

## 硬先修与 Remediation

- **硬先修**：Week 25 explicit stream/event runner通过；能说明 output-ready 与 host buffer lifetime。
- **开周诊断（15 分钟）**：对 pageable、pinned、registered、managed memory 分别写 allocation owner、transfer API、lifetime/system cost。
- **未通过时**：先只做 pageable→pinned单向 copy smoke，分离 setup与 steady-state；未证明 async behavior前不得开始 overlap 结论。

## Observable Objectives

1. 公平比较 pageable 与 pinned host buffers，分开 setup/registration cost 与 steady transfer。
2. 说明 async H2D/D2H 要求：合适 host memory、non-default/explicit stream、独立 engine/workload与无隐式依赖。
3. 在 Orin 上记录实际 memory model与版本；解释共享 DRAM不消除 cache coherency、ownership、sync、migration/placement或测量成本。
4. 用 event/timeline验证 transfer completion，不在完成前释放/复用 host buffer。
5. 设计 durable review state：schema version、task/artifact identity、legal transition、atomic save、resume/reset，不存 secret。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| pageable memory | OS 可分页 host allocation；runtime 可能需 staging 才能传输 |
| pinned memory | page-locked host memory；支持 DMA/async transfer，但占用系统资源 |
| registration | 将现有 host range注册为 page-locked 的 setup 操作 |
| managed memory | CUDA 管理、可由 CPU/GPU访问的 allocation；行为依平台/访问/同步而定 |
| shared physical DRAM | iGPU CPU/GPU共享物理内存系统，不等于所有访问无同步/缓存成本 |
| durable state | 跨进程保存、有 schema/provenance/transition规则的任务状态 |
| resume | 验证 version/hash/state后从明确 pending step继续，而非重放所有动作 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Best Practices — Pinned Memory](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#pinned-memory)，读 benefits、scarcity、batch transfer | 20 |
| 1 | [CUDA Runtime API — Memory Management](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__MEMORY.html)，读 host alloc/register、async memcpy contracts | 20 |
| 2 | [CUDA for Tegra Application Note](https://docs.nvidia.com/cuda/cuda-for-tegra-appnote/index.html)，读 “Memory Management” 与 integrated GPU coherency/usage guidance；以设备实际 JetPack版本为准 | 25 |
| 2 | [SQLite — Atomic Commit](https://www.sqlite.org/atomiccommit.html)，读 3.5、3.7、3.10–3.11 与 4.2–4.6：rollback journal、flush ordering、commit point、crash recovery；用来理解 durable save 的机制，不要求照搬 SQLite实现 | 15 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 的 state persistence、execution continuity、reset 目标仅用于考纲覆盖映射；atomicity/crash-recovery机制由 SQLite 一手设计文档和本周 fault fixtures 教授。

## 带问题阅读

- pinned memory 为什么既可能提高 async transfer，又可能降低系统整体可用资源？
- benchmark 应把 allocation/registration放在 timed region内还是外，为什么要分别报告？
- `cudaMemcpyAsync` 名字是否足以证明 host/device overlap？
- Orin 上 managed/pinned/device memory 的正确选择取决于哪些 access/ownership模式？
- durable resume 如何防重复执行、stale artifact与 schema drift？
- rollback journal 何时必须先于主状态落盘，commit point 与 hot-journal recovery如何避免半写状态被当成完成？

## Worked Example / 手算

- 对 16 MiB H2D、测得 1.0 ms，按 useful bytes计算 GB/s；分别加上 2 ms registration看 first-use latency。
- 画 host fill→H2D→event→host reuse，标 pinned buffer最早可复用点。
- 对 Orin shared DRAM画 CPU cache、GPU访问、synchronization boundary；列“无需 PCIe copy”不推出“无需任何成本”。
- 推演 state `new→validated→compared→human_reviewed→closed`，对重复 resume与 artifact hash变化决定行为。

## 累计项目 Increment

- `labs/async_pipeline/transfers/`：pageable/pinned/registered cases、raw samples、Orin manifest。
- `ai-app/state/review-state.schema.json`、state machine与 resume fixtures。
- `notes/curriculum/week-26.md`：memory choice、system cost、device-specific caveats。

## 本周必须产出

- fair transfer table、setup/steady split、host lifetime proof、Orin memory note。
- versioned durable-state schema、state machine、clean/corrupt/stale/duplicate resume tests。
- Exit ticket 与 Week 27 slot ownership inputs。

## Day 1 — Theory：Pinned Memory 与 Async Copy Contract（105 分钟）

- **20 分钟｜必修理论**：阅读 pinned-memory section；列收益前提与 system cost。
- **20 分钟｜必修理论**：阅读 Memory API host alloc/register/memcpy async contracts。
- **30 分钟｜worked example**：完成 16 MiB bandwidth、first-use/steady、buffer reuse timeline。
- **25 分钟｜experiment design**：冻结 size/direction/iterations/warm-up/allocation boundary与 correctness。
- **10 分钟｜retrieval**：闭卷解释 pageable/pinned/registered。
- **必须留下**：transfer contract、公式、lifetime graph。

## Day 2 — Theory + Guided：Orin Caveat 与 Durable State（105 分钟）

- **25 分钟｜必修理论**：阅读 CUDA for Tegra Memory Management；记录实际 JetPack/SoC对应版本，不外推其他平台。
- **15 分钟｜必修理论**：阅读 SQLite rollback journal、flush ordering、commit point与crash recovery；映射到 state temp-write→flush→atomic publish→recover contract。
- **40 分钟｜guided lab**：实现 pageable/pinned两种 host buffer与同一 explicit stream copy；分别采 setup与 steady event/wall times。
- **15 分钟｜state guided**：定义 schema version/task/artifact hash/status/completed/pending/decision/updated fields。
- **10 分钟｜检查**：所有输出先做 checksum/correctness。
- **必须留下**：raw transfer samples、environment、state schema v1。

## Day 3 — Guided → Independent：Registration/Managed Probe 与 Resume（105 分钟）

- **55 分钟｜Core lab**：增加 host registration；若 Orin可用，做一个受控 managed-memory访问 probe，记录同步与访问模式；不可用则写 `BLOCKED`，不得模拟结果。
- **35 分钟｜Sidecar build**：实现/伪实现 load→validate→resume→atomic save；clean resume只能执行 pending step，不重跑已完成动作。
- **15 分钟｜验证**：corrupt JSON、old schema、artifact hash mismatch 全部 fail closed。
- **必须留下**：registration/managed evidence或blocked record、三条 resume tests。

## Day 4 — Independent Lab：Lifetime Fault 与 State Transitions（105 分钟）

- **60 分钟｜Core independent**：在隔离 target 注入 early reuse/free 或 wrong-event fault，收集 diagnostic/timeline，修复后压力重复；故障不进入默认 build。
- **30 分钟｜Sidecar independent**：测试 backward transition、duplicate resume、closed-state mutation；分别 reject/idempotent no-op/human re-open policy并审计。
- **15 分钟｜evidence**：写 sanitizer/timeline detection limits 与 human escalation。
- **必须留下**：fault→diagnostic→fix、三种 transition results、residual risk。

## Day 5 — Integration：Transfer/State Evidence（105 分钟）

- **60 分钟｜Core integration**：用相同 size/cases比较 pageable/pinned/registered；报告 setup、steady、event、wall、thermal，结论绑定平台与 workload。
- **30 分钟｜Sidecar integration**：让 state只引用 transfer artifact hash/manifest；修改 raw artifact后 resume必须 invalidated。
- **15 分钟｜cross-check**：从 clean process复跑并验证 state不含 token/path command/untrusted instruction。
- **必须留下**：fair table、device-specific conclusion、artifact-invalidated resume。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：给三个 memory/use scenarios选择 pageable/pinned/managed/device并辩护；运行 unseen transfer size与 lifetime check。
- **35 分钟｜Sidecar assessment**：从 interrupted fixture clean resume；演示 old schema/hash/backward/duplicate行为。
- **25 分钟｜Exit ticket**：回答“Orin shared memory为何不等于free zero-copy”，列 Week 27 frame-slot owner/resource。
- **必须留下**：scenario answers、unseen result、resume demo 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在前序 Gate 为 `PASS` 且本周 transfer/state artifacts 无 backlog 时做；若 Gate 落后、resume remediation 未清或核心实验未完成，直接取消，不补做。
- **来源与选择规则**：从 NVIDIA Technical Blog 的 [Unified Memory 专题](https://developer.nvidia.com/blog/tag/unified-memory/) 选择一篇明确写出 GPU 架构、CUDA/JetPack 或 memory model 的文章；必须与自己的 SoC/软件版本匹配，否则只记录“不适用”并取消技术采纳。
- **30–45 分钟流程**：5 分钟按平台筛选 → 15–20 分钟阅读并查原始版本说明 → 10–15 分钟写 `claim → evidence → scope → decision`，特别区分“shared DRAM”“managed memory”“full coherence”。
- **artifact**：`notes/feed/week-26.md`，含平台/version match、反例或限制、一个 memory-choice 决策。

## Exit Ticket / Unlocks

- **通过条件**：transfer公平、setup/steady分开、lifetime正确、Orin结论有限定；durable state有 version/hash/legal transitions/reset，无秘密。
- **解锁**：Week 27 ownership/frame-slot state machine。
