# Week 28 — Unit 5：Double-buffer Pipeline 与 Context Drift

## Unit / 课程角色

- **Unit 5 · Asynchronous Pipeline，第四周。** 将 Week 27 已验证的 slot复制成两个完整资源集合，先保证 per-frame correctness与lifetime，再谈性能。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：在 version/hash/TTL 之上增加 checkpoint与context-drift detection。

## 硬先修与 Remediation

- **硬先修**：Week 27 single-slot状态机、early-stop/error drain通过；已写two-slot no-share invariants。
- **开周诊断（15 分钟）**：给三frames/two slots，逐stage写slot index、owner、event与最早可复用时间。
- **未通过时**：退回 single-slot；只在纸面模拟3/4/5 frames，直到无共享 host/device storage和illegal transition，再开始 Day 1。

## Observable Objectives

1. 实现两个完整、互不共享active storage的frame slots与round-robin scheduler。
2. 对1/2/3/odd/100 frames保持frame ID、output order、per-frame checksum/correctness。
3. 用events表达slot reuse与consumer readiness，不靠global sync。
4. 在early stop/failure时停止新提交、drain/cleanup已提交 work，无析构/复用race。
5. 建checkpoint与drift detector：objective/config/source/dataset/artifact hash改变时暂停并请求revalidation。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| double buffer | 两套完整资源交替使用，使不同frames的stages有并行机会 |
| slot isolation | 一个slot的active host/device buffers不被另一个slot共享或复用 |
| round robin | frame `i`使用slot `i mod 2`，前提是slot已ready/free |
| backpressure | 无free slot时停止提交，等待合法completion而不是覆盖数据 |
| per-frame correctness | 每个frame按ID与同步oracle比较，不只检查最后一帧 |
| checkpoint | objective/config/source/progress/decision的可验证快照 |
| context drift | 当前上下文与已确认checkpoint发生未授权/未验证偏离 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Best Practices — Asynchronous and Overlapping Transfers with Computation](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#asynchronous-and-overlapping-transfers-with-computation)，读 staged concurrent copy/execute 条件 | 25 |
| 1 | [CUDA Programming Guide — Asynchronous Execution](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/asynchronous-execution.html)，读 “What is Asynchronous Concurrent Execution?”、“CUDA Streams”与concurrent transfer examples | 20 |
| 2 | [NIST AI RMF Playbook — MEASURE 2.4](https://airc.nist.gov/airmf-resources/playbook/measure/)，读 MEASURE 2.4 “About”“Suggested Actions”“Transparency and Documentation”：production behavior monitoring、drift、baseline差异、alert与human review | 20 |
| 2 | [CUDA Runtime API — Event Management](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__EVENT.html)，复读 cross-stream dependency与query | 15 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 的 stale context、context drift、durable progress、resume 目标仅用于考纲覆盖映射；monitoring/drift机制由 NIST MEASURE 2.4 与本周 checkpoint experiments 教授。

## 带问题阅读

- 两个streams为什么不自动带来copy/compute overlap？
- 每个slot必须独享哪些host/device/event对象，哪些immutable metadata可共享？
- round-robin遇到slow frame时如何backpressure，为什么不能覆盖slot？
- output order与completion order不同，consumer如何重排/验证？
- drift detector应阻断什么变化，什么变化只需记录而不阻断？
- MEASURE 2.4 要求把 production 指标与哪个 baseline 比较？什么差异触发 alert、revalidation 或 human review，而不是自动“修正”？

## Worked Example / 手算

- 为frames 0/1/2、slots A/B写 fill→H2D→compute→D2H→ready→consume 时间表。
- 假设A的frame0 kernel变慢，推演frame2到达时scheduler是wait、queue还是overwrite。
- 给 checkpoint `{objective, runner_hash, config_hash, dataset_hash, completed=[0,1]}`；改变block size、case order、comment分别决定block/log/ignore。

## 累计项目 Increment

- `labs/async_pipeline/double_buffer/`：two-slot resources、scheduler、per-frame oracle、stress/error tests。
- `ai-app/state/checkpoint.schema.json`、drift policy与fixtures。
- `notes/curriculum/week-28.md`：schedule hand table、invariants与drift decisions。

## 本周必须产出

- double-buffer runner、slot-isolation proof、per-frame correctness、stress/shutdown/error evidence。
- checkpoint/drift schema、authorized-change workflow、stale/drift tests。
- Exit ticket 与 Week 29 profiler questions。

## Day 1 — Theory：Overlap Preconditions 与 Two-slot Design（105 分钟）

- **25 分钟｜必修理论**：阅读 Best Practices overlap章节；列pinned memory、streams、engines、dependencies等前提。
- **20 分钟｜必修理论**：阅读 Programming Guide concurrent transfers。
- **30 分钟｜worked example**：完成3 frames/2 slots schedule与slow-frame backpressure。
- **20 分钟｜design**：枚举每slot独享storage/stream/events与可共享immutable objects。
- **10 分钟｜retrieval**：闭卷回答overlap prerequisite与no-share invariants。
- **必须留下**：schedule、resource table、two-slot design。

## Day 2 — Theory + Guided：Scheduler 与 Drift Schema（105 分钟）

- **20 分钟｜必修理论**：阅读 NIST MEASURE 2.4 的 production monitoring、drift、baseline comparison、alert与human-review建议。
- **15 分钟｜必修理论**：复读 Event API dependency/query。
- **45 分钟｜guided lab**：建立two-slot objects与transition checker；先处理1/2/3 frames，不优化submit loop。
- **15 分钟｜drift guided**：定义checkpoint objective/source/config/dataset/progress/decision hashes。
- **10 分钟｜检查**：证明任何slot复用前ready/consumed event条件满足。
- **必须留下**：slot construction、transition tests、checkpoint schema。

## Day 3 — Guided → Independent：Double Buffer 与 Drift Detector（105 分钟）

- **55 分钟｜Core lab**：实现round-robin submit/consume；保存frame ID，逐帧与synchronous oracle比较，覆盖odd frame count。
- **35 分钟｜Sidecar build**：实现/模拟drift detection；runner/config/dataset/objective变化分别block并要求explicit accept/new checkpoint。
- **15 分钟｜验证**：comment/timestamp等非语义变化只记录；不能让hash策略产生无意义reset而不说明。
- **必须留下**：runner/per-frame matrix、drift fixtures与decision log。

## Day 4 — Independent Lab：Stress、Backpressure、Recovery（105 分钟）

- **65 分钟｜Core independent**：运行0/1/2/3/100/odd frames、slow frame、early stop、operator/copy failure；验证stop-submit→drain→cleanup与no illegal reuse。
- **25 分钟｜Sidecar independent**：测试stale checkpoint、missing artifact、unauthorized config change；全部暂停，不自动“修正”历史。
- **15 分钟｜evidence**：保存transition trace、checksum、sanitizer/log limitations。
- **必须留下**：stress/error summary、三条 drift deny、residual risk。

## Day 5 — Integration：Pipeline State × Durable Checkpoint（105 分钟）

- **60 分钟｜Core integration**：从clean state跑fixed dataset，生成per-frame stage/event/slot trace与correctness summary；无device-wide sync。
- **30 分钟｜Sidecar integration**：每批完成后checkpoint只保存completed frame IDs与artifact hashes；resume验证hash后跳过已完成，不重复submit。
- **15 分钟｜cross-check**：模拟中断后resume，检查output completeness/order且无duplicate。
- **必须留下**：integrated trace、checkpoint/resume evidence、duplicate check。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：给unseen four-frame timing/problem trace找slot reuse bug并修复；运行unseen odd-frame/early-stop。
- **35 分钟｜Sidecar assessment**：对六个checkpoint changes分类continue/log/revalidate/new-run/reject并解释。
- **25 分钟｜Exit ticket**：提出Week 29三个timeline问题与可量化overlap定义，不把double buffer存在当作overlap证据。
- **必须留下**：assessment、unseen result、drift decisions与profiler question set。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在前序 Gate 为 `PASS` 且 two-slot correctness、checkpoint 与 Exit Ticket 无 backlog 时做；若 Gate 落后或 drift/duplicate remediation 未清，直接取消，不顺延。
- **来源与选择规则**：看 MLOps Community 的 [“ML Drift — How to Identify Issues Before They Become Problems”](https://home.mlops.community/en/public/videos/ml-drift-how-to-identify-issues-before-they-become-problems)；只选一个能区分 data drift、concept drift 或 acceptable variation，且能由 NIST MEASURE 2.4 支持或限定的 claim。
- **30–45 分钟流程**：5 分钟定位一个带时间戳片段 → 15–20 分钟观看并回查 NIST → 10–15 分钟写 `claim → evidence → scope → decision`；无 threshold/baseline 证据则不采用自动化规则。
- **artifact**：`notes/feed/week-28.md`，含片段时间戳、NIST交叉证据、一个 monitor/alert 决策。

## Exit Ticket / Unlocks

- **通过条件**：slot isolation/per-frame correctness/stress/error drain全；resume无duplicate；drift变化可检测并需显式处理。
- **解锁**：Week 29 timeline overlap evaluation。
