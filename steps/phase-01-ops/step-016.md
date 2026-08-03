# Step 016 — `add` 与形状检查

**你会做出**：两输入的逐元素加法，shape 不匹配时明确报错。

**为什么是现在**：从一元到二元，第一次需要回答"两个 shape 怎样算兼容"。
先只支持完全相同的 shape，下一步再引入广播——分两步做，
你才能看清广播到底多做了什么。

## 动手

1. `Tensor add(const Tensor& a, const Tensor& b)`。
2. 现在只接受 `a.shape() == b.shape()`，否则报错，
   错误信息要**同时打印两个 shape**。
3. dtype 也必须一致。混合精度是 Phase 09 的事。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt op-demo add
```

## 你应该看到

```
(2,3) + (2,3) -> ok
(2,3) + (3,2) -> error: shape mismatch: (2,3) vs (3,2)
(2,3) f32 + (2,3) i32 -> error: dtype mismatch: f32 vs i32
```

## 卡住了

- 错误信息只说 "shape mismatch" → 加上两个 shape。你在 Phase 04 调试
  一个 20 层的网络时会需要它。
