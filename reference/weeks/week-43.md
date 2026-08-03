# Unit 8 · Week 43 — Controlled Optimization 与 GitHub Evidence Retrieval

> **课程角色**：Unit 8 入口。只从 M7 frozen beta 的最高贡献 bottleneck 选择一个
> optimization，预注册 hypothesis 与 falsification，再做公平 before/after。
> Sidecar 用 read-only GitHub MCP/standard artifacts 找证据，不让 agent 决定结果。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：M7 pass、frozen source/model/input/environment/baseline、ranked bottlenecks。
- TensorRT optimization 资料只读 actual-version archive；JetPack/power/thermal 变化先
  重建 baseline。禁止从 `latest` 文档复制不支持的 flags。
- **Remediation**：baseline 漂移超阈值时 Day 1–2 只修 measurement；没有 profiler
  evidence 时选“补证据”而非凭直觉实现；correctness regression 时停止 timing。

## Observable objectives（可观察目标）

完成后能够：

1. 写单一 optimization hypothesis、success/falsification 与 rollback。
2. 用最小 profiler evidence 定位具体 copy/kernel/reformat/allocation/host gap。
3. 在相同 contract/operating point 下采公平 before/after。
4. 通过 GitHub MCP/CI artifacts 检索证据，同时保留 human decision ownership。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Controlled variable | before/after 唯一有意改变的因素 |
| Falsification | 预先定义何种观察会否定 hypothesis |
| Local speedup | 某 stage/kernel 的变化，不代表 end-to-end gain |
| Mechanism evidence | 证明预期 copy/kernel/gap 实际发生变化的 trace/metric |
| Rollback | 恢复 frozen beta 的可验证路径 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin guide 的 “Best Practices / Performance” 对应章节 | 20 | 1 |
| T2 | [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/) — CUDA trace、NVTX、statistics/export | 15 | 2 |
| T3 | [Using the GitHub MCP Server from Copilot Chat](https://docs.github.com/en/copilot/how-tos/copilot-on-github/copilot-for-github-tasks/using-the-github-mcp-server-from-copilot-chat) — read-only file/PR/workflow evidence | 15 | 4 |

## 带问题阅读

- top bottleneck 的“可优化”与“占时最多”为何不是同义词？
- kernel 快 20% 时 end-to-end 至少会快多少？哪些瓶颈会阻断？
- GitHub artifact 的存在为何不等于其内容可信或 comparison valid？

## Worked example

```text
hypothesis: remove one D2H/H2D pair
invariant: identical input/final output within frozen tolerance
expected: copy count -2; stage time down; end-to-end p95 non-regression
falsify: copy remains, or final correctness/p95 regresses
rollback: beta path feature switch / known commit
```

## 累计项目 increment

- `artifacts/week-43/optimization-contract.md`
- `artifacts/week-43/focused-profile.md`
- `artifacts/week-43/before-after.csv`
- `docs/case-studies/runtime-integration.md`
- `notes/ai-app/week-43.md`

## 本周必须产出

- 单变量 experiment contract、focused evidence 与 reversible implementation。
- full correctness、raw before/after、mechanism evidence 与 keep/revert decision。
- GitHub MCP evidence trace、standard artifact map 与 3–5 个 GH scenarios。

## Day 1 — 理论与 experiment contract（105 分钟）

- **20 分钟**：阅读 T1 的实际版本章节。
- **25 分钟**：复制 M7 baseline identity，写 hypothesis、metric、falsification、
  invariants、sample protocol、rollback。
- **50 分钟**：做 baseline confirmation；漂移超阈值则调查。
- **10 分钟**：冻结 `optimization-contract.md`。

**必须留下**：experiment contract、confirmed baseline 和 drift decision。

## Day 2 — Guided lab：Focused profiler evidence（110 分钟）

- **15 分钟**：阅读 T2。
- **70 分钟**：只捕获相关 stages/frames，定位具体 copy/kernel/reformat/allocation/
  launch/host gap；保存 command、trace range、metrics。
- **15 分钟**：写 change spec 与“不修改的 semantics”。
- **10 分钟**：由另一遍人工检查 hypothesis 是否仍可被推翻。

**必须留下**：`focused-profile.md`、trace links 和 change spec。

## Day 3 — Independent lab：实现一个变化（105 分钟）

- **15 分钟**：列 files/symbols/rollback switch。
- **60 分钟**：实现唯一变化，保留 beta variant。
- **20 分钟**：运行 unit/operator/end-to-end correctness 与 failure path。
- **10 分钟**：出现 regression 时记录并停止 timing。

**必须留下**：working change、full correctness 和 verified rollback。

## Day 4 — Integration：Before/after × GitHub evidence（110 分钟）

- **15 分钟**：阅读 T3。
- **65 分钟**：在相同 model/input/build/power/thermal window 交替采 before/after；
  计算 stage/end-to-end median/p95/throughput/memory，再 profile optimized case。
- **20 分钟**：用 read-only GitHub MCP 或 local fixture 取 source/PR/workflow artifact
  metadata，验证 hashes；不让 agent merge或选择 winner。
- **10 分钟**：保存 comparison validity check。

**必须留下**：`before-after.csv`、mechanism trace 和 GitHub evidence trace。

## Day 5 — Assessment：Keep/revert defense（105 分钟）

- **60 分钟**：盲审 before/after：correctness、effect size、variance、thermal、local vs
  end-to-end；按预注册条件 keep/revert/needs-data。
- **30 分钟**：写 3–5 个 scenarios，覆盖 cherry-picked samples、changed workload、
  agent unsupported claim、missing artifact、rollback。
- **15 分钟**：更新 GH checklist 与 case-study evidence links。

**必须留下**：decision record、3–5 题解析和 evidence links。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean rerun selected/reverted candidate 与 correctness subset。
- **20 分钟**：口述 hypothesis→evidence→mechanism→decision。
- **25 分钟**：更新 `runtime-integration.md`，包含无收益/反直觉结果。
- **15 分钟**：准备 W44 sustained validation 与 CI artifacts。

**必须留下**：Week 43 evidence index、case-study increment 和 W44 validation plan。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 唯一 changed variable 可从 manifest/diff 证明。
- local 与 end-to-end effect 分开，thermal/variance 不被隐藏。
- GitHub MCP 只检索证据，不拥有 merge或最终性能判断。
- **Unlock W44**：candidate 有 keep/revert decision 和可复现 rollback。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [NVIDIA Technical Blog · CUDA](https://developer.nvidia.com/blog/tag/cuda/)
  只选 1 篇包含 profiler evidence 与可控优化变量的文章；一次改变多个变量或无 baseline
  的案例不选。
- **Artifact**：写 `notes/feed/week-43.md`，固定四栏
  `claim → evidence → scope → decision`，decision 只能进入 test backlog，不能直接 keep。
- **取消规则**：controlled experiment 或 rollback evidence 落后时直接取消。
