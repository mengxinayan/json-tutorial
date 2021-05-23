## Tutorial 01

### CMake 的使用

创建 `/build` 目录，所有的生成文件都会在该目录下，删除时也方便删除。

```shell
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
```

可以将上面的 Debug 替换为 Release，这样生成的就会是 Release 版本的代码。

### 头文件

由于头文件也可以 #include 其他头文件，为避免重复声明，通常会利用宏加入 include 防范（include guard）：

```c
#ifndef LEPTJSON_H__
#define LEPTJSON_H__

/* ... */

#endif /* LEPTJSON_H__ */
```

宏的名字必须是唯一的，通常习惯以 `_H__` 作为后缀。由于 leptjson 只有一个头文件，可以简单命名为 `LEPTJSON_H__`。如果项目有多个文件或目录结构，可以用 `项目名称_目录_文件名称_H__` 这种命名方式。

因为 C 语言没有 C++ 的命名空间（namespace）功能，一般会使用项目的简写作为标识符的前缀。通常枚举值用全大写（如 `LEPT_NULL`），而类型及函数则用小写（如 `lept_type`）。

### 单元测试

一般我们会采用自动的测试方式，例如单元测试（unit testing）。单元测试也能确保其他人修改代码后，原来的功能维持正确（这称为回归测试／regression testing）。

常用的单元测试框架有 xUnit 系列，如 C++ 的 Google Test、C# 的 NUnit。我们为了简单起见，会编写一个极简单的单元测试方式。

一般来说，软件开发是以周期进行的。例如，加入一个功能，再写关于该功能的单元测试。但也有另一种软件开发方法论，称为测试驱动开发（test-driven development, TDD），它的主要循环步骤是：

    加入一个测试。
    运行所有测试，新的测试应该会失败。
    编写实现代码。
    运行所有测试，若有测试失败回到3。
    重构代码。
    回到 1。

TDD 是先写测试，再实现功能。好处是实现只会刚好满足测试，而不会写了一些不需要的代码，或是没有被测试的代码。

但无论我们是采用 TDD，或是先实现后测试，都应尽量加入足够覆盖率的单元测试。

然而，完全按照 TDD 的步骤来开发，是会减慢开发进程。所以我个人会在这两种极端的工作方式取平衡。通常会在设计 API 后，先写部分测试代码，再写满足那些测试的实现。

### 宏定义编写技巧

有些同学可能不了解 EXPECT_EQ_BASE 宏的编写技巧，简单说明一下。反斜线代表该行未结束，会串接下一行。而如果宏里有多过一个语句（statement），就需要用 do { /*...*/ } while(0) 包裹成单个语句。

### 断言（assertion）

断言（assertion）是 C 语言中常用的防御式编程方式，减少编程错误。最常用的是在函数开始的地方，检测所有参数。有时候也可以在调用函数后，检查上下文是否正确。

C 语言的标准库含有 `assert()` 这个宏（需 `#include <assert.h>`），提供断言功能。当程序以 release 配置编译时（定义了 `NDEBUG` 宏），`assert()` 不会做检测；而当在 debug 配置时（没定义 `NDEBUG` 宏），则会在运行时检测 `assert(cond)` 中的条件是否为真（非 0），断言失败会直接令程序崩溃。

例如上面的 `lept_parse_null()` 开始时，当前字符应该是 'n'，所以我们使用一个宏 `EXPECT(c, ch)` 进行断言，并跳到下一字符。

初使用断言的同学，可能会错误地把含副作用的代码放在 `assert()` 中：

```c
assert(x++ == 0); /* 这是错误的! */
```

这样会导致 debug 和 release 版的行为不一样。

另一个问题是，初学者可能会难于分辨何时使用断言，何时处理运行时错误（如返回错误值或在 C++ 中抛出异常）。**简单的答案是，如果那个错误是由于程序员错误编码所造成的（例如传入不合法的参数），那么应用断言；如果那个错误是程序员无法避免，而是由运行时的环境所造成的，就要处理运行时错误（例如开启文件失败）。**

### 为什么使用宏而不使用函数或内联函数

因为这个测试框架使用了 `__LINE__` 这个编译器提供的宏，代表编译时该行的行号。如果用函数或内联函数，每次的行号便都会相同。另外，内联函数是 C99 的新增功能，本教程使用 C89。


## Tutorial 02

### 重构

在 TDD 的过程中，我们的目标是编写代码去通过测试。但由于这个目标的引导性太强，我们可能会忽略正确性以外的软件品质。在通过测试之后，代码的正确性得以保证，我们就应该审视现时的代码，看看有没有地方可以改进，而同时能维持测试顺利通过。我们可以安心地做各种修改，因为我们有单元测试，可以判断代码在修改后是否影响原来的行为。

### 注释代码，使用 #if 0 ... #endif

因为在做第 1 个练习题时，我希望能 100% 通过测试，方便做重构。另外，使用 `#if 0 ... #endif` 而不使用 `/* ... */`，是因为 C 的注释不支持嵌套（nested），而 `#if ... #endif` 是支持嵌套的。代码中已有注释时，用 #`if 0 ... #endif` 去禁用代码是一个常用技巧，而且可以把 `0` 改为 `1` 去恢复。

### 习题回顾

#### 1. 重构合并

- 可以传入要比较的字符串与其类型，这样就方便进行赋值
- 不会改变的值，应该设置为 `const`
- 注意在 C 语言中，数组长度、索引值最好使用 `size_t` 类型，而不是 `int` 或 `unsigned`。

#### 2. 数字解析

把它分为四个部分（负号、整数、小数、指数），分别进行判断。

#### 3. 数字过大的处理

```
1. errno == ERANGE
2. v->n == HUGE_VAL
```

两种方法都可以进行判断

## Tutorial 03

### 注释的写法

`/* \TODO */`

### 结构体设计

然而我们知道，一个值不可能同时为数字和字符串，因此我们可使用 C 语言的 union 来节省内存：

```c
typedef struct {
    union {
        struct { char* s; size_t len; }s;  /* string */
        double n;                          /* number */
    }u;
    lept_type type;
}lept_value;
```

### 动态堆栈

```c
typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}lept_context;
```

当中 size 是当前的堆栈容量，top 是栈顶的位置（由于我们会扩展 stack，所以不要把 top 用指针形式存储）。

然后，我们在创建 lept_context 的时候初始化 stack 并最终释放内存。在释放时，加入了断言确保所有数据都被弹出。

然后，我们实现堆栈的压入及弹出操作。和普通的堆栈不一样，我们这个堆栈是以字节储存的。每次可要求压入任意大小的数据，它会返回数据起始的指针：

```c
#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

static void* lept_context_push(lept_context* c, size_t size) {
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = LEPT_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void* lept_context_pop(lept_context* c, size_t size) {
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}
```

注意到这里使用了 `realloc()` 来重新分配内存，`c->stack` 在初始化时为 `NULL`，`realloc(NULL, size)` 的行为是等价于 `malloc(size)` 的，所以我们不需要为第一次分配内存作特别处理。

另外，我们把初始大小以宏 `LEPT_PARSE_STACK_INIT_SIZE` 的形式定义，使用 `#ifndef X #define X ... #endif` 方式的好处是，使用者可在编译选项中自行设置宏，没设置的话就用缺省值。

### Linux/OSX 下的内存泄漏检测方法（valgrind）

可以发现内存泄漏的地方、未初始化的变量，使用下面的命令：

```shell
$ valgrind --leak-check=full  ./leptjson_test
```

编写单元测试时，应考虑哪些执行次序会有机会出错，例如内存相关的错误。然后我们可以利用 TDD 的步骤，先令测试失败（以内存工具检测），修正代码，再确认测试是否成功。

### PUTC 函数

```c
#define PUTC(c, ch)         do { *(char*)lept_context_push(c, sizeof(char)) = (ch); } while(0)

static void* lept_context_push(lept_context* c, size_t size) {
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = LEPT_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}
```

### 习题回顾

1. Must Break at case ch=='\', otherwise will go in default.
2. 错误时让 `c->top = head;`，相当于让栈回到最开始的地方
3. 范围的确定，需要看说明

## Tutorial 04

## Tutorial 05

## Tutorial 06

## Tutorial 07

## Tutorial 08
