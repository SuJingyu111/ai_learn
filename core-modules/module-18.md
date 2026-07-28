# Core Module 18 — TensorRT Engine 与 C++ Runtime

## 本周目标

使用 `trtexec` 建立可信 baseline，再用 TensorRT C++ API 理解 builder、serialized
engine、runtime、execution context、tensor address 和 enqueue 生命周期。

## 每日计划

### Day 1 — `trtexec` FP32 Baseline

- **10 分钟**：记录实际 TensorRT version 和 `trtexec --help` 相关 flags。
- **20 分钟**：确认 fixed ONNX input names/shapes/dtypes 和 correctness reference。
- **50 分钟**：用 `trtexec` parse/build/run FP32；保存完整 command、engine、layer
  info 和 output log。
- **10–30 分钟**：区分 build 与 inference metrics，检查 engine 文件/版本信息。
- **必须留下**：命令、log、engine generation manifest，不只留下终端截图。

### Day 2 — Build/Load/Run Timing Boundary

- **10 分钟**：画 parse→build/tactics→serialize→load→context→warm-up→enqueue。
- **20 分钟**：阅读 TensorRT build/runtime object overview。
- **50 分钟**：分别采集 build time、engine size/load、warm-up 和 steady latency；
  重复 run 但不每次 rebuild。
- **10–30 分钟**：保存 raw samples、power/thermal 和 `trtexec` measurement mode。
- **必须留下**：stage table 和哪些数字不能横向比较的说明。

### Day 3 — C++ Engine Inspection

- **10 分钟**：列 C++ runner 最小 objects 与 destruction order。
- **20 分钟**：阅读 runtime deserialize、engine I/O tensor APIs。
- **50 分钟**：创建 `labs/runtime/week18_trt_runner.cpp`，反序列化 engine，打印
  tensor names/modes/shapes/dtypes/formats。
- **10–30 分钟**：与 ONNX contract 比较，遇到 dynamic/unknown dimension 先报错。
- **必须留下**：inspector、build command、I/O metadata table。

### Day 4 — Device Buffers 与 Enqueue

- **10 分钟**：计算每个 I/O buffer bytes，写 overflow checks。
- **20 分钟**：设计 RAII：runtime/engine/context/stream/buffers 的 owner。
- **50 分钟**：分配/reuse device buffers、设置 input shape/address、enqueue，
  在正确边界取 output。
- **10–30 分钟**：运行一组 fixed input，与 saved reference 比较。
- **必须留下**：runner end-to-end path、max error 和 lifetime code/diagram。

### Day 5 — Failure Paths 与 Cross-backend Correctness

- **10 分钟**：列 wrong name/shape/dtype、corrupt engine、allocation failure risks。
- **15 分钟**：将 PyTorch/ORT/TRT 使用同一 serialized input/reference。
- **55 分钟**：实现 cross-backend comparator；测试至少两个 safe failure paths，
  确保 partial resources 被清理。
- **10–30 分钟**：运行 sanitizer/valgrind-like host tool（可用时）或显式 resource log。
- **必须留下**：correctness table、failure outputs 和 cleanup evidence。

### Day 6 — TensorRT Runtime Baseline

- **15 分钟**：clean build/run C++ runner 和 `trtexec`。
- **40 分钟**：完成 TensorRT object/buffer/stream lifetime 图。
- **20 分钟**：写 `notes/concepts/tensorrt-build-runtime.md`。
- **15–30 分钟**：weekly review；冻结 Week 19 FP32 baseline 和 dynamic shape range。
- **必须留下**：C++ runtime baseline、reproduction 和 object model。

## 需要解释

- engine 为什么与硬件/软件版本存在兼容边界。
- execution context 为什么不等于 engine。
- build tactics/timing cache 与 steady-state execution 的区别。
- enqueue 返回与 output 可读之间的同步关系。

## 交付

- `trtexec` baseline command/results。
- 最小 TensorRT C++ runner。
- object/buffer/stream lifetime diagram。

## 完成标准

- [ ] engine build 不混入 inference latency。
- [ ] C++ runner 输出通过 correctness gate。
- [ ] buffers 在所有 async work 完成前保持有效。
- [ ] engine metadata 和实际版本被保存。
