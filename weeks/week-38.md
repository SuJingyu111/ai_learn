# Unit 7 · Week 38 — Precision/Dynamic Validation 与 Human-in-the-loop

> **课程角色**：把 W35 的 FP16/profile 单元测试提升为 beta-level validation，检查
> precision×shape×input class 的交互；量化只总结可迁移 lessons，INT8 engine 仍为选修。
> Sidecar 把 autonomy matrix 变成 material-risk approval points。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W37 beta contract/correctness set；W35 FP32/FP16 thresholds 与 dynamic profile。
- TensorRT materials 必须解析到实际设备 version archive；precision/profile API 不从
  其他版本或 `latest` 搬运。
- **Remediation**：若 beta fixed FP32 未过，Day 1–2 只修该 blocker；若无真实 dynamic
  input，则只验证 fixed shape 并把 dynamic 标 N/A with rationale，不能伪造 coverage。

## Observable objectives（可观察目标）

完成后能够：

1. 运行 input-class×precision×shape matrix 并报告 error distribution/task checks。
2. 检测 FP16 overflow/reformat/profile miss 与 buffer-sizing failures。
3. 从 Q/DQ、rounding、clipping 解释量化风险而不宣称已完成 INT8。
4. 让 material action 在 plan/diff/arguments/blast-radius 可见后才请求 approval。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Validation matrix | 输入类别、precision、shape、expected result 的笛卡尔子集 |
| Profile miss | runtime shape 不被任何 optimization profile 覆盖 |
| Reformat | runtime/engine 为 format compatibility 插入的数据转换 |
| Approval point | action plan 完成后、side effect 发生前的决策点 |
| Material risk | 会改变外部状态、安全/compliance 或难恢复的风险 |
| Quantization lesson | Q/DQ/error 的可解释结论，不等同 INT8 deployment |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin “Accuracy Considerations / Reduced Precision” | 20 | 1 |
| T2 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin “Working with Dynamic Shapes / Optimization Profiles” | 20 | 2 |
| T3 | [MCP Tools, stable revision 2025-11-25](https://modelcontextprotocol.io/specification/2025-11-25/server/tools) — “User Interaction Model” | 15 | 4 |
| T4 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** blueprint/domain/skill mapping；HITL 机制以 T3 为准 | 15 | 5 |

## 带问题阅读

- 何时 max error 会隐藏 tail/task-level regression？
- out-of-range request 应在 allocation、set-shape 还是 enqueue 后才失败？
- approval prompt 若不展示 exact target/arguments，是否真的降低风险？

## Worked example

```text
case boundary_02 × FP16 × 512x512
checks: finite, max_abs, p99_abs, task agreement, profile=in-range, bytes=checked
decision: pass / numerical-fail / profile-fail / environment-blocked
```

“运行固定 CPU test”可自动执行；“修改 Orin power mode”必须展示 current/target mode、
benchmark reason、reboot/thermal implications，再由用户批准。

## 累计项目 increment

- `artifacts/week-38/validation-matrix.csv`
- `artifacts/week-38/quantization-lessons.md`
- `ai-app/evals/approval-cases.md`
- `ai-app/autonomy-policy.md`
- `notes/ai-app/week-38.md`

## 本周必须产出

- beta precision/dynamic validation matrix 与 failure diagnostics。
- quantization worksheet/lessons；INT8 status 明确为 elective/not attempted/experiment。
- approval policy、blocked-action fixture 与 3–5 个 GH-600 scenarios。

## Day 1 — 理论与 precision validation contract（105 分钟）

- **20 分钟**：阅读 T1，列 FP16 rounding/range/overflow/sensitive operations。
- **25 分钟**：冻结 matrix rows 与 max/relative/p99/task thresholds。
- **50 分钟**：运行 FP32/FP16 × normal/boundary inputs，保存 full/summary errors 与
  NaN/Inf checks。
- **10 分钟**：标出 reformat/fallback evidence。

**必须留下**：precision rows、threshold rationale 和 first validation results。

## Day 2 — Guided lab：Dynamic validation（110 分钟）

- **20 分钟**：阅读 T2，复述 build/profile/context shape 顺序。
- **70 分钟**：运行 min/opt/max、两个 in-range non-opt、一个 out-of-range；按实际
  shape sizing/reuse buffer，区分 first-use 与 warmed runs。
- **10 分钟**：检查 wrong rank/dimension diagnostics。
- **10 分钟**：更新 `validation-matrix.csv`。

**必须留下**：multi-shape correctness/latency、buffer policy 和 failure output。

## Day 3 — Independent lab：Quantization lessons（105 分钟）

- **20 分钟**：闭卷写 scale、zero-point、per-tensor/per-channel、Q/DQ。
- **55 分钟**：用小 tensor 演示 round-to-nearest、clipping、scale tradeoff，计算
  max/p99/task proxy error；不 build INT8 engine。
- **20 分钟**：把哪些 lessons 可迁移到本 beta、哪些需要 calibration/QAT evidence 写清。
- **10 分钟**：标记 INT8 为 **选修且非 blocker**。

**必须留下**：`quantization-lessons.md`、数值 worksheet 和 honest status。

## Day 4 — Integration：Approval fixture（110 分钟）

- **15 分钟**：阅读 T3，区分 protocol hint 与 host confirmation。
- **60 分钟**：创建 `ai-app/evals/approval-cases.md`，覆盖 read-only、reversible、
  external side effect、device config、secret、merge；每例列 plan/diff/args/blast radius。
- **25 分钟**：测试 allow-listed benchmark 与 arbitrary shell；前者按 policy，
  后者 deny，不在真实设备执行危险动作。
- **10 分钟**：更新 autonomy policy 的 enforcement owner。

**必须留下**：approval fixtures、allow/ask/deny evidence 和 policy diff。

## Day 5 — Assessment：Matrix defense × HITL（105 分钟）

- **15 分钟**：阅读 T4。
- **55 分钟**：随机抽 6 个 matrix cells 复跑，解释 pass/fail 根因与 evidence。
- **25 分钟**：写 3–5 个 HITL scenarios，解释 approval fatigue 与 under-approval。
- **10 分钟**：更新 GH checklist evidence links。

**必须留下**：sampled rerun、3–5 题解析和 checklist link。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean run representative FP32/FP16/dynamic subset。
- **20 分钟**：口述 quantization errors 与 INT8 仍缺的证据。
- **25 分钟**：写周总结与 remaining precision/profile risks。
- **15 分钟**：冻结 W39 device preprocessing tensors/buffers。

**必须留下**：Week 38 evidence index、risk register 和 W39 buffer manifest。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 每个 supported precision/shape 有明确 oracle 与 buffer rule。
- out-of-range/wrong-rank fail closed，INT8 未被伪装成完成。
- approval 只用于 material actions，且发生在 execution 前。
- **Unlock W39**：beta matrix 中所有 declared-supported cells 通过。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [GitHub Changelog · Copilot](https://github.blog/changelog/label/copilot/)
  只选 1 条会改变 approval、permission 或 tool scope 的更新；Preview 只记录 maturity，
  不进入 required policy 或 Gate evidence。
- **Artifact**：写 `notes/feed/week-38.md`，固定四栏
  `claim → evidence → scope → decision`，并写一条需回归的 HITL fixture。
- **取消规则**：precision/shape matrix 落后或有未关闭 correctness cell 时直接取消。
