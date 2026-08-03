# Week 09 — ONNX IR、Opset、Export 与 Typed Tool Contract

> **Unit 2 · Model-to-Runtime CPU Vertical Slice（第 3/6 周）**  
> **课程角色**：把 framework model 变成可检查的交换图，并冻结 M2 的 model/runtime-input/preprocess scope；sidecar 把 agent 的抽象 tool 变成 typed、read-only contract，但不提前进入 MCP wire protocol。

## 开始前：硬先修与 remediation

- **硬先修**：Week 07 checkpoint 可加载，Week 08 能解释 inference graph 与 training graph 边界。
- **先修检查**：固定 model 的 input name/shape/dtype 与 output contract 必须写得出。
- **失败时 remediation**：先 clean-run checkpoint→eval→fixed output；若无法固定 contract，不导出 ONNX，只修 source model。

## Observable objectives

- 解释 ONNX model/graph/node/value/initializer/operator 与 opset。
- 冻结 export contract，并运行 checker。
- 检查 graph inputs/outputs/nodes/initializers/known shapes。
- 用 evidence 说明 Module 不等于 ONNX node、node 不等于 runtime kernel。
- 冻结 model、runtime input tensor、dataset、normalize/layout 与 planned source→model shape；将 Resize coordinate/pixel-center/border/rounding/tolerance 明确 deferred 到 W21/M4。
- 定义 typed read-only tool 的 input/output/errors/non-goals。

## 术语表

需能解释：**ONNX、IR、graph、node、value info、initializer、operator、opset、export contract、shape inference、checker、runtime input tensor、dataset manifest、planned source shape、deferred Resize contract、tool schema、validation、structured error**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[ONNX Concepts](https://onnx.ai/onnx/intro/concepts.html) — model、graph、node、initializer、opset。
- **T2（Day 2，20 分钟）**：[ONNX IR Specification](https://onnx.ai/onnx/repo-docs/IR.html) — Models、Graphs、Nodes、Names。
- **T3（Day 3，20 分钟）**：[PyTorch ONNX exporter](https://docs.pytorch.org/docs/stable/onnx.html) — exporter overview、export API、verification。
- **S1（Day 1，20 分钟）**：[Tooling, MCP, and Agent Execution Environments](https://learn.microsoft.com/en-us/training/modules/agent-tooling-mcp-execution-environments/) — 本周只读“How agents interact with GitHub APIs and workflows”及 execution context/boundaries；跳过 MCP server/registry 单元，协议从 W13 开始。

80 分钟计入 Day 1–3；本周不记录或实现 MCP protocol。Podcast/blog 仅 optional。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **先修或 Gate 落后时取消**：从 [The Gradient](https://thegradient.pub/) 选择一篇有原始论文/代码链接、主题为 model representation、inference 或 evaluation 的文章；优先近 18 个月内容。
- 在 `notes/feed/week-09.md` 写 `claim → source/evidence → scope → experiment/decision`，并说明文章 claim 能否由 ONNX graph inspection 验证。不可把评论文章当 ONNX 规范，不计 unlock。

## 带问题阅读

- initializer 与 runtime input 的 ownership/lifetime 有何不同？
- opset 描述 operator 语义版本，为什么不等于 ONNX IR 版本？
- exporter 为什么可能将一个 Module 变成多个 nodes 或一个 Gemm？
- 为什么本周可冻结 runtime input tensor/normalize/layout，却不能假装已经理解并冻结 Resize coordinate semantics？
- schema validation 与 filesystem/tool permission 各解决什么？

## Worked example / 手算

逻辑图 `Y=Relu(MatMul(X,W)+B)`：`X=(1,3)`、`W=(3,2)`、`B=(2,)`，所以 MatMul=`(1,2)`、Add broadcast 后=`(1,2)`、Y=`(1,2)`；W/B 是 initializers，X 是 graph input。实际 exporter 可能用 `Gemm`，必须以 inspect 输出为 evidence。

## 累计项目 increment

- `labs/ml/week09_export_onnx.py`
- `labs/ml/week09_inspect_onnx.py`
- `artifacts/models/week09_model.onnx`（可重复生成）
- `artifacts/contracts/m2-input-preprocess-scope.md`
- `ai-app/schemas/get_project_overview.schema.json`
- `notes/concepts/onnx-ir-export.md`

## Exit ticket / unlocks

**形成性**：checker 通过、graph summary 完整、module→graph mapping 有实际证据；M2 scope 已冻结 model/runtime tensor/dataset/normalize/layout/planned source shape 并明确 Resize deferred；typed schema 拒绝 wrong type/extra field/path。完成 unlock Week 10。

## Day 1

**阶段：理论 + graph vocabulary（100 分钟）**

- **20 分钟**：读 T1。
- **25 分钟**：逐步做顶部 graph shape/initializer worked example。
- **35 分钟**：从 Week 07 model 预测 nodes/initializers/input-output。
- **20 分钟**：读 S1 指定的非 MCP 单元，为普通 typed `get_project_overview` 初拟 name/description/input/output。
- **必须留下**：graph prediction、shape calculation 和 tool contract 草案。

## Day 2

**阶段：worked IR inspection（100 分钟）**

- **20 分钟**：读 T2。
- **25 分钟**：手写一个三节点 graph 表：name/domain/op_type/inputs/outputs。
- **40 分钟**：guided 创建 inspect script，打印 graph metadata 与 parameter bytes。
- **15 分钟**：为 tool schema 写 allow/no-path/no-shell/non-goals。
- **必须留下**：IR 表、inspect output 和 permission boundary。

## Day 3

**阶段：guided export lab（100 分钟）**

- **20 分钟**：读 T3。
- **60 分钟**：冻结 model/name/shape/dtype/opset/seed、runtime input tensor、dataset manifest、normalize/layout 与 planned source→model shape；创建 export script，保存 reference input/output，运行 ONNX checker，并写明 Resize semantics deferred 到 W21/M4。
- **10 分钟**：给 schema 加 `additionalProperties:false` 与 structured error。
- **10 分钟**：记录 framework/ONNX/opset versions。
- **必须留下**：export script、checker output、contract 和 schema version。

## Day 4

**阶段：independent graph experiment（100 分钟）**

- **70 分钟**：独立做 module→node mapping；改变一个 model dimension/operation 后重导出并 diff，解释差异。
- **20 分钟**：为 schema 写 valid、wrong type、extra field、path traversal fixtures。
- **10 分钟**：clean-run export→inspect。
- **必须留下**：mapping、graph diff、四个 schema fixtures。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：完成 ONNX concept note与 M2 scope audit；核对 initializer bytes 与 Week 07 parameter table，不强求 node count，也不声称 G2 已有 CPU/CUDA Resize oracle。
- **20 分钟**：执行/纸面验证 tool fixtures，记录 expected/actual/blocked。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：concept note、schema result table 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：clean process export→checker→inspect→mapping。
- **25 分钟**：闭卷解释 IR/opset/operator/module/node/kernel。
- **15 分钟**：说明 typed schema 能/不能保证什么。
- **15 分钟**：填写 `notes/weekly/week-09.md` 与 Week 10 provider question。
- **必须留下**：形成性 Exit ticket、clean commands、limitations 和 unlock 状态。
