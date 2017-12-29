## 空间配置器

### 标准空间配置器
以 STL的实作角度而言,第一个需要介绍的就是空间配置器,因为整个 STL 的操作对象(所有的数值)都存放在容器之内,而容器一定需要配置空间以置放数据。根据 STL 的规范,以下是 allocator 的必要界面：

* allocator::allocator()

     default constructor。
     
* allocator::allocator(const allocator&)

    copy constructor。
    
* template &lt;class U&gt;allocator::allocator(const allocator&lt;U&gt;&)

    泛化的 copy constructor。

* allocator::~allocator()

    default constructor。
    
* pointer allocator::address(reference x) const

    传回某个对象的地址。算式 a.address(x) 等同于 &x 。
    
* const_pointer allocator::address(const_reference x) const

    传回某个 const 对象的地址。算式 a.address(x) 等同于 &x 。
    
* pointer allocator::allocate(size_type n, cosnt void* = 0)

    配置空间,足以储存 n 个 T 对象。第二自变量是个提示。实作上可能会利用它来增进区域性(locality),或完全忽略之。
    
* void allocator::deallocate(pointer p, size_type n)

    归还先前配置的空间。
    
* size_type allocator::max_size() const

    传回可成功配置的最大量。
    
* void allocator::construct(pointer p, const T& x)

    等同于 new(const void*) p) T(x) 。
    
* void allocator::destroy(pointer p)

    等同于 p->~T() 。

### SGI STL空间配置器
SGI STL的配置器与众不同,也与标准规范不同,其名称是alloc而非allocator ,而且不接受任何自变量。但是这个事实通常不会对我们带来困扰,因为通常我们使用预设的空间配置器,很少需要自行指定配置器名称,而 SGI STL 的每一个容器都已经指定其预设的空间配置器为 alloc 。例如下面的 vector 宣告:

template &lt;class T, class Alloc = alloc&gt; // 预设使用 alloc 为配置器

class vector { ... };

虽然 SGI 也定义有一个符合部份标准、名为 allocator 的配置器,但 SGI 自己从未用过它,也不建议我们使用，主要原因是效率不佳。

为了精密分工,STL allocator 决定将内存分配与建构解构对象动作区分开来。内存配置动作由 alloc:allocate() 负责,内存释放动作由 alloc::deallocate() 负责;对象建构动作由 ::construct() 负责,对象解构动作由 ::destroy() 负责。在此仅列出构造与析构的基本工具源码的分析

#include <new.h>

template &lt;class T1, class T2&gt;

inline void construct(T1* p, const T2& value) {

new (p) T1(value);

// placement new ; 唤起 T1::T1(value);

}
// 以下是 destroy() 第一版本,接受一个指标。

template &lt;class T&gt;

inline void destroy(T* pointer) {

pointer->~T();

// 唤起 dtor ~T()

}

// 以下是 destroy() 第二版本,接受两个迭代器。此函式设法找出元素的数值型别,进而利用 __type_traits求取最适当措施。

template &lt;class ForwardIterator&gt;

inline void destroy(ForwardIterator first, ForwardIterator last) {

__destroy(first, last, value_type(first));

}

// 判断元素的数值型别( value type )是否有 trivial destructor

template &lt;class ForwardIterator, class T&gt;

inline void __destroy(ForwardIterator first, ForwardIterator last, T*)

{

typedef typename __type_traits&lt;T&gt;::has_trivial_destructor trivial_destructor;

__destroy_aux(first, last, trivial_destructor());

}

// 如果元素的数值型别( value type )有 non-trivial destructor...

template &lt;class ForwardIterator&gt;

inline void

__destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {

for ( ; first < last; ++first)

destroy(&*first);

}

// 如果元素的数值型别( value type )有 trivial destructor...

template &lt;class ForwardIterator&gt;

inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

可见destroy() 有两个版本,第一版本接受一个指标,准备将该指标所指之物解构掉。直接呼叫该对象的解构式即可。第二版本接受first 和 last 两个迭代器,准备将 [first,last) 范围内的所有物件解构掉。我们不知道这个范围有多大,万一很大,而每个物件的解构式都无关痛痒(所谓 trivial destructor),那么一次次呼叫这些无关痛痒的解构式会降低效率。因此,这里首先利用 value_type() 获得迭代器所指物件的型别,再利用 __type_traits判别该型别的解构式是否无关痛痒。若是( __true_type ),什么也不做就结束;若否( __false_type ),这才以循环方式巡访整个范围,并在循环中每经历一个对象就呼叫第一个版本的 destroy()。

关于内存的分配与释放，SGI以malloc()和free()完成。考虑小型区块所可能造成的内存破碎问题,SGI 设计了双层级配置器,第一级
配置器直接使用 malloc() 和 free() ,第二级配置器则视情况采用不同的策略:当配置区块超过 128bytes,视之为「足够大」,便呼叫第一级配置器;当配置区块小于128bytes,视之为「过小」,为了降低额外负担便采用复杂的 memory pool 整理方式,而不再求助于第一级配置器。

SGI 第一级配置器的 allocate() 和 realloc() 都是在呼叫 malloc()和realloc()不成功后,改呼叫oom_malloc() 和 oom_realloc() 。后两者都有内循环,不断呼叫「内存不足处理例程」,期望在某次呼叫之后,获得足够的内存而圆满达成任务。但如果「内存不足处理例程」并未被客端设定,oom_malloc() 和 oom_realloc() 便呼叫 __THROW_BAD_ALLOC ,丢出 bad_alloc 异常讯息,或中止程序。

SGI 第二级配置器中，当区块小于 128 bytes,则以记忆池(memory pool)管理:每次配置一大块内存,并维护对应之自由串行(free-
list)。下次若再有相同大小的内存需求,就直接从 free-lists 中拨出。如果客端释还小额区块,就由配置器回收到 free-lists 中 为了方便管理,SGI 第二级配置器会主动将任何小额区块的记忆体需求量上调至8的倍数 (例如客端要求 30 bytes , 就自动 调整 为 32bytes),并维护 16个free-lists,各自管理大小分别为 8, 16, 24, 32, 40, 48, 56, 64, 72,80, 88, 96, 104, 112, 120, 128 bytes 的小额区块。

###内存基本处理工具

STL定义有五个全域函数,作用于未初始化空间。除了用于建构的construct()和用于解构的destroy(),另三个函数是
uninitialized_copy(),uninitialized_fill(),uninitialized_fill_n()。

* uninitialized_copy(InputIterator first, InputIterator last,ForwardIterator result);

    uninitialized_copy()使我们能够将记忆体的配置与物件的建构行为分离开来。如果做为输出目的地的 [result, result+(last-first)) 范围内的每一个迭代器都指向未初始化区域,则 uninitialized_copy() 会 使 用 copy constructor,为身为输入来源之 [first,last) 范围内的每一个对象产生一份复制品,放进输出范围中。
    
* void uninitialized_fill(ForwardIterator first, ForwardIterator last,const T& x);

    如果 [first,last) 范 围内的每个迭代器都指向未初始化的内存 ,那么uninitialized_fill() 会在该范围内产生x(上式第三参数)的复制品。

* uninitialized_fill_n(ForwardIterator first, Size n, const T& x);

    如果 [first, first+n) 范围内的每一个迭代器都指向未初始化的内存,那么uninitialized_fill_n() 会呼叫 copy constructor,在该范围内产生 x (上式第三参数)的复制品。
    
    
## 迭代器

迭代器(iterators)是一种抽象的设计概念,现实程序语言中并没有直接对映于这个概念的实物。它提供一种方法,使得能够依序巡访某个聚合物(容器)所含的各个元素,而又无需暴露该聚合物的内部表述方式。STL 的中心思想在于,将数据容器(containers)和算法(algorithms)分开,彼此独立设计，最后用迭代器加以撮合。

迭代器是一种行为类似指针的对象,而指针的各种行为中最常见也最重要的便是内容提领(dereference)和成员取用(member access),因此迭代器最重要的编程工作就是对 operator* 和 operator-> 进行多载化(overloading)工程。

### 迭代器相应类型part1
算法之中运用迭代器时,很可能会用到其相应类型(associated type)，如迭代器所指之物的类型。假设算法中有必要声明一个参数, 以 「迭代器所指物件的类型」为类型,需要用到利用 function template 的自变量推导机制。例如：

template &lt;class I, class T&gt;

void func_impl(I iter, T t)

{

T tmp; // 这里解决了问题。T 就是迭代器所指之物的类型

// ... 这里做原本 func() 应该做的全部工作

};

template &lt;class I&gt;

inline

void func(I iter)

{

func_impl(iter, *iter); // func 的工作全部移往 func_impl

}

### Traits

迭代器所指物件的类型,称为该迭代器的 value type。上述的自变量型别推导技巧虽然可用于 value type,却非全面可用:万一 value type 必须用于函数的返回值,就束手无策了,毕竟函式的「template 自变量推导机制」推而导之的只是自变量,无法推导函数的返回值类型。

使用template partial specialization解决这个问题。Partial Specialization(偏特化)指：如果 class template 拥有一个以上的 template参数,我们可以针对其中某个(或数个,但非全部)template 参数进行特化工作。例如：

template&lt;typename T&gt;

class C { ... };

// 这个泛化版本允许(接受)T 为任何类型

我们便很容易接受它有一个型式如下的 partial specialization:

template&lt;typename T&rt;

class C&lt;T*&rt; { ... }; // 这个特化版本仅适用于「T 为原生指针」的情况

// 「T 为原生指针」便是「T 为任何类型」的一个更进一步的条件限制

如此我们便可以萃取迭代器的特性。，例如下面这个 class template 专门用来「萃取」迭代器的特性,而 value type 正是迭代器的特性之一:

template &lt;class I&gt;

struct iterator_traits {

typedef typename I::value_type

};


traits 可以拥有特化版本。现在,我们令 iterator_traites 拥有一个 partial specializations 如下

template &lt;class T&gt;

struct iterator_traits&lt;T*&gt; {

typedef T value_type;

};
// 偏特化版 — 迭代器是个原生指针

于是,指针int* 虽然不是一种 class type, 亦可透过traits取其valuetype。这就解决了先前的问题。


### 迭代器相应类型part2

最常用到的迭代器相应类型有五种:value type, difference type, pointer,reference, iterator catagoly。

* value type ：迭代器所指对象的类型
* difference type：用来表示两个迭代器之间的距离
* reference type：传回一个左值,令它代表 p 所指之物
* pointer type：传回一个左值,令它代表 p 所指之物的地址
* iterator_category：表示迭代器的五种类型，在“STL简介”中已经介绍




    