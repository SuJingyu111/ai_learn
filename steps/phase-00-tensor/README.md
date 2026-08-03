# Phase 00 — Tensor

结束时你会有：一个能打印任意张量的 shape、strides、dtype 和真实地址的程序，
并且每一个 offset 推导都有断言保护。

这一整个 phase 不涉及任何数学运算。它建立的是后面所有东西的地基：
**逻辑上的张量如何映射到物理上的字节**。绝大多数 runtime bug 最后都能追到这里。

不需要 GPU。不需要 Orin。一台能编译 C++17 的机器就够。

从 [step-001](step-001.md) 开始。
