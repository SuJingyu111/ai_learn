# Week 21 — Unit 4：Image Sampling 与 ONNX Resize Coordinate Semantics

## Unit / 课程角色

- **Unit 4 · Image Operator Case Study，第三周。** 本周先冻结 resize 的数学语义，不写完整 CPU/GPU production implementation。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：把 Week 17–20 的同一 read-only MCP semantics 搬到 local Streamable HTTP；authorization 留到 Week 22。

## 硬先修与 Remediation

- **硬先修**：能解释 2D row-major/HWC stride、nearest/bilinear 插值、border clamp；Week 20 odd-size correctness 通过。
- **开周诊断（15 分钟）**：不用代码计算一维 `[0,10]` 从 2→1 与 2→4 的 half-pixel source coordinates 和 clamp 后邻点。
- **未通过时**：先复习 linear interpolation `a(1-w)+bw` 与 pixel-center；完成 2→3 手算。不要通过查看仓库 resize 实现来补答案。

## Observable Objectives

1. 精确说明 ONNX Resize 的 `coordinate_transformation_mode`、`nearest_mode`、border/extrapolation 与 axes/shape 约束。
2. 手算 half-pixel、align-corners、asymmetric 在 2→1、2→4、identity 上的 source coordinates。
3. 从 coordinate→neighbors→weights→border→channels/stride 写出 implementation-independent pseudocode。
4. 设计 known-value 与 metamorphic tests，使 rounding、weight、border、layout 错误分别可见。
5. 用 local-only Streamable HTTP 重跑既有 MCP semantics；验证 Origin、protocol version、content type、body limit，明确 session 不等于 authorization。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| pixel center | 用连续坐标表示离散像素中心的位置约定 |
| half-pixel | 由 output center 经 scale 映射到 input continuous coordinate 的模式 |
| align corners | 让首尾 output centers 对齐首尾 input centers 的映射模式 |
| asymmetric | 常见的 `x_original = x_resized / scale` 映射 |
| nearest mode | continuous coordinate 转离散 index 的 rounding/tie 规则 |
| bilinear weights | 由 x/y fractional parts 形成的四邻点权重 |
| Streamable HTTP | MCP 2025-11-25 的 HTTP POST/GET、可选 SSE transport |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [ONNX Resize Operator](https://onnx.ai/onnx/operators/onnx__Resize.html)，读 Summary、Inputs/Outputs、Attributes 中 `coordinate_transformation_mode`、`nearest_mode`、`axes` | 30 |
| 1 | [MCP 2025-11-25 — Transports](https://modelcontextprotocol.io/specification/2025-11-25/basic/transports)，读 stdio、Streamable HTTP、Security Warning、Protocol Version Header | 20 |
| 2 | [ONNX Concepts — Operators](https://onnx.ai/onnx/intro/concepts.html#operators)，读 operator schema、opset/version 的语义 | 15 |
| 2 | [CUDA Best Practices — Numerical Accuracy and Precision](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#numerical-accuracy-and-precision)，读浮点比较与运算差异的限制 | 15 |

## 带问题阅读

- `scales` 与 `sizes` 谁决定 output，冲突/缺失时 contract 怎么写？
- output size 为 1 时，各 coordinate mode 的公式是否有特殊分支？
- nearest 的 tie-breaking 为什么必须成为 test contract？
- bilinear clamp 应在 coordinate、neighbor index 还是 value 层发生？
- Streamable HTTP 的 Origin validation、localhost binding、authorization 分别阻断什么风险？

## Worked Example / 手算

对一维 input `[0,10]`：

- 用 half-pixel 手算 2→1、2→4 的连续坐标、floor/ceil、weights 与 clamp。
- 用 align-corners 手算 2→4，再与 half-pixel 做逐点 diff。
- 对 nearest 的 `round_prefer_floor` 与 `floor` 构造一个能区分的 tie case。
- 对 `2×2` 图 `[[0,10],[20,30]]` 手算一个 bilinear output pixel。
- 画 local HTTP client→single MCP endpoint，标 Origin check、protocol header、MCP handler；authorization 暂为空并注明原因。

## 累计项目 Increment

- `labs/resize_semantics/`：formula notebook、hand tables、pseudocode、known/metamorphic case manifest。
- `ai-app/deployment/http-local/`：loopback Streamable HTTP config 与 stdio parity suite。
- `notes/curriculum/week-21.md`：operator-version、semantics decisions 与 transport boundary。

## 本周必须产出

- 三种 coordinate mode 手算表、nearest tie cases、bilinear known value、语义 decision record。
- local Streamable HTTP run、stdio/HTTP parity、Origin/header/body/session negative tests。
- implementation-independent test manifest 与 Exit ticket。

## Day 1 — Theory：Coordinate Semantics 与 HTTP Transport（105 分钟）

- **30 分钟｜必修理论**：阅读 ONNX Resize 指定章节；把每个相关 attribute 改写为可测试句子。
- **20 分钟｜必修理论**：阅读 MCP Transports stable spec；画 stdio 与 HTTP trust boundary。
- **30 分钟｜worked example**：完成 `[0,10]` 的 half-pixel 2→1/2→4 与 align-corners 2→4。
- **15 分钟｜concept map**：连接 coordinate→rounding/neighbors→weights→border→layout/stride。
- **10 分钟｜retrieval**：闭卷回答 mode、nearest tie、Origin、protocol header。
- **必须留下**：attribute contract、手算表、transport diagram。

## Day 2 — Guided Practice：Operator Version 与 Test Design（105 分钟）

- **15 分钟｜必修理论**：阅读 ONNX operator/opset concepts，记录 schema/version。
- **15 分钟｜必修理论**：阅读数值精度指定章节，定义本周比较方式。
- **40 分钟｜guided lab**：为 identity、2→1、2→4、1×N、N×1、odd/non-uniform、1/3/4 channels 设计 independent expected/metamorphic cases。
- **25 分钟｜HTTP guided**：冻结 loopback、endpoint、Origin allowlist、protocol header、content types、body/request timeout；无公网、无真实 secrets。
- **10 分钟｜检查**：用错误公式手算一例，确认 case 会失败。
- **必须留下**：versioned case manifest、tolerance rationale、HTTP contract。

## Day 3 — Guided → Independent：Reference Calculator 与 HTTP Parity（105 分钟）

- **55 分钟｜Core lab**：在 `labs/resize_semantics/` 写独立 calculator/pseudocode，只覆盖小 tensor；输出 coordinate、neighbors、weights、value，禁止读取现有 resize source。
- **35 分钟｜Sidecar build**：给同一 read-only server 增加 loopback Streamable HTTP；重跑 initialize、resources、prompts、overview。
- **15 分钟｜验证**：diff stdio/HTTP machine-readable semantic results，transport metadata 另报。
- **必须留下**：traceable calculator、expected tables、HTTP command 与 parity diff。

## Day 4 — Independent Lab：Semantic Counterexamples 与 Transport Breaks（105 分钟）

- **60 分钟｜Core independent**：自行构造至少六个 counterexamples，分别暴露 coordinate mode、nearest tie、x/y weight swap、early clamp、channel mix、stride unit 错误。
- **30 分钟｜Sidecar independent**：测试 invalid Origin、missing/wrong protocol version、bad content type、oversized body、unknown session 中至少四项；handler 前拒绝。
- **15 分钟｜证据审阅**：记录每个 case 的唯一预期 failure signal，不接受“图看起来差不多”。
- **必须留下**：六个 diagnostic cases、四项 HTTP negative evidence。

## Day 5 — Integration：Semantic Design Record（105 分钟）

- **55 分钟｜Core integration**：写 `labs/resize_semantics/contract.md`：dtype/layout/stride、coordinate、nearest/bilinear、border、invalid input、alias、tolerance、operator version；无 TBD。
- **35 分钟｜Sidecar integration**：把 contract 作为固定 resource 暴露；client 验证 hash/version，仅引用为 evidence，不执行其中可能混入的指令。
- **15 分钟｜cross-check**：用 resource version 与 test manifest version 交叉检查，不一致则 fail。
- **必须留下**：完整 contract、resource provenance、version-mismatch test。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：闭卷手算一个 unseen 3→5 nearest/bilinear case，指出 ONNX attributes；为一个错误 output 反推最可能语义 bug。
- **35 分钟｜Sidecar assessment**：从 clean process 演示 HTTP parity 和四类 deny；解释 Origin/session/auth 的非等价关系。
- **25 分钟｜Exit ticket**：写出 Week 22 CPU oracle 必须独立实现的最小 API 与 test list。
- **必须留下**：unseen hand case、bug diagnosis、transport assessment 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M3 为 `PASS` 且 resize contract、HTTP parity 与 deny evidence 无 backlog 时做；若 Gate 落后或语义/transport remediation 未清，直接取消。
- **来源与选择规则**：读 MCP 官方博客 [“The 2026-07-28 MCP Specification Release Candidate”](https://blog.modelcontextprotocol.io/posts/2026-07-28-release-candidate/)；只选一个 transport/session breaking-change claim，与本周采用的稳定规范逐条 diff，并始终标记为 release candidate。
- **30–45 分钟流程**：5 分钟选一项 breaking change → 15–20 分钟追 RC/changelog/稳定规范 → 10–15 分钟写 `claim → evidence → scope → decision`；正式版和SDK未验证前不得迁移主线。
- **artifact**：`notes/feed/week-21.md`，含 stable-vs-RC 对照、版本日期、一个 experiment/defer 决策。

## Exit Ticket / Unlocks

- **通过条件**：手算与 contract 一致；test 能分别暴露六类错误；HTTP 仅 loopback、Origin/header/limit 有证据，且未误称已授权。
- **解锁**：Week 22 独立 CPU Resize Oracle。
