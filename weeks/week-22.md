# Week 22 — Unit 4：Independent CPU Resize Oracle

> **答案隔离规则（硬要求）**：本周只在 `labs/resize_starter/` 的空白/自建 namespace 中实现。不得阅读、复制、diff 或调用现有 `src/cpu/resize_cpu.cpp`；它只能在 Week 24 M4 hard gate `PASS` 后用于 post-gate comparison。违反即 reproducibility/academic-integrity blocker。

## Unit / 课程角色

- **Unit 4 · Image Operator Case Study，第四周。** 把 Week 21 数学语义转成可信 CPU oracle；它将成为 Week 23 CUDA correctness 的唯一 expected source。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：在 local HTTP transport 上设计 MCP authorization architecture 与 synthetic deny tests；不保存真实 token。

## 硬先修与 Remediation

- **硬先修**：Week 21 contract 无 TBD；能独立手算 nearest/bilinear；starter namespace 已为空且未链接现有 resize implementation。
- **开周诊断（15 分钟）**：从空白写出 `map_coordinate` 与 `sample_bilinear` pseudocode，并对 2×2→3×3 center pixel 手算。
- **未通过时**：只回看 Week 21 contract/ONNX spec/手算；不得查看 `src/cpu/resize_cpu.cpp`。先补三组 expected tables再进入 Day 1。

## Observable Objectives

1. 在独立 namespace 实现 CPU nearest/bilinear oracle，语义完全来自 Week 21 contract。
2. 支持 1/3/4 channels、explicit padded stride、one-pixel/odd/non-uniform sizes。
3. 对 null/zero/negative/short stride/alias/overflow risks定义并测试稳定失败行为。
4. 建立 known-value、metamorphic、fixed-seed randomized 与 ASan/sentinel evidence。
5. 画 MCP HTTP authorization discovery/flow；验证 missing/expired/wrong-audience/insufficient-scope 在 handler 前 fail closed。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| independent oracle | 未调用/模仿待测实现、由 contract 与手算验证的 expected source |
| active region | destination 中属于实际 pixels/channels 的可写区域 |
| padded stride | 每行 storage 大于 active elements，padding 必须保持 sentinel |
| metamorphic test | 不需逐值 oracle、利用 identity/constant 等不变量检查 |
| alias policy | source/destination storage 重叠是否允许的明确 API 规则 |
| protected resource metadata | MCP server 用于声明 authorization server location 的 OAuth metadata |
| audience binding | token 必须明确发给当前 MCP resource server |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [ONNX Resize Operator](https://onnx.ai/onnx/operators/onnx__Resize.html)，复读 Inputs/Outputs、coordinate/nearest/extrapolation attributes | 20 |
| 1 | [AddressSanitizer Documentation](https://clang.llvm.org/docs/AddressSanitizer.html)，读 “Usage”“Introduction” 与 limitations | 15 |
| 2 | [MCP 2025-11-25 — Authorization](https://modelcontextprotocol.io/specification/2025-11-25/basic/authorization)，读 Roles、Authorization Server Discovery、Authorization Flow Steps | 25 |
| 2 | [MCP 2025-11-25 — Authorization / Security Considerations](https://modelcontextprotocol.io/specification/2025-11-25/basic/authorization#security-considerations)，读 audience、token theft、PKCE、communication security | 15 |

## 带问题阅读

- oracle 如何证明 expected 不是由待测算法反向生成？
- padded source/destination stride 的单位、sentinel 与 allocation size 如何一致？
- huge dimensions 的乘法溢出应在哪一步拒绝？
- stdio 与 HTTP 的 credential model 为什么不同？
- protected resource metadata、authorization server metadata、access token validation 各由谁执行？

## Worked Example / 手算

- 对 `2×2` HWC image（三 channels 各用不同 pattern）手算 3×3 center/corner。
- 画 source stride 8 elements、active width×channels 6 的两行 storage；标 padding sentinel。
- 对 `height*stride*sizeof(float)` 写 overflow-safe check 顺序。
- 画 401→protected resource metadata→authorization server discovery→PKCE authorization→token with resource→MCP request；标 wrong audience 拒绝点。

## 累计项目 Increment

- `labs/resize_starter/cpu/`：独立 header/source/tests；namespace 与 build target 不链接现有 resize。
- `labs/resize_starter/contracts/`：frozen semantics、known cases、random seed、source-isolation declaration。
- `ai-app/architecture/authorization-flow.md` 与 `ai-app/client-tests/auth-fixtures/`。

## 本周必须产出

- CPU nearest/bilinear oracle、known/metamorphic/randomized/stride/invalid tests、ASan/sentinel evidence。
- source-isolation declaration 与 file/build dependency check。
- MCP auth sequence、scope/audience matrix、四类 synthetic deny results。

## Day 1 — Theory：Oracle Integrity、Memory Safety（105 分钟）

- **20 分钟｜必修理论**：复读 ONNX Resize 指定内容，只引用 frozen Week 21 contract/version。
- **15 分钟｜必修理论**：阅读 ASan Usage/Introduction；写它能查与不能证明的内容。
- **30 分钟｜worked example**：完成 2×2×3→3×3 known values、padded layout、overflow check。
- **25 分钟｜starter setup**：创建/审阅独立 target、namespace 与 include dependencies；证明不链接现有 `resize_cpu.cpp`。
- **15 分钟｜retrieval**：闭卷回答 oracle、stride、alias、overflow。
- **必须留下**：hand table、starter dependency proof、source-isolation declaration。

## Day 2 — Theory + Guided：Known Cases 与 Authorization（105 分钟）

- **25 分钟｜必修理论**：阅读 MCP Authorization Roles/Discovery/Flow。
- **15 分钟｜必修理论**：阅读 Security Considerations 指定内容。
- **40 分钟｜guided lab**：从 contract 实现 coordinate helper 与 nearest，先通过 identity、2→1、2→4、one-pixel known cases。
- **15 分钟｜auth guided**：画 sequence 与 scope/audience matrix；local stdio 不套用 HTTP auth。
- **10 分钟｜检查**：故意改变一个 expected，确认 test 能失败后恢复。
- **必须留下**：nearest implementation/tests、auth diagram、scope/audience matrix。

## Day 3 — Guided → Independent：Bilinear 与 Synthetic Auth Denies（105 分钟）

- **55 分钟｜Core lab**：实现 bilinear；覆盖 1/3/4 channels、one-pixel、odd/non-uniform、constant/identity；failure 输出 coordinate/channel/expected/actual。
- **35 分钟｜Sidecar build**：创建 missing、expired、wrong-audience、insufficient-scope synthetic fixtures；handler/resource-read counter 必须保持 0。
- **15 分钟｜验证**：检查 response/log 不泄露 token fixture；合法 synthetic token case只允许 `runtime.read`。
- **必须留下**：bilinear matrix/max error、四类 deny evidence、redacted log。

## Day 4 — Independent Lab：Stride、Invalid、Randomized（105 分钟）

- **65 分钟｜Core independent**：实现 explicit source/destination stride；加入 sentinel、null/zero/negative/short stride/alias/overflow tests 与 fixed-seed randomized cases，运行 ASan（可用时）。
- **25 分钟｜Sidecar independent**：测试 metadata URL/redirect/resource mismatch 的静态 fixtures；不发真实网络 credential，不接受 token passthrough。
- **15 分钟｜evidence audit**：保存 seed、首个 failure reproduction、ASan command/version、known limitations。
- **必须留下**：stride/invalid/random suite、ASan/sentinel、auth metadata negative results。

## Day 5 — Integration：CPU Oracle Gate（105 分钟）

- **60 分钟｜Core integration**：clean build/run known→metamorphic→random→ASan；对每类 contract 条目建立 test coverage matrix；禁止查看现有 implementation。
- **30 分钟｜Sidecar integration**：用 local HTTP server 的 verifier boundary 跑 401/discovery/deny fixtures；无真实 IdP 时明确 `synthetic-only`。
- **15 分钟｜交叉检查**：hash frozen contract、starter source、case manifest 与 auth policy，生成本周 artifact index。
- **必须留下**：CPU oracle gate、coverage matrix、auth transcript、artifact hashes。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：运行 unseen odd/padded/random cases；手算其中一个失败定位；检查 build graph 无现有 resize source。
- **35 分钟｜Sidecar assessment**：闭卷重画 auth flow并解释 PKCE、resource parameter、audience、scope、token passthrough；演示 wrong-audience deny。
- **25 分钟｜Exit ticket**：冻结 CPU oracle API、test manifest 与 source-isolation evidence，列 Week 23 CUDA implementation 的只读输入。
- **必须留下**：unseen results、dependency audit、auth assessment 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M3 为 `PASS` 且 CPU-oracle/source-isolation/auth fixtures 无 backlog 时做；若 Gate 落后、wrong-audience deny失败或 remediation 未清，直接取消。
- **来源与选择规则**：读 MCP 官方博客 [“Enterprise-Managed Authorization: Zero-touch OAuth for MCP”](https://blog.modelcontextprotocol.io/posts/enterprise-managed-auth/)；只选一个 centralized policy、identity assertion 或 audit claim，并回查对应稳定 extension/spec；不连接真实 IdP、不生成真实 credential。
- **30–45 分钟流程**：5 分钟选 auth claim → 15–20 分钟画 blog→spec flow 与 trust boundaries → 10–15 分钟写 `claim → evidence → scope → decision`；vendor adoption 不是本地安全证明。
- **artifact**：`notes/feed/week-22.md`，含角色/issuer/audience/scope边界、synthetic evidence、一个 architecture/defer 决策。

## Exit Ticket / Unlocks

- **通过条件**：CPU oracle correctness/stride/invalid/random/ASan evidence 全；source isolation 无违规；auth fixtures fail closed、无 token 泄漏。
- **解锁**：Week 23 独立 CUDA Resize。不得解锁 post-gate reference comparison。
