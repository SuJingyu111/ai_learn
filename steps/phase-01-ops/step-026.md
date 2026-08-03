# Step 026 — 把算子接进统一的注册表

**你会做出**：一张"算子名 → 实现"的表，为 Phase 02 的图执行做准备。

**为什么是现在**：Phase 02 的图执行器拿到的是 ONNX 里的字符串 `"Relu"`、
`"MatMul"`，它需要一个地方去查对应的函数。现在只有 5 个算子，
接线成本最低；等到 Phase 04 有 20 个的时候再做就是重构了。

## 动手

1. `engine/include/tt/registry.h`：
   `using OpFn = std::function<std::vector<Tensor>(const std::vector<Tensor>&, const Attributes&)>;`
   先给 `Attributes` 一个空壳（Phase 03 解析 ONNX 时才填）。
2. `register_op(name, fn)` 和 `lookup_op(name)`，查不到时报错并
   **列出所有已注册的名字**——Phase 04 遇到不支持的算子时这条信息价值极高。
3. 用 ONNX 的官方算子名注册：`Relu`、`Add`、`MatMul`、`Softmax`。
   现在就对齐命名，Phase 03 就不用做映射了。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt ops-list
```

## 你应该看到

```
registered ops (4):
  Add
  MatMul
  Relu
  Softmax

lookup("Conv") -> error: unknown op "Conv"; registered: Add, MatMul, Relu, Softmax
```

## 卡住了

- 静态初始化顺序问题（注册表还没构造就往里注册）→
  把注册表放进一个函数内的 static 局部变量，返回引用。
  这是 C++ 的经典陷阱，真实 runtime 都这么处理。
