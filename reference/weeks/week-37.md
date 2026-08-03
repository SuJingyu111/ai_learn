# Unit 7 · Week 37 — End-to-end Beta Contract 与 Autonomy Levels

> **课程角色**：Unit 7 capstone 启动课。M2 只冻结 model/input/dataset 与
> normalize/layout contract；exact Resize semantics 和独立 CPU/CUDA oracle 来自
> M4/G4，M6 再提供 direct-device contract。本周组合这些既有证据为可验收 beta，
> 不重新选模型或改 preprocessing。Sidecar 将 agent actions 按 material risk 划分 autonomy。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：M2 的 model/input/dataset/normalize/layout manifest；M4/G4 的 exact
  Resize mapping/border semantics 与独立 CPU/CUDA oracle；M6 的 direct-device path、
  TensorRT version pin 和 correctness evidence，且 M6 已 PASS。
- TensorRT/Jetson 文档继续使用设备实际 JetPack/L4T/TensorRT archive；engine 或设备
  stack 变化先 invalidate M6，禁止以 `latest` 或桌面 GPU 结果补缺。
- **Remediation**：M6 未过时，本周 Day 1–3 只关闭 blocker；缺 M2、M4/G4 或 M6
  frozen artifact 时从已有 tests/commits 恢复并 hash，不重新定义更容易的 workload。

## Observable objectives（可观察目标）

完成后能够：

1. 写出有 model/input/preprocess/postprocess/acceptance/non-goal 的 beta contract。
2. 从 clean build/deploy 路径运行三类 inputs 并保存 intermediate/final correctness。
3. 区分 read-only、reversible、external-side-effect、irreversible actions 的 autonomy。
4. 证明高风险动作在 execution 前被 host/policy gate，而非只靠 prompt。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Beta contract | 可运行但仍有已知限制的冻结 scope、interfaces 与 acceptance |
| Acceptance criterion | 可由 test/artifact 观察的 pass/fail 条件 |
| Non-goal | 明确不进入本阶段，不能暗中扩张的能力 |
| Regression budget | 相对冻结 baseline 允许的正确性/性能变化边界 |
| Autonomy level | 由 action risk 决定的 allow/audit/ask/deny |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 实际版本 guide 的 “Engine Compatibility / Performing Inference” | 15 | 1 |
| T2 | [Jetson Linux Developer Guide](https://docs.nvidia.com/jetson/) — 进入与实际 L4T 相同 archive，读首页 “Devices Supported / Release scope” | 15 | 2 |
| T3 | [GitHub Copilot Agents application card](https://docs.github.com/en/copilot/responsible-use/agents) — “Key features / Limitations / Safety components and mitigations” 的 human oversight、permission 与 constrained execution | 15 | 4 |
| T4 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** blueprint/domain/skill mapping，不作为 autonomy/guardrail 教材 | 5 | 4 |

## 带问题阅读

- beta 的 acceptance 与最终 release criteria 有何不同？
- 为什么换一个“更稳定”的模型会破坏累计证据？
- 哪些 actions 的 material risk 真正值得 human approval？

## Worked example

```text
M2: model_hash=M, input_set_hash=I, dataset_hash=D, normalize_layout_hash=NL
M4/G4: exact_resize_semantics_hash=R, cpu_cuda_oracle_hash=O
M6: direct_device_contract_hash=DD
beta path: frame -> CUDA preprocess -> device tensor -> TensorRT -> postprocess
accept: 3 input classes pass; no unexplained copy; max/task error <= frozen threshold
non-goals: camera, DLA, plugin, arbitrary benchmark execution
```

Agent 可自动读取 manifest；运行 allow-listed CPU test 可 audit；修改 Orin power mode、
merge、访问 secrets 必须 ask/deny。

## 累计项目 increment

- `docs/capstone-architecture.md`
- `artifacts/week-37/beta-contract.md`
- `artifacts/week-37/model-input-manifest.json`
- `ai-app/autonomy-policy.md`
- `notes/ai-app/week-37.md`

## 本周必须产出

- 冻结 beta scope/architecture/acceptance 与 clean run transcript。
- normal/boundary/odd-shape correctness artifacts。
- autonomy matrix、8 个 policy cases 与 GH-600 evidence。

## Day 1 — 理论与 scope freeze（105 分钟）

- **15 分钟**：阅读 T1，核对 engine/runtime compatibility 边界。
- **30 分钟**：分别验证 M2、M4/G4 与 M6 hashes；不得把 M2 当作 Resize oracle，
  任何不一致先标 blocker。
- **50 分钟**：完成 `docs/capstone-architecture.md` 的 problem、users、pipeline、
  tensor contracts、acceptance、core/non-goals。
- **10 分钟**：写 M2 source/model/input/dataset/normalize-layout、M4/G4 resize/oracle、
  M6 engine/direct-device identity。

**必须留下**：`beta-contract.md`、architecture v1 和 frozen manifest。

## Day 2 — Guided lab：clean build/deploy/run（110 分钟）

- **15 分钟**：阅读 T2，确认文档 release 与设备 L4T 相同。
- **75 分钟**：从 clean build directory 执行 configure→build→model/engine prepare→
  deploy→run；移除 hard-coded personal paths，保存 expected outputs 与 first blocker。
- **10 分钟**：标出 host 与 Orin 的职责边界。
- **10 分钟**：更新 reproduction checklist。

**必须留下**：clean transcript、environment manifest 和 hidden-step list。

## Day 3 — Independent lab：beta correctness set（105 分钟）

- **15 分钟**：冻结 normal、boundary、odd/dynamic 三类 inputs。
- **70 分钟**：运行 end-to-end，比较 intermediate preprocessing 与 final output；
  校验 frame/order/shape metadata，并重复以判断 deterministic/bounded variation。
- **20 分钟**：生成 artifact index 与 checksum。

**必须留下**：三类 results、max/task error 和 reproducible artifact index。

## Day 4 — Integration：Autonomy policy（110 分钟）

- **20 分钟**：阅读 T3+T4；从 T3 提取 autonomy/guardrail enforcement 要求，
  用 T4 只定位 blueprint/domain。
- **50 分钟**：写 `ai-app/autonomy-policy.md`：read-only、reversible、external、
  irreversible/compliance 四级；为 benchmark、power mode、PR、merge、secret 指定
  allow/audit/ask/deny 与 enforcement owner。
- **30 分钟**：运行 8 个 policy fixtures，含一条伪装成 log 的恶意 instruction。
- **10 分钟**：记录 prompt rule 与 host enforcement 的差异。

**必须留下**：autonomy policy、8 case verdicts 和 permission-boundary diagram。

## Day 5 — Assessment：Beta review × guardrails（105 分钟）

- **60 分钟**：由 acceptance checklist 审查 beta；随机抽一例从 input hash 追到 final
  output，不满足时只修 correctness/reproduction blocker。
- **30 分钟**：写 3–5 个 autonomy scenarios，解释过度自治与无意义审批为何都错误；
  链到 `docs/gh600-checklist.md`。
- **15 分钟**：做 secret/path/license/non-goal 检查。

**必须留下**：beta review、3–5 题解析和 GH evidence link。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean rerun 一个代表 case，确认不依赖 profiler 或交互历史。
- **20 分钟**：口述 frozen contract、regression budget 与两个 non-goals。
- **25 分钟**：写 “Mechanism→Evidence→Limitation→Next action”。
- **15 分钟**：冻结 W38 precision/shape validation matrix。

**必须留下**：beta v0.1 evidence index、周总结和 W38 matrix。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 能从 manifest 证明没有偷换 model/input/preprocess semantics。
- 三类 inputs 均有 intermediate 与 final oracle evidence。
- autonomy decision 指向真实 enforcement boundary。
- **Unlock W38**：beta correctness 通过且 precision/shape matrix 已冻结。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [GitHub Blog · AI & ML](https://github.blog/ai-and-ml/)
  只选 1 篇明确讨论 agent permissions、human review 或 autonomy boundary 的文章；
  必须记录 GA/Preview/观点文章的 maturity，不能把产品能力当 enforcement guarantee。
- **Artifact**：写 `notes/feed/week-37.md`，固定四栏
  `claim → evidence → scope → decision`，decision 映射到现有 autonomy policy。
- **取消规则**：beta contract 或三类 correctness evidence 落后时直接取消。
