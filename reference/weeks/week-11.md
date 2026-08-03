# Week 11 — Correctness、Tolerance、Benchmark Statistics 与 Tool Permissions

> **Unit 2 · Model-to-Runtime CPU Vertical Slice（第 5/6 周）**  
> **课程角色**：把“能运行”升级为可比较、可复现的 runtime evidence；sidecar 为 tool 建立 least-privilege capability/blast-radius matrix。

## 开始前：硬先修与 remediation

- **硬先修**：Week 10 ORT/PyTorch correctness 通过并有 raw latency samples。
- **先修检查**：能说明 validation、warm-up、steady-state 的顺序与 timing inclusion。
- **失败时 remediation**：若没有 raw samples，先重跑固定 input/iterations；若 tolerance 无依据，退回 exact hand case 与 FP32 scale analysis。

## Observable objectives

- 选择并解释 abs/rel tolerance，输出 max error 与 worst coordinate。
- 冻结 workload/environment/warm-up/iterations/timing method。
- 从 raw samples 计算 median、p95、min，不挑最佳结果。
- 区分 correctness、latency、throughput 与统计稳定性。
- 将统计结论限制在 frozen runtime input tensor；source→model shape 只记录 planned contract，不把 Resize 算进 timed region 或 correctness claim。
- 给每个 tool capability 写权限、blast radius、approval、audit 和 fallback。

## 术语表

需能解释：**oracle、tolerance、absolute/relative error、raw sample、median、percentile、p95、outlier、warm-up、steady state、confidence、capability、permission、blast radius、audit、approval**。

## 必读理论材料

- **T1（Day 1，15 分钟）**：[NumPy `allclose`](https://numpy.org/doc/stable/reference/generated/numpy.allclose.html) — comparison equation 与 asymmetry warning。
- **T2（Day 1，20 分钟）**：[PyTorch Numerical Accuracy](https://docs.pytorch.org/docs/stable/notes/numerical_accuracy.html) — backend/batched/extremal limitations。
- **T3（Day 2，15 分钟）**：[Python `statistics.median`](https://docs.python.org/3/library/statistics.html#statistics.median) — median definition。
- **T4（Day 2，15 分钟）**：[NumPy `percentile`](https://numpy.org/doc/stable/reference/generated/numpy.percentile.html) — method parameter 与 interpolation。
- **S1（Day 3，20 分钟）**：[GitHub Actions security hardening](https://docs.github.com/en/actions/security-for-github-actions/security-guides/security-hardening-for-github-actions) — least privilege、untrusted input、credentials。

85 分钟计入 Day 1–3；同时按需查本地 [benchmark methodology](../docs/benchmark-methodology.md)，不增加阅读时长。Podcast/blog 仅为 optional seminar，不作教材。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **先修或 Gate 落后时取消**：从 [Chip Huyen](https://huyenchip.com/) 选择一篇含 latency、throughput、evaluation 或 production measurement 的文章；必须能找到 measurement boundary 或明确指出文章没有给出。
- 在 `notes/feed/week-11.md` 写 `claim → source/evidence → scope → experiment/decision`，把一条 measurement claim 改写成可被 raw samples 推翻的 hypothesis。不计 unlock。

## 带问题阅读

- `allclose(a,b)` 为什么可能不对称？
- p95 的具体数值为何依赖 percentile method？
- median/p95 稳定是否足以证明 workload 代表生产？
- 为什么本周 model/runtime-tensor latency 不能被命名为含 Resize 的 end-to-end latency？
- read-only tool 为什么仍需要 path/data scope 与 audit？

## Worked example / 手算

Samples `[10,11,12,13,100] ms`：median=`12 ms`；若采用 nearest-rank，p95 rank=`ceil(.95×5)=5`，所以 `100 ms`。NumPy 默认方法可能插值得到不同结果，因此报告必须记录 library/version/method，不能只写“p95=…”。

## 累计项目 increment

- `labs/ml/week11_runtime_benchmark.py`
- `artifacts/results/week11_ort_samples.csv`
- `notes/experiments/week-11-runtime-baseline.md`
- `notes/ai-app/week-11-permission-matrix.md`

## Exit ticket / unlocks

**形成性**：correctness/tolerance 有 rationale，raw samples 可重算统计，timed region 清楚且仅覆盖 frozen runtime tensor scope，不声称 Resize correctness/latency；permission matrix 覆盖 read/write/network/secret/device。完成 unlock Week 12 defense。

## Day 1

**阶段：理论 + correctness model（100 分钟）**

- **15 分钟**：读 T1。
- **20 分钟**：读 T2。
- **25 分钟**：做三组 allclose 手算：large、near-zero、NaN/Inf policy。
- **25 分钟**：guided 输出 max abs/rel/worst coordinate 与 tolerance rationale。
- **15 分钟**：列 Runtime Assistant capabilities inventory。
- **必须留下**：三组手算、correctness reporter 和 capability list。

## Day 2

**阶段：worked statistics（100 分钟）**

- **15 分钟**：读 T3。
- **15 分钟**：读 T4。
- **25 分钟**：逐步完成顶部 median/p95 example，并用两种 percentile methods 对照。
- **35 分钟**：从 Week 10 raw samples 计算 median/p95/min，保存 method/version。
- **10 分钟**：给 capabilities 标 read/write/network/secret/device。
- **必须留下**：统计 worksheet、reproducible calculation 和 permission axes。

## Day 3

**阶段：guided benchmark（100 分钟）**

- **60 分钟**：按固定 workload 采新 raw samples，确认 validation/logging 在 timed region 外；比较两个 batch/shape 但不混合分布。
- **20 分钟**：读 S1。
- **10 分钟**：为每个 capability 写 default allow/deny/approval。
- **10 分钟**：记录 environment/confounders。
- **必须留下**：raw CSV、benchmark contract 和 initial permission matrix。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立重复 benchmark，比较 run-to-run delta；若 drift，检查 thermal/background/warm-up，不删除 outlier。
- **20 分钟**：补 blast radius/audit/timeout/fallback；arbitrary shell/path/secret 默认 deny。
- **10 分钟**：验证统计可由 CSV 重算。
- **必须留下**：repeat-run analysis 与完整 capability matrix。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：写 experiment report：question→frozen runtime-input scope→method→raw→stats→correctness→limitations；planned source shape 单列，Resize 明确 deferred 到 W21/M4。
- **20 分钟**：做 5 个 permission scenarios，解释错误选项。
- **20 分钟**：完成 Exit ticket 初测并列 Week 12 blockers。
- **必须留下**：experiment report、5 个 scenarios 和 blocker list。

## Day 6

**阶段：review / defense unlock（100 分钟）**

- **45 分钟**：修复 blockers，clean-run correctness+benchmark。
- **25 分钟**：闭卷重算一个 sample set 并解释 p95 method。
- **15 分钟**：闭卷解释 permission matrix 不是 enforcement。
- **15 分钟**：填写 `notes/weekly/week-11.md` 与 M2 defense order。
- **必须留下**：形成性 Exit ticket、clean evidence、defense agenda 和 unlock 状态。
