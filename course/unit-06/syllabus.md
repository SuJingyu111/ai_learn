# Unit 6 Syllabus — ORT CUDA、TensorRT 与 Device Integration

**Weeks 31–36 · M6 · Gate G6**

## Entry prerequisite

G2 model/ONNX/oracle + G4 CUDA preprocessor + G5 stream/ownership 全部 PASS。

## Observable outcomes

1. 检查 ORT provider priority、graph partition、CPU fallback 和 profiling；
2. 使用 OrtValue/I/O Binding/user stream 保持输入输出在 device；
3. 解释 TensorRT builder/network/config/engine/runtime/context 的 object model；
4. 用 `trtexec` 建立公平 baseline，再实现 C++ buffers/addresses/`enqueueV3`；
5. 对 FP32/FP16 和 min/opt/max dynamic profiles 做 correctness/performance 比较；
6. 将 CUDA preprocessing device buffer 直接送入 ORT CUDA/TRT backend。

## Concept inventory

`Execution Provider`、`partition`、`fallback`、`OrtValue`、`I/O Binding`、`user stream`、
`builder`、`network`、`config`、`serialized engine`、`runtime`、`execution context`、
`tensor address`、`enqueueV3`、`FP16`、`overflow/underflow`、`optimization profile`、
`min/opt/max shape`、`compatibility`。

## Primary teaching spine

- [ORT Execution Providers](https://onnxruntime.ai/docs/execution-providers/)
- [ORT CUDA Execution Provider](https://onnxruntime.ai/docs/execution-providers/CUDA-ExecutionProvider.html)
- [ORT I/O Binding](https://onnxruntime.ai/docs/performance/tune-performance/iobinding.html)
- [TensorRT Documentation Hub](https://docs.nvidia.com/tensorrt/)
- [TensorRT 10.x Quick Start](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/getting-started/quick-start-guide.html)
- [TensorRT C++ API](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/c-api-docs.html)
- [How TensorRT Works](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/architecture/how-trt-works.html)
- [TensorRT Accuracy Considerations](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/accuracy-considerations.html)
- [TensorRT Dynamic Shapes](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/work-dynamic-shapes.html)

课程链接提供 10.x 文档入口；实验必须记录 Orin 实际 JetPack/TensorRT，并选择匹配 archive。
不要用桌面或 `latest` 生成的 engine 假定可在 Orin 运行。

## Six-week lesson sequence

| Week | Theory | Worked problem | Lab | Project increment |
|---:|---|---|---|---|
| 31 | ORT provider partition/fallback | node assignment map | ORT CUDA baseline/profile | backend contract |
| 32 | I/O Binding/user stream | copy/ownership timeline | device OrtValues | ORT device path |
| 33 | TRT/trtexec/object model | build vs run phase | reproducible trtexec | TRT baseline |
| 34 | C++ buffers/enqueue/lifetime | object lifetime table | TRT C++ inference | RuntimeBackend |
| 35 | FP16/dynamic profiles | error distribution/profile cases | FP16 + min/opt/max | validated variants |
| 36 | direct-device integration | cross-backend table | preproc→runtime | M6 / G6 |

## Required problem set

- 为 mixed-support graph 标出 provider partition、fallback 和 copy boundaries。
- 找出 I/O Binding 中 5 个 ownership/lifetime bugs。
- 画 TensorRT build/runtime phases 与 object lifetimes。
- 对 FP16 overflow/underflow 设计 input，并选择 accuracy metric。
- 判断 8 个 shapes 是否落在 profile 内；解释 opt shape 的意义。

## Sidecar prerequisite chain

在已有 eval/state 基础上学习 sequential/parallel/reviewer/handoff 模式、workspace/tool/
credential isolation、explicit handoff schema、stalled/partial/conflict recovery。只对只读
runtime evidence fixtures 使用 multi-agent，不扩大设备权限。

## G6

- closed-book provider/TRT/precision/dynamic/lifetime exam；
- unseen fallback/hidden-copy/lifetime diagnosis；
- PyTorch、ORT CPU、ORT CUDA、TRT cross-backend correctness table；
- clean device-resident M6，公平比较 fixed/FP16/dynamic 并 oral defense。

silent fallback、hidden host round-trip、engine/context/buffer lifetime 错或版本不可复现
均阻塞 U7。
