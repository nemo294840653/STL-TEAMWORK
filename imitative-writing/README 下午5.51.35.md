# 仿写STL

## vector

| 函数名称         | 作用                            |
| ------------ | :---------------------------- |
| empty()      | 判断容器是否为空                      |
| size()       | 返回容器的元素个数                     |
| capacity()   | 返回容器的当前内存大小                   |
| begin()      | 返回首部迭代器                       |
| end()        | 返回尾部迭代器                       |
| operator[]() | 重载[]迭代器，返回第i的元素的值             |
| push_back    | 在vector的最后添加一个数据              |
| pop_back()   | 去掉数组的最后一个数据                   |
| resize       | 改变当前使用数据的大小，如果它比当前使用的大，则填充默认值 |
| reserve      | 改变当前vecotr所分配空间的大小            |
| clear        | 清空当前的vector                   |
| insert       | 向vector中插入元素                  |







/***************************
0:tiny_vector();
tiny_vector(size_t n, T  obj);
tiny_vector(iterator begin, iterator end);
~tiny_vector();
tiny_vector(const tiny_vector& obj);
构造函数和析构函数
1:tiny_vector& operator=(const tiny_vector& obj);
重载等号操作符
2:bool empty() const { return begin() == end(); }

3:size_type size() const { return (size_type)(finish - start); }

4:size_type capacity() const { return (size_type)(end_of_storage - start); }

5:iterator begin() { return start; }
const_iterator begin() const { return start; }

6:iterator end() { return finish; }
const_iterator  const { return finish; }

7:reference operator[](size_type i) { return *(start + i); }
const_reference operator[](size_type i)const { return *(start + i); }

8:void (const T& value);
插入元素
9:void insert(iterator position, size_type n, const T& value);
在迭代器之前插入n个值为value的元素
10:void pop_back();
从尾部弹出元素
11:void erase(iterator first, iterator last);
删除两个迭代器之间的元素
12:void clear();
删除容器的所有元素
13:void reserve(size_type n);
重新分配内存空间，若n小于当前空间大小则不作改变，反之重新分配更大空间并将当前数据拷贝至新内存
14:void resize(size_type n, value_type &value);
重新分配元素的长度，若n大于当前容器的元素个数，需要将不足部分用value进行补充，反之则去除多余部分
****************************/