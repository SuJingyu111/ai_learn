# Unit 3 Syllabus — CUDA Programming Foundations

**Weeks 13–18 · M3 · Gate G3**

## Entry prerequisite

G1 tensor memory PASS + G2 oracle/benchmark PASS；CUDA 环境或 Orin 可用并完成版本记录。

## Observable outcomes

1. 解释 host/device compilation、kernel launch 和异步 execution；
2. 将 1D/2D problem 映射到 grid/block/thread，并覆盖 odd/non-multiple sizes；
3. 从 warp address pattern 推理 coalescing，不把 SIMT 当作 CPU threads；
4. 正确使用 global/shared/register memory 和 block-level synchronization；
5. 分开 API、launch、execution error，并用 CUDA events 定义 timed region；
6. 使用 memcheck/racecheck/initcheck/synccheck 验证 CUDA transform。

## Concept inventory

`host/device`、`kernel launch`、`grid`、`block`、`thread`、`SM`、`warp`、`SIMT`、
`divergence`、`global/shared/register/local memory`、`coalescing`、`bank conflict`、
`barrier`、`asynchronous error`、`CUDA event`、`non-associativity`、`sanitizer`。

## Primary teaching spine

- *Programming Massively Parallel Processors*, 4th ed., Chapters 2–5：data parallelism、
  multidimensional grids、GPU architecture、memory locality。
- [CUDA Programming Guide: Programming Model](https://docs.nvidia.com/cuda/cuda-programming-guide/01-introduction/programming-model.html)
- [CUDA Programming Guide: Introduction to CUDA C++](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/intro-to-cuda-cpp.html)
- [CUDA Programming Guide: Asynchronous Execution](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/asynchronous-execution.html)
- [CUDA Best Practices Guide](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html)
- [Compute Sanitizer](https://docs.nvidia.com/cuda/compute-sanitizer/index.html)

## Six-week lesson sequence

| Week | Theory | Worked problem | Lab | Project increment |
|---:|---|---|---|---|
| 13 | architecture/toolchain/launch | host→device execution timeline | vector kernel + build | CUDA harness |
| 14 | grid/block/warp/SIMT | 2D index + boundary threads | 1D/2D mapping variants | transform skeleton |
| 15 | memory hierarchy/coalescing | warp address sequences | aligned/strided copy | HWC→CHW access design |
| 16 | sync/shared memory | barrier/bank-conflict cases | tiled transform | CUDA transform |
| 17 | error/timing/sanitizer | launch vs async error trace | injected failures + tools | hardened M3 |
| 18 | cumulative CUDA practical | unseen 2D spec | independent implementation | M3 / G3 |

## Required problem set

- 为 7 个 `N/block` 组合手算 grid、inactive lanes 和 boundary conditions。
- 画 contiguous、stride-2、stride-32、misaligned warp addresses。
- 判断 6 个 barrier placements 是否安全；解释 divergence around barrier。
- 区分 host enqueue time、event kernel time、synchronized end-to-end time。
- 解释 floating-point reduction 为什么可能稳定但不 bitwise identical。

## Guided lab ladder

vector add → 2D index → copy access variants → shared-memory tile → CUDA normalize/
HWC→CHW → controlled mem/race/init/sync failures → clean sanitizer suite。

每个 CUDA call、launch 与 execution path 都必须可观察错误；`cudaDeviceSynchronize()` 只
用于明确的 correctness/timing boundary，不得靠全局同步掩盖依赖。

## Sidecar prerequisite chain

本单元才正式进入 MCP：architecture → JSON-RPC request/response/notification →
initialize/initialized lifecycle → protocol version/capability negotiation →
tools/resources/prompts → stdio skeleton。

主材料使用 [MCP Architecture](https://modelcontextprotocol.io/docs/learn/architecture) 和
[stable 2025-11-25 specification](https://modelcontextprotocol.io/specification/2025-11-25)。

## G3

- closed-book CUDA execution/memory/error exam；
- 从未见 spec 独立写或修一个 2D kernel；
- M3 odd/non-block-multiple correctness + full sanitizer；
- oral：画一个 warp address pattern，解释 launch error 与 async error。

OOB、race、unobserved error 或只在整齐 shape 上正确均阻塞 U4。
