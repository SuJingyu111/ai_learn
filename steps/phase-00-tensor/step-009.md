# Step 009 — `reshape`：什么时候能不复制

**你会做出**：`reshape`，在能共享内存时共享，不能时明确报错。

**为什么是现在**：reshape 看起来只是改个 shape，实际上是第一个"逻辑视图与物理
布局可能对不上"的地方。ONNX 的 `Reshape` 节点在 Phase 04 会用到；
理解它什么时候免费、什么时候必须复制，直接决定后面 layout 转换的代价。

## 需要知道的（5 分钟）

只有当张量是 **contiguous**（strides 恰好等于 shape 推出的 row-major strides）
时，reshape 才能只改元数据。否则元素在内存里的顺序就不是新 shape 期望的顺序。

## 动手

1. `Tensor::reshape(Shape)`：先检查 `numel()` 相等，不等直接报错。
2. 检查是否 contiguous；是则返回共享同一 storage 的新 Tensor（**offset 不变**）。
3. 不是 contiguous 时**先抛异常**。第 012 步会给它加上 `contiguous()` 回退路径。
   现在让它明确失败，好过悄悄给出错误结果。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt reshape-demo
```

## 你应该看到

base 与 reshape 后的 `base=` 地址相同——没有复制：

```
t         shape=(2,3,4) strides=(12,4,1) base=0x14b606b00
t.reshape((6,4))  shape=(6,4) strides=(4,1) base=0x14b606b00   [共享]
t.reshape((5,5))  -> error: numel mismatch: 24 vs 25
```

## 卡住了

- 两个 base 不同 → 你构造了新 Storage 而不是复制 Tensor 的 storage 成员。

---

### 🎯 Additional goal — 自己写引用计数（可选）

**假设**：把 `Storage` 的 `shared_ptr` 换成自己写的侵入式引用计数后，
Tensor 的复制会变快（少一次原子操作 + 少一次控制块解引用），
但在单线程 demo 里这个差异小到测不出来。

**做什么**：保留现在的实现，另写一个 `StorageIntrusive`，用同一套 demo 各跑一遍。

**测什么**：`engine/bench` 的 harness，测 100 万次 Tensor 复制构造的 median/p95。
再用 `nm -C` 或反汇编看看两者的复制路径各有多少指令。

**你可能会发现**：差异在 10% 以内，甚至被噪声淹没。如果你测出了 2× 以上，
先确认编译器没有把整个循环优化掉——把结果累加起来并打印出来。

**为什么会这样**：`shared_ptr` 的复制是一次原子自增；在无竞争的单线程下，
现代 CPU 上的原子自增只比普通自增贵几个周期。**它的代价要到多线程争用时才显现**，
而那正是 Phase 08 会遇到的场景。

**写下来**：假设 → 实测数据 → 结论 → 这个结论在什么条件下不成立。
