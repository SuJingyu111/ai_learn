# Week 15 — Memory Hierarchy、Coalescing 与 MCP Lifecycle

> **Unit 3 · GPU Execution and Memory Foundations（第 3/6 周）**  
> **课程角色**：把 warp mapping 转成 memory-access evidence；sidecar 让 JSON-RPC messages 进入 MCP initialize→operate→close 的有序 lifecycle。

## 开始前：硬先修与 remediation

- **硬先修**：能画 warp lanes/global indices，SAXPY boundary cases 通过。
- **先修检查**：写 warp 中 lane `l` 访问 `base+l` 与 `base+2l` 的地址序列。
- **失败时 remediation**：先在纸上画 8 lanes，再扩成 32；没有 Nsight/GPU 时用 address simulator 与理论问题，不伪造 transaction/profile。

## Observable objectives

- 区分 registers、local/shared/global/constant memory 与 cache 的作用域/生命周期。
- 从 warp address pattern 解释 contiguous、strided、misaligned access。
- 计算 useful bytes/effective bandwidth，并保留 assumptions。
- 实现 aligned/offset/strided copy，先 correctness 后 timing/profile。
- 画 MCP lifecycle state machine并拒绝 out-of-order messages。

## 术语表

需能解释：**memory hierarchy、register、local/shared/global/constant memory、cache、coalescing、alignment、transaction、useful bytes、effective bandwidth、initialize、protocol version、capability exchange、initialized notification、shutdown/close**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[CUDA Programming Guide 1.2.3 — GPU Memory](https://docs.nvidia.com/cuda/cuda-programming-guide/01-introduction/programming-model.html#gpu-memory)。
- **T2（Day 2，25 分钟）**：[CUDA Best Practices — Coalesced Access to Global Memory](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#coalesced-access-to-global-memory)。
- **T3（Day 3，15 分钟）**：[CUDA Best Practices — Effective Bandwidth Calculation](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#effective-bandwidth-calculation)。
- **S1（Day 1，20 分钟）**：[MCP Lifecycle](https://modelcontextprotocol.io/specification/2025-11-25/basic/lifecycle) — initialization、operation、shutdown。

80 分钟计入 Day 1–3；Podcast/blog 仅 optional。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **先修或 Gate 落后时取消**：阅读 [NVIDIA Technical Blog: How to Access Global Memory Efficiently in CUDA C/C++ Kernels](https://developer.nvidia.com/blog/how-access-global-memory-efficiently-cuda-c-kernels/)；选择 aligned/offset/stride 中一类 claim，并先标记文章硬件年代。
- 在 `notes/feed/week-15.md` 写 `claim → source/evidence → scope → experiment/decision`；旧硬件数据只能生成当前设备的 hypothesis，不能直接成为结论。不计 unlock。

## 带问题阅读

- logical global memory access 与 physical transaction/cache behavior 的证据边界在哪里？
- stride 2 相比 contiguous 增加了哪些无效/分散访问？
- effective bandwidth 的 bytes 必须包含 read 与 write 吗？
- initialization 前为什么不能调用 tool，version negotiation 失败如何退出？

## Worked example / 手算

假设 warp 32 lanes 读取 FP32：contiguous 地址为 `base+4l`，useful bytes=`128`；stride 2 为 `base+8l`，仍只消费 128 useful bytes，却覆盖 252-byte address span。实际 transactions 取决于 GPU/cache/alignment，必须以当前硬件文档/profiler验证，不能只凭 span 声称倍数。

## 累计项目 increment

- `labs/cuda/week15_memory_access.cu`
- `artifacts/results/week15-memory.csv`
- `notes/concepts/memory-hierarchy-coalescing.md`
- `ai-app/client-tests/lifecycle/week-15-traces.jsonl`

## Exit ticket / unlocks

**形成性**：四种 address patterns 手算、copy correctness/raw data 完整、effective-bandwidth 公式可重算、lifecycle normal + 3 negative traces 通过。完成 unlock Week 16。

## Day 1

**阶段：理论 + address model（100 分钟）**

- **20 分钟**：读 T1。
- **25 分钟**：逐步做顶部 worked example，再画 stride 32 与 misaligned。
- **35 分钟**：写 host simulator 输出每 lane address/span/useful bytes。
- **20 分钟**：读 S1，画 initialize→initialized→operation→close state machine。
- **必须留下**：四张 address 图、simulator 和 lifecycle v1。

## Day 2

**阶段：worked coalescing（100 分钟）**

- **25 分钟**：读 T2。
- **25 分钟**：为 aligned/offset/stride2/stride32 写 hypothesis 与可推翻条件。
- **40 分钟**：guided 实现四种 copy；先 full-output/canary correctness。
- **10 分钟**：写一条正常 MCP message trace。
- **必须留下**：hypothesis table、copy source/correctness 和 normal trace。

## Day 3

**阶段：guided measurement/lifecycle（100 分钟）**

- **15 分钟**：读 T3。
- **60 分钟**：冻结 N/block/build/power/warm-up，采 CUDA-event raw samples，算 median/p95/effective bandwidth；无设备保留 blocked。
- **15 分钟**：增加 initialization 前 call、wrong response ID、unsupported version traces。
- **10 分钟**：记录 CUDA/MCP versions。
- **必须留下**：raw CSV/formula 和三条 negative traces。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立实现 naive transpose 或等价 2D access；CPU oracle，覆盖 `1×1,31×33,32×32,1000×777`，报告 first mismatch。
- **20 分钟**：实现/纸面检查 lifecycle validator，out-of-order 必须 fail closed。
- **10 分钟**：保存 launch/address contract。
- **必须留下**：2D case matrix、first mismatch logic 与 lifecycle validator。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：完成 memory concept note，明确 logical addresses、measured duration、transaction inference 的层次。
- **20 分钟**：写 5 个 lifecycle scenarios，解释错误选项。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：concept note、5 个 lifecycle cases 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：clean-run copy/transpose或复核 blocked evidence。
- **25 分钟**：闭卷重做 address/bandwidth 题。
- **15 分钟**：闭卷重画 lifecycle。
- **15 分钟**：填写 `notes/weekly/week-15.md` 与 Week 16 shared-memory question。
- **必须留下**：形成性 Exit ticket、clean/blocked evidence 和 unlock 状态。
