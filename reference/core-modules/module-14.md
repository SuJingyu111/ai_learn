# Core Module 14 — CUDA Affine Warp 与不规则访问

## 本周目标

实现 CUDA affine warp，并分析 inverse mapping 带来的 source locality、border
divergence 和 matrix delivery 选择。

## 每日计划

### Day 1 — CUDA Affine Nearest Baseline

- **10 分钟**：手算 grid/block 和 output index，复查 Week 13 contract。
- **20 分钟**：设计 async API、device pointers、stream 和 error return。
- **50 分钟**：实现一线程一输出 pixel 的 nearest baseline，先跑 identity/translation。
- **10–30 分钟**：与 CPU逐元素比较，并运行 non-block-multiple size。
- **必须留下**：baseline kernel/API、max error 和 launch/lifetime contract。

### Day 2 — CUDA Bilinear 与 Shared Cases

- **10 分钟**：比较 CPU/CUDA floor/clamp/fraction 计算。
- **15 分钟**：将 Week 13 table-driven cases 接入 GPU harness。
- **55 分钟**：实现 bilinear；运行 identity、subpixel、scale、rotation。
- **10–30 分钟**：检查 NaN/Inf、padding sentinel 和 first mismatch coordinates。
- **必须留下**：bilinear GPU、shared case matrix 和 correctness。

### Day 3 — Border/Access Workloads

- **10 分钟**：定义 border-heavy 和 interior-heavy，保证 output size/work 可比。
- **20 分钟**：预测 branch/source locality 差异。
- **50 分钟**：生成代表 inputs/transforms，采集 correctness 和 baseline latency。
- **10–30 分钟**：确认不是因为大量 constant-border output 减少了有效计算。
- **必须留下**：workload definitions、raw timing 和预测。

### Day 4 — Matrix Delivery 对照

- **10 分钟**：预测 kernel parameter 与 constant memory 的收益条件。
- **20 分钟**：阅读 constant memory broadcast 与 kernel parameter 相关资料。
- **50 分钟**：实现一个替代 delivery path；同一 transform/workload/correctness 对照。
- **10–30 分钟**：采集 raw samples，不在看到结果后改变 case。
- **必须留下**：两版代码、correctness、latency 和调用复杂度比较。

### Day 5 — Nsight Evidence

- **10 分钟**：列 source access、cache、branch、duration 四个问题。
- **15 分钟**：选择 Systems/Compute metrics/sections。
- **55 分钟**：profile border/interior 和 matrix delivery；标出最能解释差异的证据。
- **10–30 分钟**：说明无法由当前 metrics 证明的内容。
- **必须留下**：profiler commands、compact table 和 hypothesis result。

### Day 6 — Warp 总结

- **15 分钟**：运行 full CPU/CUDA randomized suite 和 sanitizer subset。
- **40 分钟**：写 warp correctness/performance summary，更新 known limitations。
- **20 分钟**：决定是否保留 constant path；不为展示而增加 perspective。
- **15–30 分钟**：weekly review，为 Week 15 画单 frame async pipeline 初稿。
- **必须留下**：CUDA affine Gate 和设计决策。

## Hypothesis 示例

- 小矩阵广播可能受益于 constant memory，但 kernel parameters 可能已经足够。
- border-heavy input 可能增加分支和不规则访问，但影响需由数据确认。
- affine warp 的 source access 比 resize 更难 coalesce，cache 行为可能更重要。

## 交付

- CUDA affine implementation/tests。
- border-heavy vs interior-heavy raw data。
- matrix delivery 对照结论。

## 完成标准

- [ ] CPU/CUDA randomized comparison 通过。
- [ ] transform、sampling、border 三类问题可独立定位。
- [ ] performance 结论不从 resize 直接外推。
- [ ] perspective extension 不阻塞主线。
