# Week 29 — Unit 5：Nsight Timeline 与 Overlap Evidence

## Unit / 课程角色

- **Unit 5 · Asynchronous Pipeline，第五周。** 用timeline证明或否定double buffer是否产生实际overlap，并区分first-frame latency、steady-state latency与throughput。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：冻结第一个20-case agent eval dataset；本周只建baseline，不调instructions/tools。
- **CUDA Graphs 仅为 Elective**：不属于必修Day、不进入M5分数、不阻塞Week 30。

## 硬先修与 Remediation

- **硬先修**：Week 28 double-buffer correctness/stress全过；每stage有NVTX/frame/slot identity；能从clean command生成Systems report。
- **开周诊断（15 分钟）**：定义可量化overlap：用时间区间intersection，而不是“timeline颜色看起来重叠”。
- **未通过时**：回 Week 19 focused trace；先对single-stream标NVTX并回答3个timestamp问题，再做double-buffer capture。

## Observable Objectives

1. 为 synchronous、single-stream、double-buffer 使用同一 workload/correctness/measurement contract。
2. 从 timestamps量化H2D/compute/D2H overlap，定位implicit sync、pageable fallback、engine/resource limits。
3. 分开报告 first-frame、steady-state、throughput、setup与end-to-end。
4. 对“无overlap”给可操作解释，不为了得到正结果改变case。
5. 建20-case、版本化、代表性的agent eval dataset与rubric，覆盖happy/invalid/permission/stale/injection/environment cases。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| interval intersection | 两stage时间区间重叠长度，用于量化而非目测 |
| first-frame latency | 第一输入开始到第一输出ready的时间 |
| steady-state interval | pipeline warm后相邻outputs完成间隔 |
| throughput | 固定窗口完成的frames/work数量 |
| implicit synchronization | 未显式设计却让host/device/streams串行的同步行为 |
| eval dataset | 固定version、inputs、expected/rubric与coverage的case集合 |
| leakage | 在调优/实现时看到或针对评估答案，破坏独立比较 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)，读 CUDA Trace、NVTX Trace、CLI capture/report/export | 25 |
| 1 | [CUDA Best Practices — Concurrent Copy and Execute](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#concurrent-copy-and-execute)，读 hardware/stream/pinned prerequisites | 20 |
| 2 | [NIST AI RMF Playbook — MEASURE 2.3 与 2.5](https://airc.nist.gov/airmf-resources/playbook/measure/)，读 deployment-like conditions、documented measures、validity/reliability、operating limits、generalizability与variance | 25 |
| 2 | [NIST AI RMF Playbook — MEASURE 2.6 与 2.7](https://airc.nist.gov/airmf-resources/playbook/measure/)，读 safety/security evaluation、knowledge limits、fail-safe、residual risk与documented evidence | 15 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 的 “Perform agent evaluation and tuning” 仅用于把 outcomes、representative cases、signals 对齐认证考纲；dataset/evidence/validity方法由 NIST MEASURE 与本周 baseline 教授。

## 带问题阅读

- Systems timeline能证明dependency/overlap，不能直接证明哪个microarchitecture瓶颈？
- copy engines数量、transfer direction、pinned memory如何限制overlap？
- first-frame latency可能变差但throughput变好时，如何报告？
- dataset的representativeness由哪些task/risk/environment维度组成？
- 为什么看过baseline后新增case必须产生新dataset version？
- MEASURE 2.3/2.5 如何把 case、metric、deployment-like condition、operating limit与generalizability limitation连成可审查证据？
- safety/security case 的 fail-safe 与 residual-risk criteria 为何必须在跑 baseline 前冻结？

## Worked Example / 手算

- `H2D0=[0,2]`、`K0=[2,7]`、`D2H0=[7,9]`；`H2D1=[2,4]`、`K1=[7,12]`、`D2H1=[12,14]`：计算copy/compute interval intersections与9→5?的steady interval。
- 给5 frames completion `[9,14,19,24,29]`，计算first-frame latency、steady interval与throughput。
- 把20 eval cases按normal/invalid/permission/stale/injection/environment分配，要求每类至少2且高风险类有negative expected。

## 累计项目 Increment

- `labs/async_pipeline/profiles/week-29/`：three variants、NVTX trace、timestamp CSV、overlap calculations。
- `ai-app/evals/dataset-v1/`：20-case manifest、fixtures、rubric、coverage matrix与baseline。
- `notes/curriculum/week-29.md`：prediction→timeline→conclusion。

## 本周必须产出

- 三variant fair comparison、overlap timestamp table、latency/throughput/limitation。
- 20-case eval dataset v1、rubric/coverage、untuned baseline。
- Exit ticket 与 Week 30 failure labels seed。

## Day 1 — Theory：Timeline 与 Overlap Metrics（105 分钟）

- **25 分钟｜必修理论**：阅读 Nsight Systems CUDA/NVTX/CLI章节。
- **20 分钟｜必修理论**：阅读 concurrent copy/execute prerequisites。
- **30 分钟｜worked example**：完成interval intersection、first/steady/throughput计算。
- **20 分钟｜capture design**：冻结variants、frames、NVTX names、capture range、expected overlap与confounders。
- **10 分钟｜retrieval**：闭卷区分timeline/Compute/event/wall。
- **必须留下**：metric formulas、worked table、capture plan。

## Day 2 — Theory + Guided：Dataset Specification（105 分钟）

- **25 分钟｜必修理论**：阅读 NIST MEASURE 2.3/2.5；定义 deployment-like conditions、validity、limits与variance evidence。
- **15 分钟｜必修理论**：阅读 NIST MEASURE 2.6/2.7；冻结 safety/security、fail-safe与residual-risk criteria。
- **40 分钟｜guided lab**：profile synchronous与single-stream，提取stage timestamps，不比较不同correctness/workload。
- **15 分钟｜eval guided**：定义dataset objective、non-goals、case schema、rubric、versioning与leakage rule。
- **10 分钟｜检查**：先写coverage targets，再选cases。
- **必须留下**：两variant traces、dataset contract v1。

## Day 3 — Guided → Independent：Double-buffer Trace 与 20 Cases（105 分钟）

- **55 分钟｜Core lab**：profile double-buffer；导出timestamps，计算实际intersection、first/steady/throughput，定位至少一个limiting dependency或明确无overlap。
- **35 分钟｜Sidecar build**：完成20-case manifest/fixtures/expected/rubric，覆盖六类并含无权限/注入/过期/环境失败。
- **15 分钟｜验证**：schema lint、duplicate ID、missing expected/fixture、coverage检查。
- **必须留下**：double-buffer trace/calculation、20-case dataset/coverage。

## Day 4 — Independent Lab：Hypothesis Test 与 Baseline Run（105 分钟）

- **60 分钟｜Core independent**：改变一个可解释变量（transfer size或stage duration），先预测再profile；不为得到overlap切换semantics。
- **30 分钟｜Sidecar independent**：在不改instructions/tools的前提下跑/人工dry-run dataset baseline，记录per-casescore/failure/evidence。
- **15 分钟｜evidence**：冻结dataset/config/runner hashes与baseline run ID。
- **必须留下**：one-variable trace、untuned baseline、immutable manifest。

## Day 5 — Integration：Async Evidence + Eval Baseline（105 分钟）

- **60 分钟｜Core integration**：写case-study skeleton：Question→Correctness→Timeline→Overlap→Latency/Throughput→Limits；三variants数字链接raw reports。
- **30 分钟｜Sidecar integration**：审查baseline false positive/negative/unknown；只记录failure candidates，不改dataset/rubric或系统。
- **15 分钟｜cross-check**：选择一个runtime measurement mistake与一个agent eval mistake，比较evidence需求。
- **必须留下**：case-study skeleton、baseline analysis、cross-track note。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：从unseen timestamp table计算overlap/latency/throughput并诊断一条implicit sync；复现一条focused capture。
- **35 分钟｜Sidecar assessment**：审阅5个随机cases的representativeness/scoring/evidence，指出dataset leakage/version change。
- **25 分钟｜Exit ticket**：冻结dataset v1与三variant evidence；预列reasoning/tool/context/environment/policy failure labels，供Week 30验证。
- **必须留下**：unseen calculation、trace reproduction、dataset audit与label seed。

## Elective — CUDA Graphs（可选 45–60 分钟，不计本周/不计 M5）

- 阅读 [CUDA Programming Guide — CUDA Graphs](https://docs.nvidia.com/cuda/cuda-programming-guide/04-special-topics/cuda-graphs.html)，只在fixed shape/address/control flow成立时做capture/replay spike。
- 单独报告capture/setup cost、replay、restrictions与fallback；不得用Graph结果替代Week 29三variant必修证据。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在前序 Gate 为 `PASS`、20-case dataset 与三variant核心证据无 backlog 时做；若 Gate 落后、dataset validity 未通过或 M5 preflight 有 blocker，直接取消，不以 Feed Card 或 CUDA Graphs补分。
- **来源与选择规则**：读 MLCommons 的 [“MLPerf Inference v6.0 Benchmark Results”](https://mlcommons.org/2026/04/mlperf-inference-v6-0-results/)；只选一个关于 representative/reproducible/real-world benchmark 的 claim，并必须回查其公开 benchmark rules、dataset或scenario定义，新闻稿数字不能单独当证据。
- **30–45 分钟流程**：5 分钟选 claim → 15–20 分钟读文章并追一层技术来源 → 10–15 分钟写 `claim → evidence → scope → decision`。
- **artifact**：`notes/feed/week-29.md`，含 benchmark/version、适用scenario、一个是否改变 dataset coverage 的决策。

## Exit Ticket / Unlocks

- **通过条件**：overlap由timestamp量化，三variants公平，latency/throughput分开；dataset恰好20个版本化cases、有rubric/coverage、baseline未调优。
- **解锁**：Week 30 async case study 与 M5 hard gate。
