# Week 13 — GPU Architecture、CUDA Toolchain 与 MCP Architecture

> **Unit 3 · GPU Execution and Memory Foundations（第 1/6 周）**  
> **课程角色**：从 CPU runtime 进入 GPU host/device execution 与编译工具链；sidecar 从 typed tool 上升到 MCP host/client/server 的协议架构。

## 开始前：硬先修与 remediation

- **硬先修**：Unit 2 Hard Gate 通过；能读写 C++17、编译最小程序、解释 oracle/correctness/timing boundary。
- **设备检查**：记录是否有 Jetson Orin/CUDA host、GPU model、CUDA Toolkit/driver；只查询，不修改 power/clock。
- **失败时 remediation**：无 CUDA 设备时完成 host-side launch/index simulator、编译命令解析和代码 review，GPU run 标 `blocked-no-device`；不得伪造 latency/profile。

## Observable objectives

- 区分 CPU host、GPU device、SM、CUDA core、memory 与 runtime/driver/toolkit。
- 解释 `.cu` 从 nvcc host/device compilation 到 executable 的高层路径。
- 编译运行 vector add，并用 CPU oracle 检查完整 output。
- 观察 API、launch、execution error 的不同边界。
- 画 MCP host/client/server/data/transport 与 trust boundaries。

## 术语表

需能解释：**host、device、GPU、SM、CUDA core、kernel、runtime API、driver、toolkit、nvcc、PTX、cubin、compute capability、MCP host、client、server、data layer、transport layer**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[CUDA Programming Guide 1.1 — Introduction](https://docs.nvidia.com/cuda/cuda-programming-guide/01-introduction/introduction.html)。
- **T2（Day 2，20 分钟）**：[CUDA Programming Guide 1.2 — Programming Model](https://docs.nvidia.com/cuda/cuda-programming-guide/01-introduction/programming-model.html) — heterogeneous programming 概览。
- **T3（Day 3，20 分钟）**：[CUDA Programming Guide 2.7 — NVCC](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/nvcc.html) — compilation workflow 与常用 command options。
- **S1（Day 1，20 分钟）**：[MCP Architecture](https://modelcontextprotocol.io/docs/learn/architecture) — participants、data/transport layers。

80 分钟计入 Day 1–3；Podcast/blog 不作教材。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **先修或 Gate 落后时取消**：阅读 [NVIDIA Technical Blog: An Even Easier Introduction to CUDA](https://developer.nvidia.com/blog/even-easier-introduction-cuda/)；只选择 vector-add、memory transfer 与 launch/error 相关段落，并记录发布日期/访问日，不能把示例性能迁移到 Orin。
- 在 `notes/feed/week-13.md` 写 `claim → source/evidence → scope → experiment/decision`，把一个 claim 转为本周 vector-add 可验证问题；无 GPU 时 decision 必须标 blocked。不计 unlock。

## 带问题阅读

- Toolkit、driver、runtime 与 device 各自提供什么？
- kernel launch 返回为何不代表 device work 已完成？
- PTX 与目标 cubin 的角色为何不能简化成“源代码/二进制”两个词？
- MCP host 为什么应控制 client lifecycle 与 user authorization？

## Worked example / 手算

Vector add `N=1000`，block size=`256`，grid size=`ceil(1000/256)=4`；共发射 1024 threads，最后 block 只有 `1000-3×256=232` 个有效 threads，24 个必须被 `i<N` 阻断。该手算只说明覆盖，不证明最佳 block size。

## 累计项目 increment

- `notes/environments/week-13-cuda-host.md`
- `labs/cuda/week13_vector_add.cu`
- `notes/concepts/gpu-cuda-toolchain.md`
- `ai-app/architecture/week-13-mcp-architecture.md`

## Exit ticket / unlocks

**形成性**：环境事实可追溯、vector add 全量正确、能解释 compilation/execution/error boundaries，MCP 图含 host/client/server 与两条 trust boundaries。完成 unlock Week 14；无设备可条件解锁，但 GPU evidence 保持 blocked。

## Day 1

**阶段：理论 + environment diagnosis（100 分钟）**

- **20 分钟**：读 T1。
- **25 分钟**：记录 OS/GPU/compute capability/toolkit/driver/compiler；查询失败保留 error。
- **35 分钟**：逐步完成顶部 launch worked example并写 host simulator 验证 coverage。
- **20 分钟**：读 S1，画 MCP 三角色初图。
- **必须留下**：environment manifest、coverage table 和 MCP participant 图。

## Day 2

**阶段：worked host/device model（100 分钟）**

- **20 分钟**：读 T2。
- **25 分钟**：手画 allocation→H2D→kernel→D2H→validation。
- **40 分钟**：guided 写 vector add + CPU oracle；先 `N=32`，检查完整 output。
- **15 分钟**：给 MCP 图标 data/credential/permission owners。
- **必须留下**：execution timeline、source 首版和 trust annotations。

## Day 3

**阶段：guided toolchain lab（100 分钟）**

- **20 分钟**：读 T3。
- **60 分钟**：用 nvcc 或现有 CMake compile/run；记录 exact command、arch flags、artifacts、first error；无设备则做到 compile/inspect 或 blocked reproduction。
- **10 分钟**：为 MCP 图加 local/remote transport 边界。
- **10 分钟**：保存 versions/output。
- **必须留下**：compile/run evidence 与完整 MCP architecture v1。

## Day 4

**阶段：independent correctness lab（100 分钟）**

- **70 分钟**：独立扩展 `N=0,1,31,32,33,255,256,257,1000`；统一 CUDA API/launch checks，输出 max error/canary。
- **20 分钟**：走查“读取项目概览”与“修改设备配置”两例；后者必须拒绝/等待 explicit approval。
- **10 分钟**：clean rebuild。
- **必须留下**：case matrix、error helper 和两条 trust-boundary walkthrough。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：写 toolchain/execution concept note，区分实际观察与无设备推断。
- **20 分钟**：写 5 个 MCP architecture scenarios：secret owner、server compromise、多 client、untrusted output、approval。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：concept note、5 个 scenarios 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：clean-run vector cases或复核 blocked evidence。
- **25 分钟**：闭卷重画 host/device compilation/execution。
- **15 分钟**：闭卷重画 MCP trust boundary。
- **15 分钟**：填写 `notes/weekly/week-13.md` 与 Week 14 indexing question。
- **必须留下**：形成性 Exit ticket、clean/blocked evidence 和 unlock 状态。
