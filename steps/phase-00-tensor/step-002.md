# Step 002 — `Shape`：rank、维度、元素总数

**你会做出**：一个能表示任意维度的 `Shape`，并且能算出元素总数。

**为什么是现在**：shape 是张量身上唯一一个"纯逻辑"的属性——它不关心内存。
先把它单独拿出来，后面才能清楚地看到 strides 才是连接逻辑与物理的那一层。

## 动手

1. `engine/include/tt/shape.h`：用 `std::vector<int64_t>` 存维度，或者定长数组
   加一个 rank（真实 runtime 常这么做以避免堆分配——两种都行，选一个并说明理由）。
2. 提供 `rank()`、`dim(i)`、`numel()`（所有维度相乘）。
3. **rank 0 是合法的**：标量的 shape 是空的，`numel()` 应该返回 1，不是 0。
   这不是特例，是乘法单位元；后面 reduction 会依赖它。
4. 任何维度为负数应该直接 abort 或抛异常。维度为 0 是合法的（空张量）。

## 跑起来

给 `tt` 加一个 `shape-demo` 子命令，构造几个 shape 并打印：

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt shape-demo
```

## 你应该看到

```
shape=()        rank=0  numel=1
shape=(5)       rank=1  numel=5
shape=(2,3,4)   rank=3  numel=24
shape=(2,0,4)   rank=3  numel=0
```

## 卡住了

- `numel()` 对 rank 0 返回 0 → 你的累乘初值写成了 0，应该是 1。
