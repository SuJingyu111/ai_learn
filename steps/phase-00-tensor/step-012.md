# Step 012 — `is_contiguous` 与 `contiguous()`

**你会做出**：判断是否连续，以及在必要时**真正复制**成连续布局。

**为什么是现在**：前两步造出了非 contiguous 张量，Step 009 的 reshape 会对它们
直接报错。现在补上回退路径。更重要的是：这是引擎里**第一次发生真实的数据复制**，
而复制是有代价的——Phase 06 优化时你会回来看这里。

## 动手

1. `is_contiguous()`：把 shape 推成 row-major strides，与实际 strides 比较。
   注意 dim==1 的维度其 stride 可以是任意值而不影响连续性（先忽略这个细节，
   在注释里标记 TODO；真实 runtime 都要处理）。
2. `contiguous()`：已连续则返回自身（共享），否则分配新 storage，
   **按逻辑顺序**逐元素搬过去。
3. 让 `reshape` 在非连续时不再抛异常，而是先 `contiguous()`。
   但要打一条可开关的日志——静默的复制是性能问题的头号来源。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt contiguous-demo
```

## 你应该看到

permute 后的张量复制成连续时 base **改变**了，且数据按新逻辑顺序排列：

```
p        shape=(4,2,3) strides=(1,12,4) base=0x14b606b00 contiguous=no
p.contiguous()  shape=(4,2,3) strides=(6,3,1) base=0x14b6071a0 contiguous=yes  [已复制]
values: 0 4 8 12 16 20 1 5 ...
```

## 卡住了

- base 没变 → 你走了"已连续"分支，检查 `is_contiguous()` 的比较逻辑。
- values 顺序不对 → 复制时你按物理顺序读了，应该按**逻辑索引**遍历，
  用 `at()` 或等价的 strides 计算。
