# 序列式容器源码分析

**序列式容器（Sequence containers**），每个元素都有固定位置－－取决于插入时机和地点，和元素值无关，vector、deque、list；

   **Vectors：**将元素置于一个动态数组中加以管理，可以随机存取元素（用索引直接存取），数组尾部添加或移除元素非常快速。但是在中部或头部安插元素比较费时；

   **Deques**：是“double-ended queue”的缩写，可以随机存取元素（用索引直接存取），数组头部和尾部添加或移除元素都非常快速。但是在中部或头部安插元素比较费时；

  **Lists：**双向链表，不提供随机存取（按顺序走到需存取的元素，O(n)），在任何位置上执行插入或删除动作都非常迅速，内部只需调整一下指针；

## 模板

下面将逐一介绍的三种主要的序列式容器的源码实现将涉及到C++模板，模板是C++支持**参数化**多态的工具，使用模板可以使用户为类或者函数声明一种一般模式，使得类中的某些数据成员或者成员函数的参数、返回值取得任意类型。

类模板的格式为：

```
template<class  形参名，class 形参名，…>   class 类名

　　　　{ ... };
```

　　类模板和函数模板都是以**template**开始后接模板形参列表组成，模板形参不能为空，**一但声明了类模板就可以用类模板的形参名声明类中的成员变量和成员函数，即可以在类中使用内置类型的地方都可以使用模板形参名来声明。**比如

　　　　**template<class T> class A{public: T a; T b; T hy(T c, T &d);};**

在类**A**中声明了两个类型为**T**的成员变量**a**和**b**，还声明了一个返回类型为**T**带两个参数类型为**T**的函数**hy**。

## Vector

- 定义摘要

```
// alloc 是 SGI STL 的空间配置器
template <class T, class Alloc = alloc> class vector {
public:
// vector 的嵌套型别定义
typedef T value_type; typedef value_type* pointer;
typedef value_type* iterator;
typedef value_type& reference;
typedef size_t size_type;
typedef ptrdiff_t difference_type;
protected:
// 以下，simple_alloc 是 SGI STL 的空间配置器
typedef simple_alloc<value_type, Alloc> data_allocator;
iterator start ; // 表示目前使用空间的头 iterator finish ; // 表示目前使用空间的尾 iterator end_of_storage; // 表示目前可用空间的尾
......

```

- 常见函数的实现代码

  - ```
    iterator begin() { return start; }
    ```

  - ```
    iterator end() { return finish; }
    ```

  - ```
    size_type size() const { return size_type(end() - begin()); }
    ```

  - ```
    size_type capacity() const {
    return size_type(end_of_storage - begin()); }
    ```

  - ```
    bool empty() const { return begin() == end(); }
    ```

  - ```
    reference operator[] (size_type n) { return *(begin() + n); }
    ```

  - ```
    reference front() { return *begin(); }
    // 返回第一个元素
    ```

  - ```
    void push_back (const T& x) {//将元素插入最尾端
    if (finish != end_of_storage) { 
    	construct (finish, x); //全局函数
    	++finish;
    } 
    else
    	insert_aux (end(), x); //这是 vector 的一个 member function
    }
    ```

  - ```
    void pop_back() {//将最尾端元素取出
    	--finish;
        destroy(finish);//全局函数
     }
    ```

  - ```
    iteratorerase(iteratorposition){ // 清除某位置上的元素
    	if (position + 1 != end())
    		copy(position + 1, finish, position); // 后续元素往前搬移
    	--finish;
    	destroy(finish);// 全域函数
    	return position; 
    }
    ```

  - ```
    void resize(size_type new_size, const T& x) { 
    	if (new_size < size())
    		erase(begin() + new_size, end());
        else
    		insert(end(), new_size - size(), x); 
    }
    ```

## List

- 定义摘要

```
template <class T> //list 的节点 ( node )定义
struct __list_node {
	typedef void* void_pointer; 
	void_pointer prev ; // 型别为void*。其实可设为__list_node<T>*
	void_pointer next;
	T data;
};

template <class T, class Alloc = alloc> // 预设使用 alloc 为配置器 class list {
	protected:
	typedef __list_node<T> list_node; public:
	typedef list_node* link_type;
	protected:
	link_type node; // 只要一个指标，便可表示整个环状双向串行
	... 
};

```

- 常见函数的实现代码

  - ```
    iterator begin() { return (link_type)((*node).next); }
    ```

  - ```
    iterator end() { return node; }
    ```

  - ```
    bool empty() const { return node->next == node; }
    ```

  - ```
    size_type size() const {
    	size_type result = 0; 
    	distance (begin(), end(), result); //全域函数
    	return result;
    }
    ```

  - ```
    reference front() { return *begin(); } // 取头节点的内容(元素值)
    ```

  - ```
    reference back() { return *(--end()); }// 取尾节点的内容(元素值)
    ```

  - ```
    void push_front(const T& x) { insert(begin(), x); }// 安插一个节点，做为头节点	
    ```

  - ```
    void push_back(const T& x) { insert(end(), x); }// 安插一个节点，做为尾节点
    ```

  - ```
    iterator erase(iterator position) {// 移除迭代器 position 所指节点
    	link_type next_node = link_type(position.node->next); link_type prev_node = 	link_type(position.node->prev); prev_node->next = next_node;
    	next_node->prev = prev_node;
    	destroy_node (position.node);
    	return iterator(next_node);
    }
    ```

  - ```
    void pop_front() { erase(begin()); }// 移除头节点
    ```

  - ```
    void pop_back(){// 移除尾节点
    	iterator tmp = end();
    	erase(--tmp);
    }
    ```

## Deque

- 定义摘要

```
template <class T, class Alloc = alloc , size_t BufSiz = 0> 
class deque {
public://基本类型
	typedef T value_type; 
	typedef value_type* pointer; 
	...
protected:
	// 元素的指针的指针(pointer of pointer of T)
	typedef pointer* map_pointer;
protected://数据成员
	map_pointer map;// 指向 map ，map 是块连续空间，其内的每个元素
					// 都是一个指标(称为节点)，指向一块缓冲区。
	size_type map_size; // map 内可容纳多少指标。 ...
};
```

- 常见函数实现代码

  - ```
    iterator begin() { return start; }	
    ```

  - ```
    iterator end() { return finish; }
    ```

  - ```
    reference operator[](size_type n) {
    	return start[difference_type(n)]; // 唤起 __deque_iterator<>::operator[]
    }
    ```

  - ```
    reference front() { return *start; } // 唤起 __deque_iterator<>::operator*
    ```

  - ```
    reference back() {
    	iterator tmp = finish;
    	--tmp; // 唤起 __deque_iterator<>::operator--
    	return *tmp; // 唤起 __deque_iterator<>::operator*
    }
    ```

  - ```
    size_type size() const { return finish - start; }
    ```

  - ```
    size_type max_size() const { return size_type(-1); }
    ```

  - ```
    void pop_back() {
    	if (finish.cur != finish.first) {
    	// 最后缓冲区有一个(或更多)元素
    		--finish.cur; 
    		// 调整指针，相当于排除了最后元素 destroy(finish.cur); // 将最后元素解构
    	} 
    	else
    	// 最后缓冲区没有任何元素
    	pop_back_aux (); // 这里将进行缓冲区的释放工作 
    }
    ```

  - ```
    void deque<T, Alloc, BufSize>::pop_back_aux() {
    	deallocate_node(finish.first); // 释放最后一个缓冲区 
    	finish.set_node(finish.node - 1); // 调整 finish 的状态，使指向
     	finish.cur = finish.last - 1; // 上一个缓冲区的最后一个元素
     	destroy(finish.cur);// 将该元素解构
     }
    ```

  - ```
    void pop_front() {
    	if (start.cur != start.last - 1) {// 第一缓冲区有一个(或更多)元素
    		destroy (start.cur);// 将第一元素解构
    		++start.cur;// 调整指针，相当于排除了第 一元素
    	}
    	else// 第一缓冲区仅有 一个元素
    		pop_front_aux ();
    }

    ```

  - ```
    void deque<T, Alloc, BufSize>::pop_front_aux() {
    	destroy(start.cur); // 将第一缓冲区的第一个元素解构
    	deallocate_node (start.first); // 释放第一缓冲区
    	start.set_node (start.node + 1); // 调整 start 的状态，使指向下一个缓冲区的第一个元素
    	start.cur = start.first;
    }
    ```

    ​