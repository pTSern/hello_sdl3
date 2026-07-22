# C++ Memory Lifetimes, Copying, and Moving Reference

This document serves as a complete reference for C++ memory allocation, stack frame reuse, copy/move mechanics, and compiler optimizations (NRVO).

---

## 1. Object Instantiation: Stack vs. Heap

In C++, how you declare a variable determines where it lives in memory and how its lifetime is managed.

### Stack Allocation (Idiomatic & Safe)
```cpp
Game game;
```
* **What it does**: Allocates the object on the stack and automatically calls its default constructor `Game::Game()`.
* **Lifetime**: Tied to the scope (curly braces `{}`) where it was declared. It is automatically destroyed when it goes out of scope.
* **Avoid**: `Game game();` which is interpreted as a function declaration (Most Vexing Parse).

### Heap Allocation (Requires Manual Management)
```cpp
auto game = new Game(); // game is Game* (pointer)
game->initialize();
game->run();
delete game;            // MUST manually delete to avoid memory leaks
```
* **What it does**: Allocates memory on the heap.
* **Lifetime**: Persists until `delete` is explicitly called. If you lose the pointer before deleting, it causes a **memory leak**.

---

## 2. Under the Hood: Copy vs. Move

When an object is returned by value or passed to another variable, C++ performs either a **Copy** or a **Move** (if not optimized away).

### A. Copying (`Copy Constructor`)
Duplicates the data entirely.
```cpp
class _test {
    int x;
    int* ptr; // heap resource
public:
    _test(const _test& other) {
        this->x = other.x;
        // Deep copy: allocate new heap memory and duplicate content
        this->ptr = new int(*other.ptr); 
    }
};
```
* **Heap Memory**: Allocates new heap memory.
* **Original Object**: Unchanged.
* **Speed**: **Slow** (depends on the size of the resources being copied).

### B. Moving (`Move Constructor` - C++11+)
Transfers ownership of resources (like heap pointers) from the old object to the new object.
```cpp
class _test {
    int x;
    int* ptr; // heap resource
public:
    _test(_test&& other) noexcept {
        this->x = other.x;
        this->ptr = other.ptr;   // Steal the heap pointer address!
        other.ptr = nullptr;     // Nullify the old pointer so it doesn't free the memory
    }
};
```
* **Heap Memory**: Reuses existing heap memory (no allocations).
* **Original Object**: Left in a valid but "empty" state (pointers set to `nullptr`).
* **Speed**: **Extremely Fast (O(1))** (instantaneous pointer copy).

---

## 3. Returning by Value: NRVO vs. Move vs. Copy

When you call a function that returns an object by value:
```cpp
_test test() {
    _test _;
    return _;
}
```

The compiler determines how to return the object using this priority list:

1. **Named Return Value Optimization (NRVO)**: By default, the compiler constructs `_` directly inside the caller's target variable. **No copy and no move constructors are called.**
2. **Move Constructor**: If NRVO is prevented/disabled, the compiler automatically casts the local variable to an *rvalue* (temporary) and calls the **Move Constructor** to transfer the resources.
3. **Copy Constructor**: If moving is not supported by the class or compiler, it falls back to the **Copy Constructor**, duplicating the data.

---

## 4. Stack Frame Reuse (Why Local Addresses Match)

If you call `test()` multiple times sequentially and print the address of the local variable inside `test()`, you will see the **exact same address** every time (e.g. `0xA`):
1. `test()` is called -> A stack frame is created -> Local variable is placed at address `0xA`.
2. `test()` returns -> The stack frame is destroyed/popped.
3. `test()` is called again -> The CPU reuses the exact same stack space -> Local variable is placed at address `0xA` again.

However, if you store the returned values in the caller:
```cpp
auto z1 = test(); // Address 0xA1
auto z2 = test(); // Address 0xA2
auto z3 = test(); // Address 0xB1
```
These must have **different addresses** because they exist simultaneously in the caller's stack frame.

---

## 5. Learning Experiment: Code Verification

Below is a complete, runnable C++ program that lets you observe the behavior of **NRVO**, **Move**, and **Copy** constructors, and see how they affect a global counter.

```cpp
#include <iostream>

int __i = 0; // Global counter tracking construction events

class _test {
public:
    // 1. Default Constructor
    _test() {
        __i++;
        std::cout << "[Default Constructor] Object created. Global Counter __i: " << __i << std::endl;
    }

    // 2. Copy Constructor
    _test(const _test& other) {
        __i++;
        std::cout << "[Copy Constructor] Object copied. Global Counter __i: " << __i << std::endl;
    }

    // 3. Move Constructor
    _test(_test&& other) noexcept {
        __i++;
        std::cout << "[Move Constructor] Object moved. Global Counter __i: " << __i << std::endl;
    }

    // Destructor
    ~_test() {
        std::cout << "[Destructor] Object destroyed." << std::endl;
    }
};

_test test() {
    _test _;
    std::cout << "Local object address inside test(): " << &_ << std::endl;
    return _;
}

int main() {
    std::cout << "--- Starting Call 1 ---" << std::endl;
    auto z1 = test();
    std::cout << "Caller variable address: " << &z1 << std::endl;

    std::cout << "\n--- Starting Call 2 ---" << std::endl;
    auto z2 = test();
    std::cout << "Caller variable address: " << &z2 << std::endl;

    std::cout << "\n--- Starting Call 3 ---" << std::endl;
    auto z3 = test();
    std::cout << "Caller variable address: " << &z3 << std::endl;

    std::cout << "\nFinal value of __i: " << __i << std::endl;
    std::cout << "--- Exiting main (Destruction phase) ---" << std::endl;
    return 0;
}
```

### What to Look For:
1. **With NRVO (Optimizations enabled):**
   * The local object address inside `test()` will match the caller variable address exactly.
   * Only the **Default Constructor** will be called (3 times total).
   * `__i` will equal `3`.
2. **Without NRVO (Optimizations disabled, e.g., compiled with `-fno-elide-constructors`):**
   * The local object address inside `test()` will be different from the caller variable address.
   * You will see both `Default Constructor` and `Move Constructor` called.
   * `__i` will equal `6` (3 default constructions + 3 moves).

---

## 6. Uninitialized Member Variables (Garbage Values)

In C++, primitive types (like `int`, `float`, `bool`, and raw pointers) are **not zero-initialized** by default when created on the stack or heap.

### Why does it print random numbers?
C++ operates on the philosophy of **"you don't pay for what you don't use."** Zeroing out memory takes CPU cycles. Therefore, if you don't initialize a variable, the compiler simply allocates the memory and leaves whatever data happened to be sitting in that memory slot from previous operations. This data is known as **garbage values**.

> [!WARNING]
> Reading an uninitialized variable is **Undefined Behavior (UB)**. The program might print a random number, crash, or behave unpredictably depending on the compiler and platform.

### How to fix it:

#### Option 1: In-class Default Initializers (Recommended - C++11+)
Directly assign a default value when declaring the variable:
```cpp
class _test {
public:
    int bro = 0; // Automatically initialized to 0
};
```

#### Option 2: Constructor Initializer List
Initialize it in the constructor:
```cpp
class _test {
public:
    int bro;
    _test() : bro(0) { // Initializer list
    }
};
```

