# Step 017 — 广播规则：先手算，再实现

**你会做出**：NumPy/ONNX 的广播语义。

**为什么是现在**：ONNX 里几乎每个二元算子都支持广播，bias 加法就是典型。
Phase 04 绕不过去。

## 需要知道的（8 分钟）

从**尾部**对齐两个 shape，逐维比较：相等则取该值；有一个是 1 则取另一个；
否则不兼容。缺失的高维按 1 处理。

## 先手算

`(3,1)` 和 `(1,4)` 广播后的 shape 是什么？把结果矩阵完整写出来，
其中 A = `[[1],[2],[3]]`，B = `[[10,20,30,40]]`。

写完再往下。

## 动手

1. `Shape broadcast_shape(const Shape&, const Shape&)`，不兼容时报错并
   打印是**哪一维**冲突。
2. 广播的实现：把输入的 strides 扩展——**被广播的维度 stride 设为 0**。
   这是零复制广播的核心技巧：stride 0 意味着索引怎么变，地址都不动。
3. `add` 改成先求广播 shape，再按扩展后的 strides 读两个输入。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt op-demo broadcast
```

## 你应该看到

```
(3,1) + (1,4) -> (3,4)
11 21 31 41
12 22 32 42
13 23 33 43

(3,1) strides after broadcast = (1,0)
(1,4) strides after broadcast = (0,1)

(2,3) + (4,3) -> error: cannot broadcast dim 0: 2 vs 4
```

## 卡住了

- 结果是 `11 12 13 / 21 22 23 / ...` → 你把两个输入弄反了，或者行列写反了。
- stride 0 让你不安 → 它只用于**读**。任何时候都不要往 stride 为 0 的张量里写，
  那会让多个逻辑位置写到同一个地址。在代码里加断言挡住这件事。
