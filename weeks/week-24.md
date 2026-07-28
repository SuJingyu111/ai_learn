# Week 24 — Unit 4：Resize Optimization Case Study 与 Hard Gate

> **答案隔离规则（硬要求）**：本周优化和 gate 仍只使用 `labs/resize_starter/`。在 M4 verdict 为 `PASS` 之前，不得阅读、复制、diff 或调用 `src/cpu/resize_cpu.cpp`、`src/cuda/resize_cuda.cu`。只有 PASS 后才能做单独标记的 post-gate comparison；comparison 不得反向修改 gate 分数。

## Unit / 课程角色

- **Unit 4 · Image Operator Case Study，结课周。** 保留 simple correct baseline，每次改变一个变量，用 correctness+profiler+统计证据完成作品级报告。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [统一评分规则](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：在隔离 sandbox 部署 read-only Streamable HTTP server；真实 remote exposure 必须有 HTTPS、authorization、limits 与 rollback。

## 硬先修与 Remediation

- **硬先修**：Week 23 baseline/source-isolation/threat-model Exit ticket 全过；one-variable hypothesis 已冻结。
- **开周诊断（15 分钟）**：不用 profiler 重述 hypothesis、expected signal、correctness invariants、keep/rollback criterion。
- **未通过时**：回 Week 23 重建 immutable baseline 与 profiler evidence；不得边实现边改 hypothesis，也不得提前查看现有 source。

## Observable Objectives

1. 用同一 contract/cases比较 composed resize→normalize→HWC→CHW baseline 与一个 optimization（可为 resize kernel 或有证据的 fusion），正确性先于性能。
2. 报告 raw samples、median/p95、kernel/end-to-end、thermal/power 与 applicability。
3. 由 Systems/Compute 证据决定 keep/rollback/needs-more-data，不把单 size 收益泛化。
4. 在 sandbox 验证 read-only MCP deploy、Origin/auth/limits/logs/health/timeout/rollback。
5. 通过 M4：能把 image semantics、independent oracle、CUDA correctness、profiling 与安全部署串成可复现证据。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| immutable baseline | source/config/cases/environment/raw data 均有 identity 的比较基线 |
| one-variable change | 除目标优化外 contract、case、measurement procedure 不变 |
| sustained run | 足够长以观察 thermal/DVFS/latency drift 的固定 workload |
| applicability | 结论适用的 size/channel/layout/device/software 条件 |
| sandbox | 最小权限、资源受限、可停止/回滚的隔离部署环境 |
| health/readiness | 进程存活与可安全服务合法请求的不同信号 |
| rollback | 恢复到已知安全版本并验证 listener/state/credential 清理 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Best Practices — Performance Metrics](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#performance-metrics)，读 effective bandwidth、throughput 与 comparison boundaries | 20 |
| 1 | [Nsight Compute Profiling Guide](https://docs.nvidia.com/nsight-compute/ProfilingGuide/index.html)，读 “Overhead”“Metric Collection” | 15 |
| 2 | [MCP 2025-11-25 — Transports](https://modelcontextprotocol.io/specification/2025-11-25/basic/transports)，复读 Streamable HTTP Security Warning | 15 |
| 2 | [MCP 2025-11-25 — Authorization](https://modelcontextprotocol.io/specification/2025-11-25/basic/authorization)，复读 Protocol Requirements、Resource Parameter、Security Considerations | 20 |
| 2 | [MCP Security Best Practices](https://modelcontextprotocol.io/docs/tutorials/security/security_best_practices)，复读 token passthrough、SSRF/session/local server、scope minimization；协议语义仍以 2025-11-25 stable spec 为准 | 15 |

## 带问题阅读

- useful bytes 如何计算，为什么不能叫“DRAM theoretical bandwidth”？
- profiler replay/overhead 是否会改变 wall-time comparison？
- 一个优化只改善 4K RGBA 时，报告应如何限制结论？
- localhost smoke、isolated sandbox、production remote 各证明了什么？
- 缺少真实 IdP/TLS 时，哪些 deploy 项必须标 `BLOCKED`？

## Worked Example / 手算

- 对 float HWC resize，按每 output pixel 4-neighbor reads + 1 write 计算一个明确的 useful-byte convention；说明 cache/reuse为何使其不是实际 DRAM traffic。
- 给 baseline samples `[10,11,10,30,11]` 与 optimized `[9,9,10,9,25]` 手算 median，并说明 p95/样本量局限。
- 画 deploy→health→authorized read→invalid Origin→timeout→stop→rollback sequence；标每步 expected log/exit。

## 累计项目 Increment

- `docs/case-studies/resize-independent.md` 与 `labs/resize_starter/results/week-24/`。
- `ai-app/deployment/sandbox/`：manifest、runbook、health/auth/security regression、rollback evidence。
- `notes/curriculum/week-24.md`：学习记录；`notes/gates/gate-4.md`：M4 scores、blockers、verdict、post-gate decision。

## 本周必须产出

- immutable baseline、one-variable optimization、full workload/sustained data、case study。
- sandbox manifest/runbook、read-only deploy、security/limit/health/rollback evidence。
- M4 hard-gate submission；PASS 前无现有 resize source access。

## Hard Gate G4 / Milestone M4（统一 100 分）

| 维度 | 分值 | 本周证据 |
|---|---:|---|
| Knowledge | 25 | coordinate/sampling、profiling、transport/auth/security 解释 |
| Unseen practical | 30 | 新 resize/preprocess input、optimization invariants、deploy deny tests |
| Cumulative project | 30 | independent CPU/GPU preprocessor、raw data/reports/source isolation、clean deploy/rollback |
| Oral defense | 15 | case-study defense、applicability/limitations、Exit ticket |

**通过规则：`PASS` 需总分 ≥80/100、Knowledge 首次 ≥20/25、mandatory tests 100%，且 correctness、security、reproducibility blockers 全过。** 70–79 且无 critical blocker 为 `CONDITIONAL`；其余为 `RETAKE`。Source isolation 违规属于 reproducibility blocker；公网暴露无有效 auth/TLS 属 security blocker。

## Day 1 — Theory：Performance Evidence 与 Optimization Contract（105 分钟）

- **20 分钟｜必修理论**：阅读 performance metrics；冻结 useful-byte formula 与 kernel/end-to-end boundaries。
- **15 分钟｜必修理论**：阅读 Nsight Compute overhead/metric collection。
- **30 分钟｜worked example**：完成 useful bytes、sample median/outlier、applicability 手算。
- **30 分钟｜optimization design**：冻结 baseline identity、one change、expected metric、correctness invariants、keep/rollback criterion。
- **10 分钟｜retrieval**：闭卷说明 throughput/latency/effective bandwidth/replay。
- **必须留下**：immutable baseline manifest、formula、single-change plan。

## Day 2 — Theory + Guided：Deployment Security Contract（105 分钟）

- **15 分钟｜必修理论**：复读 Streamable HTTP Security Warning。
- **20 分钟｜必修理论**：复读 Authorization 指定章节。
- **15 分钟｜必修理论**：复读 Security Best Practices 指定威胁。
- **35 分钟｜guided deploy design**：写 fixed source/image、non-root、read-only filesystem（可行时）、loopback/isolated network、Origin/auth、CPU/memory/body/time limits、redacted logs、health/readiness、shutdown/rollback。
- **20 分钟｜Core guided**：运行 baseline correctness+30 samples，校验 CSV、median/p95/max error/source identity。
- **必须留下**：sandbox contract/runbook draft 与 verified baseline raw data。

## Day 3 — Guided → Independent：Optimization A 与 Sandbox Deploy（105 分钟）

- **55 分钟｜Core lab**：实现唯一 Optimization A，保留 baseline；先跑完整 independent CPU/GPU/sanitizer，再按冻结 procedure 采 raw samples。
- **35 分钟｜Sidecar build**：在本机隔离容器/已授权 sandbox 部署 read-only server；默认 loopback，运行 health、authorized/synthetic-authorized read 与 clean shutdown。
- **15 分钟｜验证**：diff effective config；无真实 HTTPS/IdP 时保留 loopback并标 `remote-auth BLOCKED`，不伪造 production pass。
- **必须留下**：A correctness/raw delta、deploy identity/config、health/auth transcript。

## Day 4 — Independent Lab：Workload Matrix 与 Break/Recovery（105 分钟）

- **60 分钟｜Core independent**：跑 `720p→1080p`、`1080p→720p`、`4K→1080p`×channels；做 sustained run，记录 thermal/latency drift；决定 keep/rollback/needs-more-data。
- **30 分钟｜Sidecar independent**：测试 invalid Origin/auth、oversized body、timeout/slow request、restart 中至少三项；验证 limits、redacted audit、恢复。
- **15 分钟｜rollback**：停止并回滚，确认无残留公网 listener、credential、写入 artifact。
- **必须留下**：full matrix/thermal note/decision、三项 security recovery、rollback proof。

## Day 5 — Integration / Gate Rehearsal（105 分钟）

- **60 分钟｜Core integration**：完成 `resize-independent.md`：Question→Baseline→Hypothesis→Evidence→Change→Correctness→Result→Confounders→Applicability；每个数字链接 raw data。
- **30 分钟｜Sidecar integration**：从 clean state按 runbook deploy→health→auth/read→negative suite→stop/rollback，生成 machine-readable acceptance summary。
- **15 分钟｜preflight**：按 25/30/30/15 自评，冻结 submission；再次证明未访问现有 resize source。
- **必须留下**：case-study draft、deployment acceptance、preflight score、source audit。

## Day 6 — Hard Gate G4 / M4（120 分钟 / 100 分）

- **45 分钟 / 25 分｜Knowledge**：闭卷手算 unseen resize coordinate/weights，解释 profiler metric 与 transport/auth/security边界。
- **35 分钟 / 30 分｜Unseen practical**：运行 unseen CPU/GPU odd/padded/random/sentinel preprocess case和 deploy deny/recovery；失败或 false pass 为 blocker。
- **25 分钟 / 30 分｜Cumulative project**：从 clean state复现 benchmark/profile与 sandbox acceptance；核对 raw data/environment/source isolation；不可复现为 blocker。
- **15 分钟 / 15 分｜Oral defense**：答辩 case study、one-variable decision、applicability、security limitation。
- **必须留下**：`notes/gates/gate-4.md`、逐项分数、blocker checklist、`PASS`/`CONDITIONAL`/`RETAKE` verdict 与不可变 evidence index。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M4 submission 已冻结且 Gate verdict 为 `PASS` 后做；若 Gate 落后、存在 blocker 或需 remediation，直接取消，不能用于改 Gate 分数或偷看隔离答案。
- **来源与选择规则**：看 MLOps Community [“Performance Optimization and Software/Hardware Co-design across PyTorch, CUDA, and NVIDIA GPUs”](https://home.mlops.community/en/public/videos/performance-optimization-and-softwarehardware-co-design-across-pytorch-cuda-and-nvidia-gpus)；只选一个带可观察metric或experiment建议的 performance claim，泛化/职业观点不进入技术结论。
- **30–45 分钟流程**：5 分钟定位带时间戳片段 → 15–20 分钟听取并回查 CUDA/Nsight一手资料 → 10–15 分钟写 `claim → evidence → scope → decision`；未由本地 frozen workload支持则保留为 hypothesis。
- **artifact**：`notes/feed/week-24.md`，含片段时间戳、一手交叉证据、一个 post-gate experiment/defer 决策。

## Exit Ticket / Unlocks

- **PASS**：≥80 且三类 blockers 全过，解锁 Unit 5 async pipeline；此后可单独阅读现有 CPU/CUDA resize 做 post-gate comparison，必须另存记录且不改 M4 分数。
- **CONDITIONAL / RETAKE**：继续只在 starter namespace按统一规则用新题/新输入补测；仍禁止访问现有实现。
