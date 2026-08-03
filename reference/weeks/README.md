# 48 周日常执行入口

这里是每天学习的唯一导航。每个周文件会直接给出：

- Unit、当前 milestone、硬先修与不通过时的 remediation；
- observable learning objectives 和本周术语；
- 精确到页面/章节的必修教材与官方参考，阅读时间已计入日程；
- worked problem、guided lab、independent transfer；
- Runtime + Agent/MCP/GH-600 累计项目增量；
- Day 1–6 的 90–120 分钟安排、必须留下和 Exit Ticket；
- Major Gate 周的评分、hard blockers 与解锁条件。

执行规则：

1. 只打开当前周；按 Day 1–6 执行。
2. Day 6 Exit Ticket 未过，先补 prerequisite，不因日期跳周。
3. Week 6/12/18/24/30/36/42/48 的 Major Gate 未 PASS，不打开下一 Unit。
4. 每周文件末尾的 Feed Card 是 30–45 分钟 optional seminar；可在通勤/散步完成，
   或替换当周一段非 Gate review，**不得叠加到 12 小时以上**。Core 落后时首先取消。
5. Unit 结构见 [`../CURRICULUM.md`](../CURRICULUM.md)，评分见
   [`../docs/mastery-gates.md`](../docs/mastery-gates.md)。

## Unit 1 — Tensor 与 Inference Math Bridge

| Week | 正式课程 + 副线 |
|---:|---|
| [01](week-01.md) | 入学复测、array/tensor metadata + Git/GitHub Flow |
| [02](week-02.md) | stride/layout/view-copy + GitHub Actions/SDLC |
| [03](week-03.md) | 线性代数、MatMul/broadcast/reduction + agent fundamentals |
| [04](week-04.md) | Conv/operator dataflow + GitHub control plane |
| [05](week-05.md) | numerical tolerance、training/inference boundary + architecture |
| [06](week-06.md) | M1 CPU vertical slice、G1 + PR governance/instructions |

## Unit 2 — Model Lifecycle、ONNX 与 ORT CPU

| Week | 正式课程 + 副线 |
|---:|---|
| [07](week-07.md) | Module/state/checkpoint/eval + custom agent |
| [08](week-08.md) | computational graph/autograd mental model + eval baseline |
| [09](week-09.md) | ONNX IR/opset/export + typed tool contract |
| [10](week-10.md) | ORT CPU/provider/optimization + schema/results/errors |
| [11](week-11.md) | correctness/tolerance/benchmark statistics + permissions |
| [12](week-12.md) | M2 clean CPU pipeline、G2 + failure/retry/rollback |

## Unit 3 — CUDA Programming Foundations

| Week | 正式课程 + 副线 |
|---:|---|
| [13](week-13.md) | GPU architecture/toolchain + MCP architecture |
| [14](week-14.md) | grid/block/warp/SIMT + JSON-RPC/lifecycle |
| [15](week-15.md) | memory hierarchy/coalescing + capability negotiation |
| [16](week-16.md) | synchronization/shared memory + tools/resources/prompts |
| [17](week-17.md) | error model/event timing/sanitizer + server skeleton |
| [18](week-18.md) | M3 CUDA transforms、G3 + protocol/server hardening |

## Unit 4 — GPU Performance 与 CUDA Preprocessor

| Week | 正式课程 + 副线 |
|---:|---|
| [19](week-19.md) | profiler mental model/NVTX + resources/prompts |
| [20](week-20.md) | transpose/reduction/shared/occupancy + Inspector/tests |
| [21](week-21.md) | sampling/ONNX Resize semantics + Streamable HTTP |
| [22](week-22.md) | independent CPU Resize oracle + authorization |
| [23](week-23.md) | CUDA Resize/Nsight + MCP threat model |
| [24](week-24.md) | M4 optimization case study、G4 + secured deployment |

## Unit 5 — Concurrency、Ownership 与 Multi-frame Pipeline

| Week | 正式课程 + 副线 |
|---:|---|
| [25](week-25.md) | streams/events/default stream + memory selection |
| [26](week-26.md) | pinned async transfer/Orin memory + durable state |
| [27](week-27.md) | ownership/frame-slot state machine + TTL/reset |
| [28](week-28.md) | double-buffer pipeline + resume/context drift |
| [29](week-29.md) | timeline/overlap + eval dataset/failure taxonomy |
| [30](week-30.md) | M5 async case study、G5 + trace/tuning evidence |

## Unit 6 — ORT CUDA、TensorRT 与 Device Integration

| Week | 正式课程 + 副线 |
|---:|---|
| [31](week-31.md) | ORT CUDA partition/fallback + eval traces |
| [32](week-32.md) | I/O Binding/user stream + controlled tuning |
| [33](week-33.md) | TensorRT/`trtexec`/object model + multi-agent patterns |
| [34](week-34.md) | TensorRT C++ buffers/enqueue/lifetime + isolation |
| [35](week-35.md) | FP16/dynamic profiles + handoff |
| [36](week-36.md) | M6 direct-device integration、G6 + recovery |

## Unit 7 — Orin End-to-end Systems Engineering

| Week | 正式课程 + 副线 |
|---:|---|
| [37](week-37.md) | beta contract/end-to-end phases + autonomy risk |
| [38](week-38.md) | precision/dynamic validation/quantization + HITL |
| [39](week-39.md) | device-resident preprocessing + audit |
| [40](week-40.md) | stream integration/timeline + security red team |
| [41](week-41.md) | Orin power/thermal/reliability + GitHub MCP/CI |
| [42](week-42.md) | M7 sustained beta/evidence service、G7 |

## Unit 8 — Capstone Studio、Portfolio 与 GH-600

| Week | 正式课程 + 副线 |
|---:|---|
| [43](week-43.md) | controlled optimization + production evidence |
| [44](week-44.md) | sustained validation/standard Actions CI |
| [45](week-45.md) | failure injection/reproduction/security |
| [46](week-46.md) | three case studies + GH-600 six-domain gaps |
| [47](week-47.md) | self-authored unofficial mock + demo defense |
| [48](week-48.md) | M8 final release、G8 + Runtime Lab Assistant v1 |

## 节奏调整

- 只有 90 分钟：保留 prerequisite theory、主要 correctness task 和必须留下；取消 seminar。
- 环境阻塞：保留 exact command、第一条真实错误、expected/actual、版本和下一步。
- 中断超过 1 周：先用新题重做上一周 Exit Ticket。
- 需要深挖：先查当前 [`course Unit syllabus`](../course/README.md)，再按需使用
  [`../core-modules/`](../core-modules/)；旧 Core Module 不改变正式顺序。
