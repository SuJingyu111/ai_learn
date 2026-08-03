# Step 011 — `permute` / `transpose`：只改 strides

**你会做出**：`permute`，零复制地重排维度。

**为什么是现在**：HWC ↔ CHW 转换是推理前处理的核心操作，而它本质上就是 permute。
先看到"permute 是免费的、但让张量变得非 contiguous"，Phase 06 讨论
layout 选择时才有基础。

## 动手

1. `Tensor::permute(std::initializer_list<int> order)`：
   同时按 order 重排 `shape_` 和 `strides_`，其他一律不动。
2. 校验 order 是 `0..rank-1` 的一个排列（不能重复、不能缺）。
3. `transpose(a, b)` 就是只交换两维的 permute。

## 跑起来

对 `(2,3,4)` 做 `permute(2,0,1)`：

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt permute-demo
```

## 你应该看到

shape 和 strides 被同步重排，base 不变：

```
t          shape=(2,3,4)  strides=(12,4,1)  base=0x14b606b00  contiguous=yes
t.permute(2,0,1)  shape=(4,2,3)  strides=(1,12,4)  base=0x14b606b00  contiguous=no
```

strides 变成了 `(1,12,4)`——**第一维的步长最小**。这正是非 contiguous 的定义：
strides 不再是 shape 的 row-major 推导结果。

## 卡住了

- strides 还是 `(12,4,1)` → 你只重排了 shape 忘了 strides，
  那样得到的数据是错的（而且不会报错，非常危险）。
  验证方法：permute 后 `at(0,0,0)` 和 `at(0,1,0)` 应该分别是 0 和 12。
