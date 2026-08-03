# Week 14 — Grid、Block、Warp、SIMT 与 JSON-RPC Messages

> **Unit 3 · GPU Execution and Memory Foundations（第 2/6 周）**  
> **课程角色**：建立 thread hierarchy/SIMT 心智模型；sidecar 在 MCP 架构内掌握 JSON-RPC request/response/notification correlation。

## 开始前：硬先修与 remediation

- **硬先修**：Week 13 vector add contract/CPU oracle/error checks 已建立。
- **先修检查**：对 `N=1000, block=128` 算 grid size；若不是 8，先复习 ceiling division。
- **失败时 remediation**：用 host simulator 打印 blockIdx/threadIdx/global index；无 GPU 时仍可完成 mapping/JSON fixtures，性能任务保持 blocked。

## Observable objectives

- 从 N 与 block size 推导 grid、global index、partial block/warp。
- 解释 warp、SIMT、divergence 与 bounds check。
- 实现 ordinary/global 与 grid-stride indexing，覆盖边界尺寸。
- 用等工作量 variants 比较 uniform/warp-coherent/lane-alternating branches。
- 构造并校验 JSON-RPC request/response/notification 与 ID correlation。

## 术语表

需能解释：**grid、block、thread、threadIdx、blockIdx、blockDim、warp、lane、SIMT、divergence、grid-stride loop、partial warp、JSON-RPC、request、response、notification、method、params、id**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[CUDA Programming Guide 2.3.2 — Thread Hierarchy](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/writing-cuda-kernels.html#thread-hierarchy)。
- **T2（Day 2，20 分钟）**：[CUDA Programming Guide 2.3.1 — Basics of SIMT](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/writing-cuda-kernels.html#basics-of-simt)。
- **S1（Day 1，20 分钟）**：[JSON-RPC 2.0 Specification](https://www.jsonrpc.org/specification) — request/response/notification/error。
- **S2（Day 3，20 分钟）**：[MCP Base Protocol](https://modelcontextprotocol.io/specification/2025-11-25/basic) — JSON-RPC message requirements。

80 分钟计入 Day 1–3；记录 MCP revision。Podcast/blog 仅为 optional seminar，不作教材。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **先修或 Gate 落后时取消**：阅读 [NVIDIA Technical Blog: Using CUDA Warp-Level Primitives](https://developer.nvidia.com/blog/using-cuda-warp-level-primitives/)；只提取 warp/lane/divergence 或 active-mask 相关 claim，不提前实现 warp primitive。
- 在 `notes/feed/week-14.md` 写 `claim → source/evidence → scope → experiment/decision`，说明该 claim 对当前 GPU/compute capability 是否成立、能否由本周 branch fixture 验证。不计 unlock。

## 带问题阅读

- warp 是编程层级、调度单位还是二者皆有条件？
- divergence 何时序列化不同 paths，何时只是 predication/短分支？
- bounds check 为什么通常只影响边界少量 lanes，却仍必须存在？
- notification 为什么没有 response，ID mismatch 会破坏什么？

## Worked example / 手算

`N=1000, block=128`：grid=`8`，每 block 4 warps；最后 block 负责 indices 896–1023，仅 104 active threads，即 3 full warps + 最后 warp 8 active lanes。总发射 1024 threads；`i<N` 必须保护 24 lanes。

## 累计项目 increment

- `labs/cuda/week14_indexing_simt.cu`
- `notes/concepts/grid-block-warp-simt.md`
- `ai-app/client-tests/jsonrpc/week-14-messages.jsonl`
- `notes/ai-app/week-14-jsonrpc.md`

## Exit ticket / unlocks

**形成性**：5 个 boundary sizes 全量正确、能画 partial warp/branch paths、JSON-RPC valid/invalid messages 被正确分类。完成 unlock Week 15。

## Day 1

**阶段：理论 + mapping（100 分钟）**

- **20 分钟**：读 T1。
- **25 分钟**：逐步做顶部 mapping，再做 `N=257, block=64`。
- **35 分钟**：guided host/kernel 打印并验证 global index coverage。
- **20 分钟**：读 S1，手写 request/response/notification 各一条。
- **必须留下**：两组 mapping、coverage output 和三种 messages。

## Day 2

**阶段：worked SIMT（100 分钟）**

- **20 分钟**：读 T2。
- **25 分钟**：画 uniform、warp-coherent、alternating lane paths。
- **40 分钟**：实现 grid-stride SAXPY+CPU oracle，测试 `0,1,31,32,33,255,256,257,large`。
- **15 分钟**：检查 JSON-RPC IDs 与 notification no-response。
- **必须留下**：lane diagrams、case matrix 和 ID correlation table。

## Day 3

**阶段：guided protocol + runtime lab（100 分钟）**

- **20 分钟**：读 S2。
- **60 分钟**：完成三种 branch variants，保持同 input/output/total work；验证 compiler 未消除核心分支。
- **10 分钟**：增加 malformed JSON 与 mismatched ID fixtures。
- **10 分钟**：保存 build/protocol revision。
- **必须留下**：branch sources/correctness 与 positive/negative JSONL。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立 block-size sweep `64/128/256/512`；同 correctness/workload/build，保存 raw latency但不从一次结果下结论。
- **20 分钟**：写最小 JSON checker 或明确 assertions，指出首个 invalid message。
- **10 分钟**：记录 launch config。
- **必须留下**：block-size raw data、checker 与 first-invalid output。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：完成 SIMT concept note；关联 mapping、correctness、branch evidence，区分 occupancy（尚未系统学习）。
- **20 分钟**：写 5 个 JSON-RPC scenarios，解释错误选项。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：concept note、5 个 protocol cases 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：clean-run boundary/branch cases。
- **25 分钟**：闭卷重画 mapping 与 divergence。
- **15 分钟**：闭卷写三种 JSON-RPC messages。
- **15 分钟**：填写 `notes/weekly/week-14.md` 与 Week 15 address hypothesis。
- **必须留下**：形成性 Exit ticket、clean evidence、protocol recall 和 unlock 状态。
