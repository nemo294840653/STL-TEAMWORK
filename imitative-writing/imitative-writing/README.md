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

## list

| 函数名称       | 作用                                      |
| ---------- | --------------------------------------- |
| find       | 查找以结点p结尾的倒数前n个结点中是否有元素e                 |
| empty      | 判断是否为空表                                 |
| valid      | 判断结点是否有效                                |
| insertsort | 插入排序                                    |
| selectsort | 选择排序                                    |
| selectmax  | 选择list对象所有结点的最大值                        |
| reverse    | 使list中的元素翻转                             |
| sort       | 排序，插入排序和选择排序随机                          |
| uniquify   | 使用时列表必须是已排序序列，使list中的元素唯一化,返回值代表删除多少个结点 |
| swap       | 交换两个list                                |
| clear      | 删除所有元素                                  |
| remove()   | 从list删除元素                               |

