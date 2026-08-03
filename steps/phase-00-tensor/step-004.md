# Step 004 — `Storage`：一块带引用计数的裸内存

**你会做出**：`Storage`——拥有一块字节，被最后一个引用释放。

**为什么是现在**：下一步的 `Tensor` 需要能在不复制数据的情况下被切片和转置，
也就是多个 Tensor 指向同一块内存。谁来释放？这就是 `Storage` 存在的理由。
PyTorch、ONNX Runtime 都有完全对应的一层。

## 动手

1. `engine/include/tt/storage.h`：内部 `std::shared_ptr<std::byte[]>`（C++17 起
   `shared_ptr` 支持数组）或者自己写引用计数——**先用 `shared_ptr`**，
   自己写引用计数留到 additional goal。
2. `Storage::allocate(size_t bytes)`、`data()`、`nbytes()`。
3. 分配出来的内存**不要**清零。未初始化就是未初始化——后面你会靠这一点
   发现"忘了写输出"的 bug。清零会把它们伪装成正确结果。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt storage-demo
```

`storage-demo` 应该：分配 64 字节、复制一份 Storage、打印两者的 `data()` 指针
和当前引用计数，然后让副本离开作用域，再打印一次引用计数。

## 你应该看到

两个指针**相同**，引用计数 1 → 2 → 1：

```
a          data=0x14b606b00 nbytes=64 refs=1
b = a      data=0x14b606b00 nbytes=64 refs=2
b 离开作用域后                        refs=1
```

地址每次运行都不一样，但**两个地址必须一致**。

## 卡住了

- 两个地址不同 → 你复制了字节而不是共享所有权。
- refs 一直是 1 → 你可能存的是裸指针，或者 `shared_ptr` 被 move 而不是 copy。
