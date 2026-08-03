# Step 007 — `tt dump`：打印元数据和真实地址

**你会做出**：一个你会用上几百次的调试工具。

**为什么是现在**：McCarthy 的课里 GDB 是从第一节就在用的。这里的等价物就是
`tt dump`——之后每一个"为什么结果不对"，你都会先 dump 一下。现在做，
不要等到 Phase 04 调 Conv 的时候才临时糊一个。

## 动手

1. `tt dump` 打印：shape、strides、dtype、offset、storage 基址、
   **第一个元素的实际地址**（`storage.data() + offset * dtype_size()`）。
2. 加 `--values` 选项，按行打印前 N 个元素（默认 8 个，多了截断加 `...`）。
3. 让它接受一个"从哪来"的名字参数，这样一次 dump 多个张量时能分辨。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt dump-demo
```

## 你应该看到

`base` 与 `first` 在 offset=0 时相同：

```
name=t  shape=(2,3,4)  strides=(12,4,1)  dtype=f32  offset=0
        base=0x14b606b00  first=0x14b606b00  nbytes=96
values: 0 1 2 3 4 5 6 7 ...
```

## 卡住了

- `first` 与 `base` 差了 4 倍 → 你把 offset 当字节用了，但它的单位是元素
  （Step 005 定的）。这两个单位在后面会咬你很多次，现在就统一。
