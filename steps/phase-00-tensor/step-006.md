# Step 006 — row-major strides 与 flat offset 公式

**你会做出**：从 shape 算出 row-major strides，以及 `(i,j,k) → flat offset` 的映射。

**为什么是现在**：这是整个引擎最核心的一个公式。CUDA 的 coalescing、im2col 的
布局选择、ONNX 的 layout 转换，全部建立在它上面。先手算，再写代码。

## 需要知道的（8 分钟）

row-major 下，最后一维变化最快。stride[i] 是"第 i 维索引加 1 时，
flat offset 增加多少个元素"。

细节与几个易混点见 [reference/core-modules/module-01.md](../../reference/core-modules/module-01.md)。

## 先手算

对 `shape=(2,3,4)`：

- `strides = (?, ?, ?)`
- `(1,2,3)` 的 flat offset = ?
- `(0,1,2)` 的 flat offset = ?

算完再往下看，别跳过——这三个答案就是下面的断言。

## 动手

1. `Shape::contiguous_strides()`：从最后一维往前累乘。
2. `Tensor::strides()` 存成成员（**不是每次现算**）——第 011 步的 permute 会
   直接改它，那时候"现算"就不成立了。
3. `flat_offset(indices)` = `offset_ + Σ index[i] * stride[i]`。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt stride-demo
```

## 你应该看到

```
shape=(2,3,4)  strides=(12,4,1)
(1,2,3) -> 23
(0,1,2) -> 6
```

## 卡住了

- strides 是 `(1,2,6)` → 你从前往后累乘了，那是 column-major。
- `(1,2,3)` 得到 11 → 检查是不是漏加了 `offset_`，或者维度顺序反了。
