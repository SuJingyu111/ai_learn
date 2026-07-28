# Unit 5 Syllabus — Concurrency、Ownership 与 Async Pipeline

**Weeks 25–30 · M5 · Gate G5**

## Entry prerequisite

G4 correct preprocessor PASS，且 G3 的 synchronization/error handling 仍保持通过。

## Observable outcomes

1. 解释 stream、event、default-stream ordering 与 host synchronization；
2. 区分 pageable、pinned、device、managed memory 及 Orin integrated-memory 特性；
3. 画 producer/consumer、frame-slot、double-buffer 的 ownership state machine；
4. 为 H2D/preprocess/sink 建立显式 event DAG，不依赖 accidental global sync；
5. 分开 first-frame latency、steady latency 和 throughput；
6. 用 500–1000 frame stress/shutdown test 与 timeline 验证 lifetime 和 overlap。

## Concept inventory

`stream`、`event`、`default stream`、`ordering`、`dependency`、`pinned/pageable`、`DMA`、
`overlap`、`RAII`、`ownership`、`lifetime`、`frame slot`、`ring/double buffer`、
`backpressure`、`latency`、`throughput`、`shutdown`、`CUDA Graph`。

## Primary teaching spine

- [CUDA Programming Guide: Asynchronous Execution](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/asynchronous-execution.html)：
  streams、events、ordering、default stream。
- [CUDA Best Practices](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html)：
  pinned memory、async transfer and overlap、zero copy。
- [CUDA for Tegra](https://docs.nvidia.com/cuda/cuda-for-tegra-appnote/)：
  Memory Management、Memory Selection。
- [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)：
  CUDA timeline。

Orin CPU/iGPU 共享物理 SoC DRAM，不代表 pageable/pinned/device/managed 的访问、cache、
同步成本相同；所有结论必须在实际设备验证。

## Six-week lesson sequence

| Week | Theory | Worked problem | Lab | Project increment |
|---:|---|---|---|---|
| 25 | streams/events/default stream | event dependency DAG | two-stream ordering | explicit dependencies |
| 26 | pinned async transfer/Orin memory | overlap preconditions | pageable vs pinned | transfer stage |
| 27 | ownership/frame-slot state machine | legal/illegal transitions | RAII slots + failures | frame pipeline |
| 28 | double buffer/backpressure | schedule table | multi-frame pipeline | M5 alpha |
| 29 | timeline/overlap | latency vs throughput | Systems A/B | evidence-backed tuning |
| 30 | stress/shutdown/case study | failure trace | 500–1000 frames | M5 / G5 |

CUDA Graphs 仅在 profiler 证明 launch overhead、且 workload 固定后选修；不得为了功能清单
挤占 ownership 与 shutdown。

## Required problem set

- 画 3-stream event DAG，并找出 missing wait 和 unnecessary wait。
- 判断 8 个 buffer transitions 的 owner 与 earliest safe reuse。
- 计算 pipeline fill/drain、steady throughput 与 single-frame latency。
- 设计 consumer stall、mid-frame error、shutdown、slot exhaustion failure cases。
- 解释为何总耗时下降不能单独证明 copy/compute overlap。

## Sidecar prerequisite chain

Runtime Lab Assistant v0 增加 durable state、source/provenance、TTL/pruning/reset、resume
和 context-drift detection；随后建立 versioned eval dataset、failure taxonomy 和 traces。
外部 artifact store 是 memory，不把模型的隐式上下文当作可靠状态。

## G5

- closed-book stream/event/ownership exam；
- unseen missing-dependency 或 lifetime debugging practical；
- 500–1000 frames per-frame correctness、stress/shutdown、sanitizer；
- Nsight timeline 证明或否定 overlap，并分别报告 latency/throughput。

隐式全局同步、race/use-after-free、无法可靠 shutdown 或无 timeline evidence 阻塞 U6。

