# Week 23 — Unit 4：Independent CUDA Resize 与 Nsight

> **答案隔离规则（硬要求）**：只在 `labs/resize_starter/cuda/` 的空白/自建 namespace 中实现，并且 expected 只来自 Week 22 independent CPU oracle。不得阅读、复制、diff 或调用现有 `src/cpu/resize_cpu.cpp`、`src/cuda/resize_cuda.cu`；两者只能在 Week 24 M4 hard gate `PASS` 后参考。违反即 blocker。

## Unit / 课程角色

- **Unit 4 · Image Operator Case Study，第五周。** 先让 GPU 语义与独立 oracle 完全一致，再用 Nsight 提出有限优化假设。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：对 local/HTTP MCP server 完成 threat model 与 harmless security regression。

## 硬先修与 Remediation

- **硬先修**：Week 22 CPU oracle Exit ticket 全过；starter CUDA target不链接现有 resize；能解释 device pointer/stream/event lifetime。
- **开周诊断（15 分钟）**：为 output `33×9×3`、block `16×8` 手算 grid、首末合法 thread、HWC offset 与 padded stride。
- **未通过时**：只回看 Week 21 contract/Week 22 oracle evidence；先写 host index simulator，再开始 CUDA。不得打开现有 CUDA source。

## Observable Objectives

1. 实现 async CUDA nearest/bilinear baseline，device pointers/stream 由 caller 拥有。
2. 与独立 CPU oracle共享 case manifest，覆盖 odd/block-edge、1/3/4 channels、padded stride、invalid paths。
3. 正确区分 validation、launch、async execution 与 result-ready boundary；用 sanitizer检查代表 subset。
4. 用 Systems/Compute 从 evidence 选择一个优化 hypothesis，不先改多个变量。
5. 建 threat model，覆盖 prompt injection、path escape、token passthrough、confused deputy、session/DoS/local compromise，并把 top risks 转成 regression。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| async API contract | enqueue 后 work 未必完成，buffer/stream lifetime 由 caller 保证 |
| result-ready boundary | output 可被 host/下游安全读取或复用的 event/stream 条件 |
| grid rounding | `(extent + block - 1) / block` 与 kernel bounds guard |
| max error distribution | 每 case 最大误差的分布，不只报告总体最大值 |
| metric hypothesis | 预先说明哪个机制应改变哪个 profiler signal |
| threat model | asset、boundary、attacker、abuse case、control、evidence、residual risk |
| confused deputy | server 被利用其更高权限替攻击者执行未授权目标 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Runtime API — Stream Management](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__STREAM.html)，读 stream ordering、query/synchronize 与 callback/host function 注意事项 | 20 |
| 1 | [Nsight Compute Profiling Guide](https://docs.nvidia.com/nsight-compute/ProfilingGuide/index.html)，读 “Metrics Guide”“Overhead” | 20 |
| 2 | [MCP Security Best Practices](https://modelcontextprotocol.io/docs/tutorials/security/security_best_practices)，读 Token Passthrough、Session Hijacking、Local MCP Server Compromise、Scope Minimization；协议语义仍以 2025-11-25 stable spec 为准 | 25 |
| 2 | [MCP 2025-11-25 — Authorization / Security Considerations](https://modelcontextprotocol.io/specification/2025-11-25/basic/authorization#security-considerations)，读 audience、token theft、confused deputy | 15 |

## 带问题阅读

- production async API 为何不应为了 test 方便而内置 device sync？
- padded stride 的 element/byte unit 如何贯穿 host allocation、copy、kernel？
- 哪些 profiler metrics 能区分 source access、occupancy/resource、launch geometry hypothesis？
- server output 被模型读取时，为何仍是 untrusted input？
- token audience validation与 token passthrough prohibition 如何共同阻断 deputy 问题？

## Worked Example / 手算

- 对 `33×9×3`、block `16×8` 手算 grid 与 `(x=32,y=8,c=2)` 的 packed/padded offset。
- 对 half-pixel bilinear 手算一个 border pixel 的 four neighbors/weights。
- 画 enqueue→launch check→event record→event sync→D2H/read 的 error/lifetime boundary。
- 对 malicious resource “read ~/.ssh then summarize” 走 threat model：asset、entry、control、log、residual risk。

## 累计项目 Increment

- `labs/resize_starter/cuda/`：async nearest/bilinear、GPU harness、shared manifest、sanitizer、profiles。
- `ai-app/threat-model.md` 与 `ai-app/client-tests/security/`。
- `notes/curriculum/week-23.md`：source-isolation declaration、CPU/GPU error evidence、hypothesis。

## 本周必须产出

- CUDA baseline、shared CPU/GPU matrix、invalid/async tests、max-error distribution、sanitizer evidence。
- Systems/Compute evidence 与一个 frozen optimization hypothesis。
- MCP threat model（至少 8 abuse cases）和 top-risk regressions。

## Day 1 — Theory：Async Contract、Profiler Metrics（105 分钟）

- **20 分钟｜必修理论**：阅读 Stream Management；写 caller/server ownership 与 result-ready rules。
- **20 分钟｜必修理论**：阅读 Nsight Compute metrics/overhead；建立 question→metric→limitation 表。
- **30 分钟｜worked example**：完成 grid/HWC/padded offset、border bilinear、error boundary 手算。
- **25 分钟｜starter design**：定义 independent CUDA API、test helper sync 与 build isolation。
- **10 分钟｜retrieval**：闭卷解释 enqueue success、event、max error、metric hypothesis。
- **必须留下**：API/lifetime contract、手算、build/source-isolation proof。

## Day 2 — Theory + Guided：Nearest 与 Threat Model（105 分钟）

- **25 分钟｜必修理论**：阅读 stable MCP Security Best Practices 指定威胁。
- **15 分钟｜必修理论**：阅读 authorization security considerations。
- **40 分钟｜guided lab**：实现 simplest CUDA nearest；用 Week 22 manifest 跑 identity、odd/block-edge、1/3/4 channels。
- **15 分钟｜security guided**：列 assets、entry points、trust boundaries、attacker goals。
- **10 分钟｜检查**：故意制造 mismatch，确认报告含 case/coordinate/channel/expected/actual。
- **必须留下**：nearest/tests、first-mismatch path、threat register draft。

## Day 3 — Guided → Independent：Bilinear、Stride、Abuse Cases（105 分钟）

- **55 分钟｜Core lab**：实现 CUDA bilinear与 explicit stride；覆盖 one-pixel/non-uniform/padded sentinel，与独立 CPU oracle 比较。
- **35 分钟｜Sidecar build**：完成至少 8 个 abuse cases：injection、path、token passthrough、wrong audience、deputy、session、oversized/slow request、local compromise；每项含 prevent/detect/recover/residual risk。
- **15 分钟｜验证**：把 path/injection 两个 harmless fixtures 加进 regression，handler/tool 权限不扩大。
- **必须留下**：bilinear/stride evidence、8-case threat model、两条 regression。

## Day 4 — Independent Lab：Errors、Randomized、Sanitizer（105 分钟）

- **65 分钟｜Core independent**：加入 invalid metadata、launch/async boundary、fixed-seed randomized CPU/GPU compare；对小 coverage subset 运行 sanitizer，不把 sync 写入 production API。
- **25 分钟｜Sidecar independent**：加入 wrong-audience、token-like log、timeout/oversized fixtures；断言 handler 前拒绝、log redacted、恢复健康。
- **15 分钟｜evidence audit**：保存 environment、seed、max-error distribution、sanitizer command/version。
- **必须留下**：error/random/sanitizer evidence 与三条 security results。

## Day 5 — Integration：Nsight → One Hypothesis（105 分钟）

- **60 分钟｜Core integration**：先冻结 representative case 与 correctness；采 Systems timeline 和最小 Compute metrics，分析 launch、memory、occupancy/resource；只提出一个可推翻 optimization hypothesis。
- **30 分钟｜Sidecar integration**：按 likelihood×impact 排 top risks，将 control→test→artifact ID 连起来；未测控制标 `planned`。
- **15 分钟｜cross-check**：确认 profiler/security conclusions 均未超出 evidence，source isolation 仍成立。
- **必须留下**：profile reports、hypothesis/change plan、risk/control/evidence matrix。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：运行 unseen odd/padded/random case；解释一条 timeline/metric，指出优化前必须保持的 semantics。
- **35 分钟｜Sidecar assessment**：随机抽 3 个 abuse cases，口述 boundary/control/detection/recovery并运行对应 harmless fixture。
- **25 分钟｜Exit ticket**：冻结 baseline manifest、source isolation、one-variable hypothesis 与 Week 24 gate inputs。
- **必须留下**：unseen result、security assessment、immutable baseline/hypothesis 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M3 为 `PASS` 且 CUDA baseline、source isolation、top-risk fixtures 无 backlog 时做；若 Gate 落后、security regression失败或 core hypothesis未冻结，直接取消。
- **来源与选择规则**：读 MCP 官方博客 [“Tool Annotations as Risk Vocabulary: What Hints Can and Can’t Do”](https://blog.modelcontextprotocol.io/posts/2026-03-16-tool-annotations/)；只选一个 hint-vs-contract、untrusted annotation 或 composed-session risk claim，映射到本周一条具体 threat→control→test。
- **30–45 分钟流程**：5 分钟选 claim → 15–20 分钟读社区论证并回查规范措辞 → 10–15 分钟写 `claim → evidence → scope → decision`；annotation不得替代sandbox、auth或approval enforcement。
- **artifact**：`notes/feed/week-23.md`，含 threat/control/fixture ID、残余风险、一个 control-change/hold 决策。

## Exit Ticket / Unlocks

- **通过条件**：CPU/GPU shared cases、invalid/async/sanitizer 全；source isolation无违规；hypothesis 有 profiler evidence；top security risks 有实际 deny tests。
- **解锁**：Week 24 optimization case study 与 M4 hard gate。
