# Core Module 19 — Precision、Quantization 与 Dynamic Shape

## 本周目标

理解 precision 和 shape flexibility 对 correctness、engine build、memory、kernel
selection 与性能的影响。主线完成 FP32/FP16 和一个 dynamic profile；INT8 可选。

## 每日计划

### Day 1 — Precision Correctness Contract

- **10 分钟**：列 output 类型、范围和 downstream interpretation。
- **20 分钟**：阅读 FP16 numerical accuracy 和 TensorRT accuracy considerations。
- **50 分钟**：对代表 inputs 运行 FP32 reference，保存 full outputs/summary；
  定义 max abs、relative、percentile error 或 task check。
- **10–30 分钟**：用人工 perturbation 验证 threshold 不会过宽。
- **必须留下**：precision contract、reference set 和 threshold rationale。

### Day 2 — FP16 Engine Build/Inspection

- **10 分钟**：预测哪些 layers/reformats 可能限制 FP16 收益。
- **20 分钟**：确认实际 TensorRT version 的 FP16/strong typing build方式。
- **50 分钟**：构建 FP16 engine，保存 build command、layer/tensor precision/format
  info 和 warnings。
- **10–30 分钟**：运行 correctness，不通过时先定位 layer/input，不放宽 tolerance。
- **必须留下**：FP16 manifest、correctness 和 reformat/fallback observations。

### Day 3 — FP32/FP16 公平比较

- **10 分钟**：冻结 engine setup、input shapes、warm-up、iterations、power state。
- **50 分钟**：采集 latency/throughput/memory/engine size 和 output error samples。
- **20 分钟**：用 Nsight/engine inspector 判断差异来自哪里。
- **10–30 分钟**：检查 sustained thermal drift。
- **必须留下**：FP32/FP16 table 和“快/不快”的证据边界。

### Day 4 — Dynamic Shape/Profile 设计

- **10 分钟**：选择真实需要变化的 dimensions，不为功能而全部设 dynamic。
- **20 分钟**：阅读 min/opt/max optimization profile 与 runtime shape steps。
- **50 分钟**：修改/export dynamic ONNX；定义 min/opt/max，构建 engine。
- **10–30 分钟**：测试范围外 shape 和错误 dimension，保存 diagnostics。
- **必须留下**：dynamic contract、profile、build command 和 failure case。

### Day 5 — Multi-shape Runtime

- **10 分钟**：列 min/opt/max 和两个 non-opt in-range shapes。
- **20 分钟**：确认 context set-shape、buffer sizing/reallocation/reuse strategy。
- **50 分钟**：C++ runner 逐 shape 执行 correctness/latency；防止按 opt size 错分配。
- **10–30 分钟**：比较 first-use 与 warmed shape，记录 profile边界。
- **必须留下**：multi-shape table、buffer policy 和 error-free boundary cases。

### Day 6 — Quantization 基础与周总结

- **15 分钟**：学习 scale、zero-point、per-tensor/per-channel、Q/DQ，不深入训练。
- **35 分钟**：用 NumPy/PyTorch 对小 tensor 做 quantize/dequantize，测 error。
- **20 分钟**：有稳定余量才尝试已有 Q/DQ model/INT8；否则写清选修入口。
- **20–40 分钟**：完成 `notes/concepts/precision-and-quantization.md` 和 weekly review。
- **必须留下**：FP16/dynamic Gate；INT8 未完成不得被标记为阻塞。

## 实验纪律

- precision 变化不只报告单个 max error。
- 不把 unsupported/fallback layer 隐藏。
- dynamic shape 测量包含多个实际 shapes。
- engine build 和 first-shape initialization 单独记录。

## 交付

- FP32/FP16 correctness/performance 表。
- dynamic profile 定义与多 shape 数据。
- `notes/concepts/precision-and-quantization.md`

## 完成标准

- [ ] FP16 correctness 标准有业务/数值依据。
- [ ] min/opt/max 与实际请求范围一致。
- [ ] 能解释 FP16 何时可能不更快。
- [ ] INT8 未完成不阻塞 Gate，但限制被记录。
