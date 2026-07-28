# Week 17 — Unit 3：CUDA Error Model、Timing 与 Sanitizer

## Unit / 课程角色

- **Unit 3 · CUDA Foundations Lab，第五周 / Gate 前收束。** 本周把“kernel 能跑”升级为“错误可观察、计时边界可解释、内存问题可复现”。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：建立 Runtime Lab Assistant 的 local read-only MCP server 骨架；Week 18 才用 Inspector 系统调试。

## 硬先修与 Remediation

- **硬先修**：能解释 grid/block/thread、host/device memory、kernel launch；能 clean-build 一个 CUDA target；已通过 Week 16 exit ticket。
- **开周诊断（限 15 分钟）**：闭卷写出一次 H2D→kernel→D2H 的所有潜在错误观察点，并运行一个有 bounds check 的 smoke kernel。
- **未通过时**：暂停新实验，回到 Week 15–16 的 indexing、lifetime 与 copy checklist；完成 `N=1,31,32,33` smoke cases 后再开始 Day 1。无 NVIDIA GPU 时可以完成阅读/代码审阅，但 sanitizer、event timing 与 exit ticket 标记 `BLOCKED`，不能写“通过”。

## Observable Objectives

完成本周后，你能够：

1. 给定一段 CUDA host code，区分 API validation、launch error 与 asynchronous execution error。
2. 在不把 production API 强制同步化的前提下，选择正确的 error-observation boundary。
3. 解释 host wall clock 与 CUDA event 各自包含/排除的工作，并给出可重现实验。
4. 用 Compute Sanitizer 定位一个隔离故障，保存 error→location→fix→clean rerun 证据链。
5. 启动一个只暴露固定、read-only `project_overview` 的 local stdio MCP server；禁止任意 shell、任意 path 与写操作。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| API validation error | runtime API 在调用处即可发现的参数/状态错误 |
| launch error | launch 配置或提交阶段可观察的错误 |
| asynchronous error | device work 执行后，直到后续 observation boundary 才暴露的错误 |
| observation boundary | event/stream/device sync 或后续 API 使既有异步错误可见的位置 |
| CUDA event | 记录在指定 stream 中、用于 GPU timeline 依赖与计时的对象 |
| memcheck | Compute Sanitizer 中检查 device memory access/API misuse 的工具 |
| MCP lifecycle | initialize→capability negotiation→operation→transport shutdown |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Runtime API — Error Handling](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__ERROR.html)，读 error value、last-error 行为与异步错误说明 | 20 |
| 1 | [MCP 2025-11-25 — Lifecycle](https://modelcontextprotocol.io/specification/2025-11-25/basic/lifecycle)，读 Initialization、Capability Negotiation、Shutdown、Timeouts | 15 |
| 2 | [CUDA C++ Best Practices Guide — Timing](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#timing)，读 CPU Timers 与 CUDA GPU Timers | 20 |
| 3 | [Compute Sanitizer User Guide — Using Compute Sanitizer](https://docs.nvidia.com/cuda/compute-sanitizer/index.html)，读 “Using Compute Sanitizer”、memcheck invocation、error output、exit behavior | 20 |
| 3 | [MCP 2025-11-25 — Tools](https://modelcontextprotocol.io/specification/2025-11-25/server/tools)，读 Capabilities、Listing Tools、Tool schema 与 Error Handling | 15 |

## 带问题阅读

- 哪些 API 的成功只说明“提交成功”，而不是“device work 正确完成”？
- 为什么在每次 launch 后 `cudaDeviceSynchronize()` 会提高可观察性，却破坏真实 async contract？
- event elapsed time 为什么不能替代端到端 wall time？
- MCP capability advertisement、input schema 与 authorization 分别解决什么问题？
- 为什么 read-only tool 仍需固定数据源、timeout、output validation 与日志脱敏？

## Worked Example / 手算

对 `N=33`、block size `32` 的 `y[i] = 2x[i] + 1`：

1. 手算 grid size 为 2，第二个 block 中只有 lane 0 合法。
2. 分别推演“缺少 `i < N`”“非法 block size”“D2H 前未等待”会在哪个 boundary 暴露。
3. 画 host wall-clock 区间与 start/stop events 在 stream 中的位置；写出两种时间各自包含的 bytes/work。
4. 为 MCP `project_overview` 写零参数 schema：`{"type":"object","additionalProperties":false}`；说明为何不接受 `path`。

## 累计项目 Increment

- CUDA：`labs/cuda_foundations/error_timing/`（独立 target、error matrix、event benchmark、sanitizer fixture）。
- AI App：`ai-app/server/`（stdio、read-only、固定 overview tool/resource、typed output）与 `ai-app/client-tests/week-17/`。
- 周证据：`notes/curriculum/week-17.md`；raw 输出必须链接 source identity、GPU/driver/tool versions 与 exact command。

## 本周必须产出

- error-observation matrix、host/event timing table、一个隔离错误 kernel 与 clean sanitizer rerun。
- 可运行的 local read-only MCP skeleton、initialize trace、tool schema 与三条 deny tests。
- 一份 worked example、一次 independent variation、Exit ticket。

## Day 1 — Theory：Error Model 与 MCP Lifecycle（105 分钟）

- **20 分钟｜必修理论**：阅读 CUDA Runtime API Error Handling；写出 immediate/launch/asynchronous 三类，不凭记忆扩展语义。
- **15 分钟｜必修理论**：阅读 MCP 2025-11-25 Lifecycle，画 initialize request/response、`notifications/initialized` 与 shutdown 顺序。
- **30 分钟｜概念建模**：对 malloc、H2D、launch、event record、D2H、free 建 error-observation matrix；每格写“可能报告当前错误/先前异步错误/不保证”。
- **25 分钟｜Worked example**：完成 `N=33` 地址、grid 与三个错误路径手算。
- **15 分钟｜检索练习**：闭卷回答 5 个术语，再对照文档纠错。
- **必须留下**：`notes/curriculum/week-17.md` 中的 error matrix、lifecycle 图、手算与纠错记录。

## Day 2 — Guided Practice：错误路径与双计时器（105 分钟）

- **20 分钟｜必修理论**：阅读 Best Practices Guide 的 Timing；列 wall clock/event 的 timed region 与必要同步。
- **45 分钟｜Guided lab**：在独立 `error_timing` target 实现正确 transform、非法 launch fixture、隔离 OOB fixture；分别在 launch check 与明确 sync/event boundary 观察。
- **25 分钟｜MCP guided**：固定 `project_overview` input/output schema、allow-listed data、stdio command 与最小 environment；stdout 只传协议，stderr 才写日志。
- **15 分钟｜验证**：对同一正确 workload 采 host wall 与 event raw samples；写出数字不可直接比较的原因。
- **必须留下**：三条错误路径、exact commands、raw timing 与 server contract；错误 kernel 不进入默认正常 target。

## Day 3 — Guided → Independent：Sanitizer 与 Server Skeleton（105 分钟）

- **20 分钟｜必修理论**：阅读 Compute Sanitizer 指定章节，先写预期 diagnostic/exit code。
- **15 分钟｜必修理论**：阅读 MCP Tools 指定章节，标出零参数 schema、structured output 与 tool-execution error。
- **45 分钟｜Core lab**：对隔离 OOB fixture 运行 memcheck，定位首个 invalid access；修复后用相同 command clean rerun，并说明 sanitizer 未证明什么。
- **15 分钟｜Sidecar build**：实现 server initialization 与固定 read-only overview tool/resource；无 `path`/`command` 参数、无 write capability。
- **10 分钟｜检查**：用一个合法和一个 unknown method smoke request 检查结构化 response。
- **必须留下**：sanitizer before/after、server code、initialize/tool trace 与版本信息。

## Day 4 — Independent Lab：Error/Timing Matrix（105 分钟）

- **60 分钟｜独立实现**：自行增加 `N=0,1,31,32,33,1000`、不同 blocks 与一个 async failure case；冻结 warm-up/iterations，保存 correctness、host/event raw samples。
- **30 分钟｜Sidecar independent**：增加 malformed JSON、unknown tool、extra property、伪 `path` 四个 fixtures；全部 fail closed，随后合法请求仍成功。
- **15 分钟｜证据审阅**：检查日志不含 token、绝对私人路径或完整不可信内容；把 failure 与 recovery 关联到 case ID。
- **必须留下**：case matrix、raw CSV、四条 MCP negative results 与 redacted stderr。

## Day 5 — Integration：统一 Evidence Harness（105 分钟）

- **55 分钟｜Core integration**：写一条 clean command 依次执行 correctness→error fixture→memcheck→timing；正常/故障 target 分离，任何一步失败返回非零。
- **35 分钟｜Sidecar integration**：用固定 client 跑 initialize→tools/list→tools/call→shutdown；加入 timeout 与进程回收，禁止无限 retry。
- **15 分钟｜跨轨连接**：让 overview 只报告 allow-listed test artifact metadata（名称、hash、状态），不得让 client 指定 filesystem path。
- **必须留下**：一键复现 transcript、MCP smoke report 与 artifact metadata schema。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：闭卷分析一段含 missing bounds、错误 event placement、过早 free 的代码；指出错误类别、最早可靠 observation boundary 与修复，再运行一条 unseen-size test。
- **35 分钟｜Sidecar assessment**：从 clean process 启动 server，演示 lifecycle、合法 read-only call、invalid schema、timeout/shutdown；解释 capability 不等于 authorization。
- **25 分钟｜Exit ticket**：用 150–250 字回答“为什么 enqueue success 不是 correctness，为什么 sanitizer clean 也不是完整证明”；链接全部 evidence 并列一个 limitation。
- **必须留下**：graded answers、unseen test、server demo transcript 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在前序 Gate 状态合格、本周 sanitizer/MCP required artifacts 与 Exit Ticket 无 backlog 时做；若 Gate 落后、GPU objective `BLOCKED` 或 remediation 未清，直接取消，不顺延。
- **来源与选择规则**：读 NVIDIA Technical Blog [“Efficient CUDA Debugging: Memory Initialization and Thread Synchronization with NVIDIA Compute Sanitizer”](https://developer.nvidia.com/blog/efficient-cuda-debugging-memory-initialization-and-thread-synchronization-with-nvidia-compute-sanitizer/)；只选一个与本周实际 fixture 对应的 memcheck/racecheck/initcheck/synccheck claim，并回查当前 Toolkit 的 Compute Sanitizer 文档。
- **30–45 分钟流程**：5 分钟选 failure mode → 15–20 分钟读文章/官方文档 → 10–15 分钟写 `claim → evidence → scope → decision`；没有本地复现时 decision 只能是新增 hypothesis/test。
- **artifact**：`notes/feed/week-17.md`，含 Toolkit/tool版本、diagnostic片段、一个 fixture/harness 决策。

## Exit Ticket / Unlocks

- **通过条件**：unseen-size correctness；三类错误边界解释正确；sanitizer before/after 可复现；local MCP server 无 shell/path/write，negative cases 后仍健康。
- **解锁**：Week 18 CUDA Transform M3 assessment。任一 correctness/security/reproducibility 条件缺失，只解锁 remediation，不得进入 hard gate。
