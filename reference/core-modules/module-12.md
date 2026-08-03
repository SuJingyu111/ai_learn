# Core Module 12 — Resize Optimization Case Study

## 本周目标

完成第一份作品级优化报告。保留 simple correct baseline，每次只改变一个变量，
并由 Nsight/benchmark 证据决定是否保留优化。

## 候选实验

- `8×8`、`16×16`、`32×8`、`32×16` block shapes。
- channel specialization 或 RGBA vectorized access。
- coordinate calculation hoisting/precomputation。
- pitched/padded memory。
- texture object（只有语义完全匹配时）。

不要求全部实现。先从 baseline profile 选择最可能影响当前瓶颈的 1–2 项。

## 每日计划

### Day 1 — 冻结 Baseline

- **10 分钟**：选择正式 case matrix，不在看到结果后删难看的 case。
- **20 分钟**：记录 commit/source identity、Orin environment、build flags、
  power/thermal procedure。
- **50 分钟**：运行 full correctness，然后采集 naive kernel raw baseline：
  warm-up、至少 30 samples、kernel/end-to-end、1/3/4 channels。
- **10–30 分钟**：校验 CSV、median/p95/checksum/max error。
- **必须留下**：immutable baseline manifest 和 raw data。

### Day 2 — Profile 与可推翻 Hypothesis

- **10 分钟**：先写 1–2 个 hypothesis，例如 block geometry 或 memory access。
- **15 分钟**：为每个 hypothesis 选 Systems/Compute evidence。
- **55 分钟**：profile 代表性 resize case，检查 launch、duration、memory、
  occupancy/registers 和 source access 相关现象。
- **10–30 分钟**：决定只实现最有证据的优化 A。
- **必须留下**：hypothesis/evidence/change plan；禁止同时修改两个变量。

### Day 3 — 实现 Optimization A

- **10 分钟**：写 A 不允许改变的 API/workload/correctness。
- **50 分钟**：实现 A，例如 block shape specialization 或 channel path；保留 baseline。
- **20 分钟**：运行完整 CPU/CUDA correctness、randomized、sanitizer subset。
- **10–30 分钟**：按 Day 1 procedure 采集 before/after raw samples。
- **必须留下**：A 的代码、correctness、raw delta 和 unexpected observations。

### Day 4 — Evidence Review 与 Optimization B 决策

- **15 分钟**：检查 A 的统计稳定性、thermal drift 和 profiler explanation。
- **20 分钟**：明确 A 是保留、回退还是仅作为实验 variant。
- **45 分钟**：只有 A 已解释清楚才实现 B；否则重复关键测量或补 profiler。
- **10–30 分钟**：B 同样运行 correctness 和相同 benchmark。
- **必须留下**：decision log；“不做 B”可以是有效结论。

### Day 5 — Workload Matrix 与 Sustained Run

- **10 分钟**：冻结 `720p→1080p`、`1080p→720p`、`4K→1080p`、channels。
- **50 分钟**：对 baseline/retained variant 运行完整 matrix。
- **20 分钟**：选一个 workload sustained 运行，记录前后温度和 latency drift。
- **10–30 分钟**：检查优化是否只对一个 size/channel 有效。
- **必须留下**：matrix、thermal note、kernel/end-to-end 对照。

### Day 6 — Case Study 与 Week 12 Gate

- **15 分钟**：复制 profiling report 到 `docs/case-studies/resize.md`。
- **55 分钟**：完成 Question→Baseline→Hypothesis→Evidence→Change→Correctness
  →Result→Limitations；链接 raw data。
- **15 分钟**：逐项验证所有数字、units、environment 和复现命令。
- **15–30 分钟**：完成 Week 12 Gate 与 weekly review，录制 3 分钟口述摘要。
- **必须留下**：第一份作品级 case study，即使结果是“优化无收益”。

## 报告结构

```text
Question → Baseline → Hypothesis → Evidence → Change
         → Correctness → Result → Confounders → Conclusion
```

## Week 12 Gate

- [ ] baseline 和 optimized workload 语义相同。
- [ ] raw samples、median/p95、max error 已保存。
- [ ] kernel-only 和 end-to-end 分开。
- [ ] 至少一个结论由 profiler 支持或推翻。
- [ ] 报告明确硬件、版本、power/thermal 和适用范围。
