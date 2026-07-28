# Week 31 — Unit 6：ONNX Runtime CUDA EP、Partition 与 Fallback

## Unit / 课程角色

- **Unit 6 · Runtime Integration，第一周。** 把已掌握的correctness、provider boundary、profiling与reproducibility应用到ONNX Runtime CUDA Execution Provider（CUDA EP）。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：把Week 29 dataset和Week 30 failure labels升级为可关联的logs/traces/artifact bundle；trace不是correctness oracle。

## 硬先修与 Remediation

- **硬先修**：M5 `PASS`；能导出/检查一个小ONNX model；能运行ORT CPU baseline；理解device buffer/stream/event lifetime。
- **开周诊断（15 分钟）**：画 model graph→provider capability query→partition→subgraphs→execution；写provider priority与CPU fallback的区别。
- **未通过时**：先完成ORT CPU session、input/output names/shapes/dtypes与known output；无兼容CUDA EP环境时可做partition设计/CPU证据，但GPU objectives和Exit ticket标 `BLOCKED`，不得把provider注册成功当作CUDA执行证明。

## Observable Objectives

1. 记录ORT/CUDA/cuDNN/driver/model/opset/provider options兼容矩阵并构建可复现session。
2. 解释provider order、graph partition、kernel coverage与CPU fallback；用profiling/log evidence定位node assignment。
3. 对同一model/input比较CPU EP与CUDA EP correctness，区分setup/warm-up/run/end-to-end。
4. 构造一个有意fallback model/case，观察partition/copy/performance，而非静默接受。
5. 生成evaluation artifact bundle，关联run/case/trace/span/tool/permission/score/failure/environment并执行redaction/integrity检查。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Execution Provider (EP) | ORT为特定硬件/runtime提供kernel与execution capability的后端 |
| provider priority | session中EP的注册/选择顺序，影响可支持nodes的分配 |
| graph partition | 按EP capability把graph nodes划分为可执行subgraphs |
| fallback | 高优先级EP不支持的nodes由后续EP（常为CPU）执行 |
| provider boundary | 不同EP subgraphs之间可能产生copy/sync/format边界 |
| session setup | model load、optimization、partition、kernel/session初始化阶段 |
| artifact bundle | 可离线审阅的versioned config、trace、results、evidence index集合 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [ONNX Runtime — Execution Providers](https://onnxruntime.ai/docs/execution-providers/)，读 EP architecture、GetCapability/partition概览 | 25 |
| 1 | [ONNX Runtime — CUDA Execution Provider](https://onnxruntime.ai/docs/execution-providers/CUDA-ExecutionProvider.html)，读 Requirements、Configuration Options、Performance Tuning | 25 |
| 2 | [ONNX Runtime — Profiling Tools](https://onnxruntime.ai/docs/performance/tune-performance/profiling-tools.html)，读 built-in profiling与trace interpretation | 20 |
| 2 | [OpenTelemetry — Traces](https://opentelemetry.io/docs/concepts/signals/traces/) 的 “Traces”“Spans”“Span Context”“Links”，并对照 [W3C Trace Context](https://www.w3.org/TR/trace-context/) 3.2 `traceparent`、3.3 `tracestate` 与 6 Privacy Considerations；区分关联标识与业务证据 | 15 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 仅用于把 evaluation evidence、tracing 与 deployment artifacts 映射到认证考纲；provider partition/fallback 与 trace-context机制以上述 ORT/OpenTelemetry/W3C 一手资料为准。

## 带问题阅读

- provider list中有CUDA EP为何仍不能证明每个node在GPU执行？
- CPU fallback可能在哪些boundary引入copy/sync，如何观测？
- session creation、first run、warm run为什么必须分开？
- ORT optimization后的node names/graph与原ONNX graph可能有什么差异？
- trace/log/artifact分别回答“发生了什么”“如何关联”“能否复核”的哪部分？
- `traceparent` 能证明请求被关联，却不能证明 output正确或artifact未被篡改；还需要哪些hash、manifest与provider evidence？

## Worked Example / 手算

- graph `Input→Conv→Relu→CustomUnsupported→Add→Output`：假设CUDA支持Conv/Relu/Add但不支持Custom，画partition与两个provider boundaries；列可能copies。
- provider order `[CUDA, CPU]` 与 `[CPU, CUDA]` 分别推演assignment；不凭名字推断实际结果。
- 对CPU output `[1,2,3]` 与CUDA `[1,2.00001,2.9999]` 手算max abs/relative error并写tolerance。
- 为一个eval run设计 `run_id→case_id→trace_id/span_id→artifact_id` 关联。

## 累计项目 Increment

- `labs/ort_runtime/provider_partition/`：model manifest、CPU/CUDA runner、profiling、fallback fixture。
- `ai-app/evals/artifacts/week-31/`：bundle schema、structured events、redaction/integrity checker。
- `notes/curriculum/week-31.md`：compatibility matrix、partition evidence、timing/correctness。

## 本周必须产出

- ORT/CUDA environment manifest、CPU/CUDA correctness、provider/partition/fallback evidence、timing table。
- 一份完整eval artifact bundle与missing/tampered/secret tests。
- Exit ticket与Week 32 I/O Binding inputs。

## Day 1 — Theory：EP Architecture 与 CUDA Requirements（105 分钟）

- **25 分钟｜必修理论**：阅读Execution Providers architecture；画GetCapability→partition→execution。
- **25 分钟｜必修理论**：阅读CUDA EP requirements/options/performance。
- **30 分钟｜worked example**：完成五node graph partition、provider order与boundary copies推演。
- **15 分钟｜environment plan**：列ORT/CUDA/cuDNN/driver/opset/provider options/source identity。
- **10 分钟｜retrieval**：闭卷解释provider available/registered/assigned的差别。
- **必须留下**：partition图、compatibility checklist、worked boundary table。

## Day 2 — Theory + Guided：CPU Baseline、Profiling、Trace IDs（105 分钟）

- **20 分钟｜必修理论**：阅读ORT Profiling Tools。
- **15 分钟｜必修理论**：阅读 OpenTelemetry trace/span/context/links，并对照 W3C `traceparent`/`tracestate`与privacy；标出 correlation ≠ correctness/integrity。
- **45 分钟｜guided lab**：加载固定small model，建立CPU EP known/random input baseline；记录session setup、first/warm runs、outputs。
- **15 分钟｜trace guided**：定义run/case/trace/span/artifact IDs与redaction fields。
- **10 分钟｜检查**：model/input/output/opset/hash均进入manifest。
- **必须留下**：CPU baseline、profiling setup、trace identity schema。

## Day 3 — Guided → Independent：CUDA EP 与 Artifact Bundle（105 分钟）

- **55 分钟｜Core lab**：以显式provider list创建CUDA session；与CPU同input比较，采provider/profile evidence确认实际assignment；无GPU则保存exact blocked reason。
- **35 分钟｜Sidecar build**：生成一个bundle：dataset/config/model/environment versions、case manifest、events/tool/permission、output summary、score/failure、artifact index。
- **15 分钟｜验证**：checker发现missing required file与hash tamper。
- **必须留下**：CUDA correctness/assignment或blocked evidence、bundle与两项integrity test。

## Day 4 — Independent Lab：Fallback Fixture 与 Failure Trace（105 分钟）

- **65 分钟｜Core independent**：构造/选择一个有意CPU fallback的model/op；保存optimized/profile evidence、provider boundary、correctness与latency，不把fallback本身标成错误。
- **25 分钟｜Sidecar independent**：为tool timeout或environment failure生成端到端trace，含attempt、permission、timeout/retry decision、final status，无secret headers。
- **15 分钟｜evidence**：按Week 30 taxonomy标failure并关联artifact ID；证据不足标unknown。
- **必须留下**：fallback report、boundary evidence、failure trace/label。

## Day 5 — Integration：Provider Report + Inspectable Evidence（105 分钟）

- **60 分钟｜Core integration**：写provider report：model/opset→providers/options→partition/fallback→CPU/CUDA correctness→setup/first/warm timing→limitations；每个claim链接profile/raw data。
- **30 分钟｜Sidecar integration**：运行bundle redaction/integrity/schema checks；离线reviewer应能从index追到case/trace/result。
- **15 分钟｜cross-check**：证明trace不能替代CPU oracle，provider registration不能替代node-assignment evidence。
- **必须留下**：provider report、validated bundle、cross-track conclusion。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：给unseen graph/support table手画partition/boundaries；运行一个unseen input，解释provider evidence与fallback。
- **35 分钟｜Sidecar assessment**：找出bundle中的orphan span、missing hash、secret-like field、score无evidence；修复后checker clean。
- **25 分钟｜Exit ticket**：回答“如何证明CUDA EP实际执行，而不是仅已安装”，冻结Week 32 input/output device/lifetime plan。
- **必须留下**：partition assessment、unseen run、bundle audit与Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M5 为 `PASS` 且 CPU/CUDA/fallback evidence 与 bundle checker 无 backlog 时做；若 Gate 落后、GPU objective仍 `BLOCKED` 或核心 remediation 未清，直接取消，不用 release note替代实测。
- **来源与选择规则**：从 [ONNX Runtime Releases](https://github.com/microsoft/onnxruntime/releases) 选择最新一个与本地 ORT major/minor 匹配、且明确提到 CUDA EP、provider partition/fallback 或 profiling 的稳定版条目；没有版本匹配项就取消。
- **30–45 分钟流程**：5 分钟按 installed version筛选 → 15–20 分钟追 release note 到 PR/docs → 10–15 分钟写 `claim → evidence → scope → decision`；PR描述必须再由本地 profile/fixture验证后才能采用。
- **artifact**：`notes/feed/week-31.md`，含 installed/release versions、上游变更链接、一个 upgrade/test decision。

## Exit Ticket / Unlocks

- **通过条件**：environment可复现；CPU/CUDA correctness；partition/fallback有profile evidence；setup/first/warm分开；bundle可关联、脱敏、完整。
- **解锁**：Week 32 I/O Binding 与 user compute stream。无GPU则保持GPU部分blocked。
