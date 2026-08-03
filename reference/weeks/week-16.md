# Week 16 — Synchronization、Shared Memory 与 MCP Capabilities/Primitives

> **Unit 3 · GPU Execution and Memory Foundations（第 4/6 周）**  
> **课程角色**：用 shared memory 和 synchronization 处理 cooperative block work；sidecar 在完成 lifecycle 后学习 capability negotiation 与 tools/resources/prompts 的选择。

## 开始前：硬先修与 remediation

- **硬先修**：能画 global-memory warp addresses，naive 2D access 有 CPU oracle。
- **先修检查**：说明为什么 block 内共享数据需要“写入完成→barrier→读取”的 happens-before。
- **失败时 remediation**：先用 4-thread 纸面 timeline；若仍无法保证所有 threads 到达 barrier，只实现无条件 barrier 的 fixed-size guided case，不写条件 barrier。

## Observable objectives

- 区分 block scope shared memory、register/global memory 与 lifetime。
- 正确放置 `__syncthreads()`，解释 divergent barrier/race。
- 实现 edge-safe tiled transpose 或等价 shared-memory pattern。
- 用 Compute Sanitizer 的 memcheck/racecheck/synccheck 观察故障与修复。
- 解释 capability advertisement≠authorization，并按语义选择 tool/resource/prompt。

## 术语表

需能解释：**shared memory、tile、bank、bank conflict、barrier、race、happens-before、divergent barrier、memcheck、racecheck、synccheck、capability negotiation、tool、resource、prompt、advertisement、authorization**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[CUDA Programming Guide 2.3.4 — Memory Performance / Shared Memory](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/writing-cuda-kernels.html#memory-performance)。
- **T2（Day 2，20 分钟）**：[CUDA Programming Guide 5.4 — C/C++ Language Extensions / Synchronization Functions](https://docs.nvidia.com/cuda/cuda-programming-guide/05-appendices/cpp-language-extensions.html)。
- **T3（Day 3，15 分钟）**：[Compute Sanitizer Documentation](https://docs.nvidia.com/cuda/compute-sanitizer/index.html) — memcheck、racecheck、synccheck overview。
- **S1（Day 1，20 分钟）**：[MCP Lifecycle — Capability negotiation](https://modelcontextprotocol.io/specification/2025-11-25/basic/lifecycle#capability-negotiation)。
- **S2（Day 2，20 分钟）**：[MCP Server concepts](https://modelcontextprotocol.io/docs/learn/server-concepts) — tools、resources、prompts。

95 分钟计入 Day 1–3；Podcast/blog 不作必修。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **先修或 Gate 落后时取消**：从 [Simon Willison’s Weblog](https://simonwillison.net/) 选择一篇近 12 个月、标题或标签明确涉及 MCP、tools、prompt injection 或 agent security 的文章；必须跟到它引用的协议/产品原始来源。
- 在 `notes/feed/week-16.md` 写 `claim → source/evidence → scope → experiment/decision`，将一个安全 claim 转成 capability/primitive negative fixture；观点不覆盖 MCP stable spec。不计 unlock。

## 带问题阅读

- `__syncthreads()` 既是 rendezvous 还是也提供 memory ordering？
- edge tile 的 inactive threads 为什么仍可能需要到达 barrier？
- bank conflict、race 与 uncoalesced global access 分别是什么层级问题？
- server 声明 capability 为什么不等于用户授权某次 action？

## Worked example / 手算

用 `2×2` tile 转置 `4×4` matrix：每 block 四 threads 先各读一个 global element 写入 `tile[ty][tx]`，所有 threads barrier 后再从 `tile[tx][ty]` 读。若只有 `tx==0` 的 threads 执行 barrier，其他 threads 跳过，行为非法；边界 thread 即使不写有效数据，也必须沿一致控制路径到 barrier，再用 bounds check 控制读写。

## 累计项目 increment

- `labs/cuda/week16_shared_memory.cu`
- `notes/concepts/shared-memory-synchronization.md`
- `ai-app/architecture/week-16-primitive-selection.md`
- `ai-app/client-tests/capabilities/week-16-fixtures.jsonl`
- `notes/ai-app/week-16-capabilities-primitives.md`

## Exit ticket / unlocks

**形成性**：shared pattern boundary cases 与 sanitizer clean path 有证据；能解释 barrier/race/bank/coalescing；capability fixtures 支持安全降级，primitive table 不混淆 resource/tool/prompt。完成 unlock Week 17；无 GPU 时 sanitizer 项保持明确 blocker。

## Day 1

**阶段：理论 + shared-memory model（100 分钟）**

- **20 分钟**：读 T1。
- **30 分钟**：逐步画顶部 `2×2` tile worked example与 memory timeline。
- **30 分钟**：guided 实现固定 `4×4` tiled pattern + CPU oracle。
- **20 分钟**：读 S1，写 client/server capabilities 与“未声明不得假设”规则。
- **必须留下**：tile/barrier 图、guided correctness 和 capability summary。

## Day 2

**阶段：worked synchronization/primitives（100 分钟）**

- **20 分钟**：读 T2。
- **25 分钟**：手画 correct、missing、divergent barrier 三种 timeline。
- **35 分钟**：扩展到 edge tile，确保所有 participating threads 一致到达 barrier。
- **20 分钟**：读 S2，用“谁控制数据/是否副作用/谁选择”分类 project overview、list runs、profiling workflow。
- **必须留下**：三种 timelines、edge-safe source 和 primitive table。

## Day 3

**阶段：guided sanitizer/capability lab（100 分钟）**

- **15 分钟**：读 T3。
- **60 分钟**：将错误 kernel 与正常代码隔离，运行 memcheck/racecheck/synccheck 中适用项；修复后重跑。无设备保留 exact blocked commands。
- **15 分钟**：创建 supported tools、unsupported prompts、unknown experimental capability fixtures。
- **10 分钟**：保存 tool/protocol/GPU versions。
- **必须留下**：sanitizer before/after 或 blocked evidence、3 个 capability fixtures。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立完成 tiled transpose 或另一个 shared pattern，覆盖 `1×1,31×33,32×32,1000×777`；与 naive/CPU 比较，不预设更快。
- **20 分钟**：验证 unsupported call 明确失败，不 silent fallback 到更强 tool；capability 不改变 permission matrix。
- **10 分钟**：输出 first mismatch。
- **必须留下**：case matrix、correctness/benchmark evidence 和 fail-closed result。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：完成 shared-memory concept note，绑定 sanitizer/correctness/measurement，并写工具 limitations。
- **20 分钟**：做 5 个 capability/primitive scenarios，解释错误选项。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：concept note、5 个 scenarios 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：clean-run naive/shared/boundary/sanitizer path或复核 blocker。
- **25 分钟**：闭卷解释 barrier/race/bank/coalescing。
- **15 分钟**：闭卷重建 capability/primitive decision table。
- **15 分钟**：填写 `notes/weekly/week-16.md`，冻结 Week 17 read-only server scope。
- **必须留下**：形成性 Exit ticket、clean/blocked evidence、primitive table 与 unlock 状态。
