# Week 10 — ONNX Runtime CPU、Providers、Graph Optimization 与 Structured Schema

> **Unit 2 · Model-to-Runtime CPU Vertical Slice（第 4/6 周）**  
> **课程角色**：建立首个真实 inference runtime baseline；输入边界沿用 W9 冻结的 runtime tensor/normalize/layout scope，Resize semantics 仍 deferred；sidecar 为 typed tool 冻结机器可检查的 success/error envelopes。

## 开始前：硬先修与 remediation

- **硬先修**：Week 09 ONNX checker 通过，reference input/output 可加载。
- **先修检查**：能准确说出 model input/output names、dtype、shape、opset。
- **失败时 remediation**：先重跑 export/checker；任何 graph contract 不确定都在 ORT session 创建前修复，不靠换 provider 绕过。

## Observable objectives

- 创建 ORT session，明确 providers，并验证实际 provider list。
- 比较 PyTorch 与 ORT CPU output，报告 abs/rel/max error。
- 区分 session creation、optimization、first run、warm-up、steady state。
- 观察 graph optimization 但不把 node 数变化直接等同性能。
- 证明 ORT 接收的是已冻结 runtime input tensor，不把尚未实现的 source-image Resize 混入 G2 claim。
- 定义 structured success/validation/not-found/permission/internal outputs。

## 术语表

需能解释：**runtime、session、execution provider、provider priority/fallback、graph optimization、session creation、first run、warm-up、steady state、structured output、error code、retryable**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[ONNX Runtime: Get started with Python](https://onnxruntime.ai/docs/get-started/with-python.html) — install、InferenceSession、run。
- **T2（Day 2，20 分钟）**：[ONNX Runtime Execution Providers](https://onnxruntime.ai/docs/execution-providers/) — provider abstraction 与 priority。
- **T3（Day 3，20 分钟）**：[ORT Graph Optimizations](https://onnxruntime.ai/docs/performance/model-optimizations/graph-optimizations.html) — basic/extended/layout 与 offline mode。
- **S1（Day 1，20 分钟）**：[JSON Schema: Creating your first schema](https://json-schema.org/learn/getting-started-step-by-step) — types、properties、required、validation。

80 分钟计入 Day 1–3；Podcast/blog 不作教材。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **先修或 Gate 落后时取消**：从 [MLOps Community Podcast](https://mlops.community/podcast) 选择一集有 show notes、主题为 model serving、inference runtime 或 observability 的节目；优先能指出具体 workload/metric 的一集。
- 在 `notes/feed/week-10.md` 写 `claim → source/evidence → scope → experiment/decision`，并标明它讨论的是 model-only、runtime-only 还是 end-to-end latency。不计 unlock。

## 带问题阅读

- 指定 provider 与实际 session provider list 为何都要记录？
- graph optimization 发生在哪个生命周期阶段？
- first run 为什么不能混进 steady-state latency？
- source image、planned source→model shape 与 ORT runtime input tensor 的边界分别在哪里？
- protocol error、tool error、domain validation error 应如何区分？

## Worked example / 手算

请求 provider priority=`["CUDAExecutionProvider","CPUExecutionProvider"]`，但环境只装 CPU package：不能声称 CUDA 被使用；必须检查 `session.get_providers()`/available providers，并将缺失 CUDA 作为 environment fact。M2 本周明确指定 CPU provider，避免 silent fallback 混淆 baseline。

## 累计项目 increment

- `labs/ml/week10_ort_cpu.py`
- `artifacts/results/week10_ort_correctness.json`
- `ai-app/schemas/tool-result.schema.json`
- `ai-app/evals/week-10-schema-fixtures.jsonl`
- `notes/concepts/ort-session-provider.md`

## Exit ticket / unlocks

**形成性**：CPU provider 明确、correctness/failure cases 通过、timing boundaries 能解释；报告只声称 frozen runtime tensor 的 ORT correctness，不声称 Resize oracle；五类 output envelopes 有 valid/invalid fixtures。完成 unlock Week 11。

## Day 1

**阶段：理论 + runtime bootstrap（100 分钟）**

- **20 分钟**：读 T1。
- **25 分钟**：逐步做 provider worked example，写 expected vs observed。
- **35 分钟**：guided 创建 CPU `InferenceSession`，打印 providers、inputs/outputs，跑 fixed input。
- **20 分钟**：读 S1，定义 success/error envelope 的基础 schema。
- **必须留下**：session bootstrap、provider evidence 和 schema skeleton。

## Day 2

**阶段：worked provider/correctness（100 分钟）**

- **20 分钟**：读 T2。
- **25 分钟**：手画 requested→available→selected provider decision。
- **40 分钟**：与 PyTorch reference 比较 max abs/rel error；测试 wrong name/dtype/shape。
- **15 分钟**：定义 validation/not-found/permission error codes。
- **必须留下**：provider decision、correctness report、三种 failures。

## Day 3

**阶段：guided optimization lab（100 分钟）**

- **20 分钟**：读 T3。
- **60 分钟**：分别记录 optimization setting、session creation、optimized graph 可观察信息和 output equality；不做性能结论。
- **10 分钟**：补 internal error/retryable 字段。
- **10 分钟**：保存 ORT/version/config。
- **必须留下**：optimization comparison、correctness 和五类 envelopes。

## Day 4

**阶段：independent timing lab（100 分钟）**

- **70 分钟**：独立测 session creation、first run、10 warm-ups、100 steady runs；validation/logging 移出 timed region，保存 raw samples。
- **20 分钟**：创建 valid/invalid JSONL fixtures：missing field、extra field、wrong enum、contradictory retryable。
- **10 分钟**：计算初步 median/p95，注明方法。
- **必须留下**：raw timing、timing boundary 和 schema fixtures。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：clean export→ORT correctness/failures/timing；写 session/provider concept note，并核对 input 来自 W9 frozen runtime-tensor fixture、Resize 仍 deferred。
- **20 分钟**：验证 fixtures fail closed，permission error 不标可重试。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：vertical slice 首版、fixture results 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：用新 fixed input 重跑 correctness 与 timing boundaries。
- **25 分钟**：闭卷解释 provider/optimization/lifecycle。
- **15 分钟**：解释 structured schema 不能替代 authorization。
- **15 分钟**：填写 `notes/weekly/week-10.md` 与 Week 11 statistics plan。
- **必须留下**：形成性 Exit ticket、raw evidence、limitations 和 unlock 状态。
