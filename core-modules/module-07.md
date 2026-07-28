# Core Module 07 — Global Memory、Coalescing 与 Data Layout

## 本周目标

从 warp 地址模式理解 global memory transaction，并把 AoS/SoA、HWC/CHW 的
layout 选择与具体 operator access pattern 关联。

## 每日计划

### Day 1 — Warp Address Pattern

- **10 分钟**：写 coalescing 的当前定义。
- **20 分钟**：阅读 global memory transaction/coalescing 的相关段落。
- **50 分钟**：对一个 warp 手算 contiguous、stride 2/32、misaligned 访问地址，
  画出请求的 bytes/cache lines or sectors（按实际文档术语）。
- **10–30 分钟**：写一个只打印/验证 index 的小 kernel 或 host simulator。
- **必须留下**：四种 address 图和哪些硬件细节需要 profiler 验证。

### Day 2 — Copy 与 Effective Bandwidth

- **10 分钟**：列出 copy kernel 的 useful read/write bytes。
- **15 分钟**：冻结 N、iterations、block size 和 power state。
- **55 分钟**：实现 aligned copy、offset copy、strided copy；验证 output 和 guard。
- **10–30 分钟**：用 CUDA events 测量并计算 effective bandwidth。
- **必须留下**：公式、raw samples、median/p95 和 correctness。

### Day 3 — Transpose Baselines

- **10 分钟**：画 naive transpose 的 read/write warp addresses。
- **20 分钟**：阅读 tiled transpose/shared memory 示例的机制，不直接复制完整代码。
- **50 分钟**：先实现 CPU oracle 和 naive CUDA transpose；测试 `1×1`、`31×33`、
  `32×32`、`1000×777`。
- **10–30 分钟**：加入 shared-memory/coalesced version，保持语义相同。
- **必须留下**：两版 kernel、case matrix 和 first mismatch reporting。

### Day 4 — Memory Profiler Evidence

- **10 分钟**：预测 naive/coalesced 的读写效率和 duration。
- **20 分钟**：选择当前 Nsight Compute 中对应的 memory throughput/transaction 指标。
- **50 分钟**：profile 两版与两个尺寸；保存 exact commands 和 metric values。
- **10–30 分钟**：将 profiler evidence 与 effective bandwidth 对照。
- **必须留下**：预测/证据/结论表，不把一个 size 的结论泛化。

### Day 5 — HWC/CHW Access Experiment

- **10 分钟**：选定具体 workload，例如“每线程处理一个 pixel 的所有 channels”
  或“每线程处理一个 channel plane”，不能只比较抽象 layout。
- **20 分钟**：画 HWC/CHW warp addresses 和 conversion costs。
- **50 分钟**：实现一个 RGB/RGBA operation 的两种 layout，或实现 conversion +
  downstream access；验证逻辑 output。
- **10–30 分钟**：测量 conversion 是否被包含，分别报告。
- **必须留下**：workload contract、layout data 和受限结论。

### Day 6 — Memory/Layout 周复盘

- **15 分钟**：闭卷画 transpose 和 HWC/CHW address pattern。
- **40 分钟**：写 `notes/concepts/coalescing-and-layout.md`。
- **20 分钟**：复核所有 bandwidth bytes 和单位。
- **15–30 分钟**：weekly review；选择 Week 8 reduction oracle 的数值范围。
- **必须留下**：raw CSV、地址图、公式和适用条件。

## 计算要求

明确每个 kernel 理论读/写字节数：

```text
effective bandwidth = total useful bytes / elapsed time
```

不要把 cache 命中、ECC、write allocation 或测量误差隐藏在“理论带宽”里。

## 交付

- transpose 至少两版。
- 一个 layout conversion/reference。
- `notes/concepts/coalescing-and-layout.md`
- raw bandwidth 数据。

## 完成标准

- [ ] transpose 支持 odd/non-tile dimensions。
- [ ] effective bandwidth 公式与字节数正确。
- [ ] layout 结论绑定一个具体访问模式。
- [ ] profiler evidence 与 wall/event timing 均可追溯。
