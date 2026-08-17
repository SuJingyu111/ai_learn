# Step 003 — `DType`：字节宽度与类型标签

**你会做出**：一个 `DType` 枚举和 `dtype_size(dt)`。

**为什么是现在**：`numel()` 告诉你有多少个元素，但分配内存需要知道多少**字节**。
这两个数在整个 runtime 里会被反复搞混——ONNX 的 tensor 描述、CUDA 的 `cudaMalloc`、
stride 的单位，各自用的不是同一个。现在就把它们分开。

## 动手

1. `engine/include/tt/dtype.h`：先只要 `F32`、`I64`、`I32`、`U8` 四个。
   （`I64` 是必须的——ONNX 的 shape 张量就是 int64。）
2. `dtype_size(DType)` 返回字节数，`dtype_name(DType)` 返回字符串。
3. 对未知枚举值，**不要**默默返回 0；abort 或抛异常。返回 0 会让后面的
   分配大小静默变成 0，那种 bug 极难查。

## 本项目相对默认实现的偏移

- 本项目沿用 `common.h` 的状态码风格：类型名为 `Dtype`，`dtype_size` 和
  `dtype_name` 返回 `Status`，通过输出参数交付结果，而不是直接返回结果。
- 未知枚举值返回 `Status::EWRONGPARAM`，且不修改输出参数；不采用本步骤默认建议的
  abort 或异常。调用方处理运行时传入的 dtype 时必须检查状态码。`dtype-demo` 只传入
  编译期确定的四个合法枚举值，因此省略状态码分支。
- Demo 只要求类型标签和字节数在语义上一致；当前输出在标签后带逗号。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt dtype-demo
```

## 你应该看到

```
f32, size=4
i64, size=8
i32, size=4
u8, size=1
```

## 卡住了

- switch 漏了分支但编译通过 → 打开 `-Wswitch`（`-Wall` 已经包含），
  别用 `default:` 兜底，那会让漏掉的新类型在编译期变成静默 bug。
