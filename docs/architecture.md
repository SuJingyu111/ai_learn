# Architecture

## Target pipeline

```text
Host/camera/synthetic input
        |
        +--> CPU/framework reference ------------------------------+
        |                                                          |
        +--> input buffer                                          |
              -> CUDA resize / normalize / HWC->CHW                |
              -> device tensor                                     |
              -> ORT CUDA or TensorRT inference                    |
              -> postprocess --------------------------------------+--> comparator
                                                                        |
                                                                        +--> report
```

早期 milestone 允许 H2D/D2H 以便独立验证；最终 capstone 要求解释每次 host/device
transition，并优先保持 preprocessing→inference 的 device-resident path。

项目不会等到最后才开始。M1 建立 normalize/layout CPU vertical slice，M2 冻结
model/runtime-input 并建立 framework/ORT CPU oracle，M3 加入 CUDA tensor transforms，
M4 学完并冻结 Resize semantics 后完成 CUDA preprocessor。M5 增加 multi-frame
ownership，M6 接入 runtime，M7–M8 才进入 Orin sustained/release。完整依赖见
[`../course/project/milestones.md`](../course/project/milestones.md)。

## Layers

```text
Application       frame scheduling, input/output semantics
Runtime adapter   ORT/TensorRT session, engine, context, tensor binding
Operator library  resize, normalize, layout conversion, warp
CUDA runtime      streams, events, memory, graph capture
Jetson Orin       GPU, memory subsystem, power/clock/thermal constraints
```

## Ownership

- `ImageView` 是 non-owning view；调用方保证执行期间 memory 有效。
- CPU API 同步返回。
- CUDA operator 接收 stream；调用方拥有 stream 和 device buffers。
- async enqueue 成功不表示 work 已完成。
- event/stream 完成前不得释放或复用相关 buffers。
- ORT/TensorRT adapter 必须说明 input/output tensor、execution context 和 stream 的
  lifetime；第三方 runtime 的隐式同步不得靠猜测。

## Correctness

- CPU/NumPy/PyTorch reference 应简单、可读并独立于 CUDA implementation。
- 固定 hand cases 锁定 coordinate/layout/operator semantics。
- fixed-seed randomized tests 覆盖 odd、single-pixel、padded stride 和 boundaries。
- precision/layout/backend 改变时重新运行 correctness gate。
- tolerance 包含 absolute/relative 或任务相关指标，并说明数值来源。

## Measurement boundaries

报告按适用范围区分：

- model/engine build；
- allocation/registration；
- H2D/input preparation；
- preprocessing kernels；
- inference；
- D2H/postprocess；
- first-frame、steady-state 和 end-to-end；
- throughput、memory、power/thermal。

CUDA event、host clock、Nsight Systems 和 Nsight Compute 回答不同问题。优化不得
改变 API/workload 语义或绕过 correctness tests。

## Evidence 与 Runtime Lab Assistant

```text
runtime execution
  -> versioned manifest / correctness / raw latency / system metrics
  -> fixed-schema artifact store
  -> read-only MCP resources and tools
  -> validate / compare / explain
  -> human engineering decision
```

Assistant 不直接读取任意文件或控制设备。它只消费 allowlisted、versioned evidence；
MCP output 仍视为 untrusted。最终 capability contract 在
[`../course/project/spec.md`](../course/project/spec.md)。

## Reference implementation boundary

现有 CPU/CUDA Resize 源码是 reference，不是学生 starter。G4 前的 independent
implementation 使用 `labs/resize_starter/`、个人 namespace 或隔离分支；通过 Gate 后
才允许 differential review。否则 CPU oracle 与 CUDA practical 不能证明独立掌握。
