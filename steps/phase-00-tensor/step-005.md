# Step 005 — `Tensor` = storage + shape + dtype + offset

**你会做出**：把前四步拼起来的 `Tensor`。

**为什么是现在**：这四个字段就是张量的全部。多一个都是冗余，少一个就有东西
表达不了。特别是 `offset`——现在加进去，第 010 步的 slice 才不用重写。

## 动手

1. `engine/include/tt/tensor.h`：字段为 `Storage storage_`、`Shape shape_`、
   `DType dtype_`、`int64_t offset_`（**单位是元素，不是字节**——记下你的选择，
   两种约定都有人用，混淆是经典 bug 源）。
2. `Tensor::zeros(shape, dtype)` / `Tensor::empty(shape, dtype)` 工厂函数。
3. `nbytes()` = `numel() * dtype_size()`。注意这是**存储**大小的下界，
   不是上界——第 011 步转置之后就不再相等了。先别急着优化这个。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt tensor-demo
```

## 你应该看到

```
t = empty((2,3,4), f32)  numel=24  nbytes=96  offset=0
```

## 卡住了

- `nbytes` 算成 24 → 你漏了乘 `dtype_size`。
- offset 的单位没写在代码注释里 → 现在就补上。你三个月后会感谢自己。
