# Step 010 — `slice`：共享 storage 的 view

**你会做出**：`slice`，返回一个指向原数据中间某处的 Tensor。

**为什么是现在**：这是 `offset` 字段真正开始起作用的地方。也是第一次出现
**非 contiguous** 的张量——后面所有关于 layout 的麻烦都从这里开始。

## 动手

1. `Tensor::slice(dim, start, end)`：只改 `shape_.dim(dim)` 和 `offset_`，
   **strides 完全不变**。这是关键——正因为 strides 不变，行与行之间的间隔
   还是原来那么大，于是切出来的张量不再 contiguous。
2. 支持连续调用：`t.slice(0,1,3).slice(1,1,3)`。
3. 越界的 start/end 要报错，`start >= end` 也要。

## 跑起来

对一个 `(3,4)`、值为 `0..11` 的张量做 `[1:3, 1:3]`：

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt slice-demo
```

## 你应该看到

offset 变成 5，strides 保持 `(4,1)` 不变，base 地址相同：

```
base    shape=(3,4)  strides=(4,1)  offset=0  base=0x14b606b00
sliced  shape=(2,2)  strides=(4,1)  offset=5  base=0x14b606b00   [共享]
values: 5 6 9 10
```

注意 `strides=(4,1)` 而 shape 是 `(2,2)`——**行距 4 大于行宽 2**。
这就是非 contiguous。

## 卡住了

- values 是 `5 6 7 8` → 你按新 shape 连续读了，没有用 strides 跳行。
- offset 是 1 或 4 → offset 应该是 `start_row * stride_row + start_col * stride_col`
  = `1*4 + 1*1` = 5。
