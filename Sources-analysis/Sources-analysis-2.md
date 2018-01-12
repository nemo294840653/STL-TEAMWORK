# 序列式容器
所谓序列式容器,其中的元素都可序,。C++ 语言本身提供了一个序列式容器array ,STL 另外再提供 vector , list , deque ,
stack , queue , priority-queue 等等序列式容器。其中 stack 和 queue 只是将 deque 改头换面而成。

## vector
vector 的数据安排以及操作方式,与 array 非常像似。两者的唯一差别在于空间的运用弹性。 array 是静态空间,一旦配置了就不能改变;vector 是动态空间,随着元素的加入,它的内部机制会自行扩充空间以容纳新元素。vector 的实作技术,关键在于其对大小的控制以及重新配置时的数据搬移效率。

### vector的迭代器
vector 维护的是一个连续线性空间,所以不论其元素类型为何,指针都可以做为 vector 的迭代器而满足所有必要条件,因为 vector 迭代器所需要的操作行为如 operator*,operator->,operator++,operator--,operator+, operator-,operator+=,operator-= ,指针天生就具备。 vector支持随机存取,而指针正有着这样的能力。所以,vector 提供的是 Random Access Iterators。
```
template <class T, class Alloc = alloc>
class vector {
public:
typedef T   value_type;
typedef value_type* iterator;
// vector 的迭代器是指针
...
};
```
如果写出以下代码：
```
vector<int>::iterator ivite;
vector<Shape>::iterator svite;
```
ivite 的类型其实就是 int* , svite 的类型其实就是 Shape* 。

### vector的数据结构
vector所采用的数据结构非常简单:线性连续空间。它以两个迭代器 start 和finish分别指向配置得来的连续空间中目前已被使用的范围,并以迭代器end_of_storage 指向整块连续空间(含备用空间)的尾端:
```
template <class T, class Alloc = alloc>
class vector {
...
protected:
iterator start;
// 表示目前使用空间的头
iterator finish;
// 表示目前使用空间的尾
iterator end_of_storage; // 表示目前可用空间的尾
...
};
```
为了降低空间配置时的速度成本, vector 实际配置的大小可能比需求量更大一些,以备将来可能的扩充。这便是容量概念。换句话说一个 vector的容量永远大于或等于其大小。一旦容量等于大小,便是满载,下次再有新增元素,整个 vector就得另觅居所。运用 start, finish, end_of_storage 三个迭代器,便可轻易提供首尾标示、大小、容量、空容器判断、注标( [ ] )运算子、最前端元素值、最后端元素值等功能。增加新元素 时,如果超过当时的容量,则容量会扩充至两倍。如果两倍容量仍不足,就扩张至足够大的容量。以下时push_back函数的源码。
```
void push_back(const T& x) {
if (finish != end_of_storage) {// 还有备用空间
construct(finish, x);
++finish;// 调整水位高度
}
else
// 已无备用空间
insert_aux(end(), x); // vector member function ,见以下列表
}
template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
if (finish != end_of_storage) {
// 还有备用空间
// 在备用空间起始处建构一个元素,并以 vector 最后一个元素值为其初值。
construct(finish, *(finish - 1));
// 调整水位。
++finish;
T x_copy = x;
copy_backward(position, finish - 2, finish - 1);
*position = x_copy;
}
else {
// 已无备用空间
const size_type old_size = size();
const size_type len = old_size != 0 ? 2 * old_size : 1;
// 以上配置原则:如果原大小为 0,则配置 1(个元素大小);
// 如果原大小不为 0,则配置原大小的两倍,
// 前半段用来放置原资料,后半段准备用来放置新资料。
iterator new_start = data_allocator::allocate(len); // 实际配置
iterator new_finish = new_start;
try {
// 将原 vector 的内容拷贝到新 vector。
new_finish = uninitialized_copy(start, position, new_start);
// 为新元素设定初值 x
construct(new_finish, x);
// 调整水位。
++new_finish;
// 将原 vector 的备用空间中的内容也忠实拷贝过来
new_finish = uninitialized_copy(position, finish, new_finish);
}
catch(...) {
// "commit or rollback" semantics.
destroy(new_start, new_finish);
data_allocator::deallocate(new_start, len);
throw;
}
// 解构并释放原 vector
destroy(begin(), end());
deallocate();
// 调整迭代器,指向新 vector
start = new_start;
finish = new_finish;
end_of_storage = new_start + len;
}
}
```
所谓动态增加大小,并不是在原空间之后接续新空间(因为无法保证原空间之后尚有可供配置的空间),而是以原大小的两倍另外配置一块较大空间,然后将原内容拷贝过来,然后才开始在原内容之后建构新元素,并释放原空间。因此,对 vector 的任何操作,一旦引起空间重新配置,指向原vector的所有迭代器就都失效了。

### vector的部分元素操作

* pop_back：将尾端元素拿掉,并调整大小
```
    void pop_back() {
    --finish;
    // 将尾端标记往前移一格,表示将放弃尾端元素。
    destroy(finish);
    }
```

* erase：
清除 [first,last) 中的所有元素：
```
    iterator erase(iterator first, iterator last) {
    iterator i = copy(last, finish, first); 
    destroy(i, finish);
    finish = finish - (last - first);
    return first;
    }
```

清除某个位置上的元素：
```
    iterator erase(iterator position) {
    if (position + 1 != end())
    copy(position + 1, finish, position); 
    --finish;
    destroy(finish);
    return position;
    }
```

* insert：在某点后插入n个元素x，仅列出部分源码。
```
if (n != 0) { // 当 n != 0 才进行以下所有动作
if (size_type(end_of_storage - finish) >= n)
// 备用空间大于等于「新增元素个数」
T x_copy = x;
// 以下计算安插点之后的现有元素个数
const size_type elems_after = finish - position;
iterator old_finish = finish;
if (elems_after > n)
// 「安插点之后的现有元素个数」大于「新增元素个数」
uninitialized_copy(finish - n, finish, finish);
finish += n;
// 将 vector 尾端标记后移
copy_backward(position, old_finish - n, old_finish);
fill(position, position + n, x_copy);
// 从安插点开始填入新值
}
else {
// 「安插点之后的现有元素个数」小于等于「新增元素个数」
uninitialized_fill_n(finish, n - elems_after, x_copy);
finish += n - elems_after;
uninitialized_copy(position, old_finish, finish);
finish += elems_after;
fill(position, old_finish, x_copy);
}
}
else {
// 备用空间小于「新增元素个数」(那就必须配置额外的内存)
// 首先决定新长度:旧长度的两倍,或旧长度+新增元素个数。
const size_type old_size = size();
const size_type len = old_size + max(old_size, n);
// 以下配置新的 vector 空间
iterator new_start = data_allocator::allocate(len);
iterator new_finish = new_start;
__STL_TRY {
// 以下首先将旧 vector 的安插点之前的元素复制到新空间。
new_finish = uninitialized_copy(start, position, new_start);
// 以下再将新增元素(初值皆为 n)填入新空间。
new_finish = uninitialized_fill_n(new_finish, n, x);
// 以下再将旧 vector 的安插点之后的元素复制到新空间。
new_finish = uninitialized_copy(position, finish, new_finish);
}
```

## list
相较于 vector 的连续线性空间, list 就显得复杂许多,它的好处是每次安插或删除一个元素,就配置或释放一个元素空间。因此, list 对于空间的运用有绝对的精准,一点也不浪费。而且,对于任何位置的元素安插或元素移除, list 永远是常数时间。

### list的节点
```
template <class T>
struct __list_node {
typedef void* void_pointer;
void_pointer prev;
void_pointer next;//双向串行
T data;
};
```

### list的迭代器
list 不再能够像vector 一样以指针做为迭代器,因为其节点不保证在储存空间中连续存在。list迭代器必须有能力指向list 的节点,并有能力做正确的递增、递减、取值、成员存取...等动作。递增时指向下一个节点,递减时指向上一个节点,取值时取的是节点的资料值,成员取用时取用的是节点的成员。安插动作(insert)和接合动作(splice)都不会造成原有的 list 迭代器失效。这在 vector 是不成立的,因为 vector 的安插动作可能造成记忆体重新配置,导致原有的迭代器全部失效。

### list的数据结构
SGI list不仅是一个双向链表,而且还是一个环状双向链表。所以它只需要一个node指针,便可以完整表现整个链表。如果让指针node 指向刻意置于尾端的一个空白节点, node 便能符合 STL 对于「前闭后开」区间的要求,成为 last 迭代器。利用node指针可以完成如下操作：
```
iterator begin() { return (link_type)((*node).next); }
iterator end() { return node; }
bool empty() const { return node->next == node; }
size_type size() const {
size_type result = 0;
distance(begin(), end(), result);
return result;
}
// 取头节点的内容(元素值)。
reference front() { return *begin(); }
// 取尾节点的内容(元素值)。
reference back() { return *(--end()); }
```

### list的建构与内存管理
节点的建构与解构函数很好理解：
```
// 配置一个节点并传回
link_type get_node() { return list_node_allocator::allocate(); }
// 释放一个节点
void put_node(link_type p) { list_node_allocator::deallocate(p); }
// 产生(配置并建构)一个节点,带有元素值
link_type create_node(const T& x) {
link_type p = get_node();
construct(&p->data, x);// 全域函数,建构/解构基本工具。
return p;
}
// 摧毁(解构并释放)一个节点
void destroy_node(link_type p) {
destroy(&p->data);
// 全域函数,建构/解构基本工具。
put_node(p);
}
```
默认构造函数构造一个空链表，令node指向自己。
```
void empty_initialize(){
node = get_node(); // 配置一个节点空间,令 node 指向它。
node->next = node; // 令 node 头尾都指向自己,不设元素值。
node->prev = node;
}
```
插入节点函数原理即为向链表中插入元素，插入位置在参数迭代器之前：
```
//:在迭代器 position 所指位置安插一个节点,内容为 x。
iterator insert(iterator position, const T& x) {
link_type tmp = create_node(x); // 产生一个节点(设内容为 x)
// 调整双向指针,使 tmp 安插进去。
tmp->next = position.node;
tmp->prev = position.node->prev;
(link_type(position.node->prev))->next = tmp;
position.node->prev = tmp;
return tmp;
}
```

### list的元素操作：

* void push_front(const T& x)：安插一个节点,做为头节点。动作为insert(begin(), x)
* void push_back(const T& x)：安插一个节点,做为尾节点。动作为insert(end(), x)
* iterator erase(iterator position)：移除迭代器 position 所指节点，动作同链表中删除节点。
* void pop_front()：移除头节点。动作为erase(begin());
* void pop_back()：移除头节点，对end（）前的节点调用erase。
* void list`<T, Alloc>`::clear()：清除所有节点。
* void list`<T, Alloc>`::remove(const T& value)：将数值为 value 之所有元素移除。
* void list`<T, Alloc>`::unique()：移除数值相同的连续元素。注意,只有「连续而相同的元素」,才会被移除剩一个。


## deque
vector 是单向开口的连续线性空间, deque 则是一种双向开口的连续线性空间。所谓双向开口,意思是可以在头尾两端分别做元素的安插和删除动作。vector 当然也可以在头尾两端做动作(从技术观点),但是其头部动作效率奇差,无法被接受。deque 和 vector 的最大差异,一在于 deque 允许于常数时间内对起头端进行元素的安插或移除动作,二在于 deque 没有所谓容量( capacity )观念,因为
它是动态地以分段连续空间组合而成,随时可以增加一段新的空间并链接起来。换句话说,像 vector 那样「因旧空间不足而重新配置一块更大空间,然后复制元素,再释放旧空间」这样的事情在 deque 是不会发生的。也因此, deque 没有必要提供所谓的空间保留(reserve )功能。

deque的迭代器并不是指针，比vector更复杂。因此,除非必要,我们应尽可能选择使用 vector 而非 deque 。对 deque 进行的排序动作,为了最高效率,可将 deque 先完整复制到一个 vector身上,将 vector 排序后(利用 STL sort 算法),再复制回 deque 。

### deque的中控器
deque在逻辑上看是连续空间，由一段一段的定量连续空间构成。一旦有必要在 deque 的前端或尾端增加新空间,便配置一段定量连续空间,串接在整个 deque 的头端或尾端。 deque 的最大任务,便是在这些分段的定量连续空间上,维护其整体连续的假象,并提供
随机存取的界面。避开了「重新配置、复制、释放」的轮回,代价则是复杂的迭代器架构。

deque 采用一块所谓的map做为主控。这里所谓map是一小块连续空间,其中每个元素(此处称为一个节点,node)都是指针,指向另一段(较大的)连续线性空间,称为缓冲区。缓冲区才是 deque 的储存空间主体。SGI STL 允许我们指定缓冲区大小,默认值 0 表示将使用 512 bytes 缓冲区。

### deque的迭代器
deque是分段连续空间。维护其「整体连续」假象的任务,着落在迭代器的operator++和 operator--两个运算子身上。deque 迭代器首先必须能够指出分段连续空间(亦即缓冲区)在哪里,其次它必须能够判断自己是否已经处于其所在缓冲区的边缘,如果是,一旦前进或后退时就必须跳跃至下一个或上一个缓冲区。为了能够正确跳跃, deque 必须随时掌握管控中心(map)。

每个迭代器都由以下四个指针保持与容器连结：
```
T* cur;// 缓冲区中的现行(current)元素
T* first;// 缓冲区的头
T* last;// 缓冲区的尾(含备用空间)
map_pointer node;// 指向管控中心
```

由于迭代器内对各种指标运算都做了多载化动作,所以各种指标运算如加、减、前进、后退...都不能直观视之。其中最重点的关键就是:一旦行进时遇到缓冲区边缘,要特别当心,视前进或后退而定,可能需要呼叫 set_node() 跳一个缓冲区:
```
void set_node(map_pointer new_node) {
node = new_node;
first = *new_node;
last = first + difference_type(buffer_size());
}
```

下面为自增和自减运算符的重载：
```
self& operator++() {
++cur;// 切换至下一个元素。
if (cur == last) {// 如果已达所在缓冲区的尾端,
set_node(node + 1);// 就切换至下一节点(亦即缓冲区)的第一个元素。
cur = first;
}
return *this;
}

self& operator--() {
if (cur == first) {// 如果已达所在缓冲区的头端,
set_node(node - 1);// 就切换至前一节点(亦即缓冲区)的最后一个元素。
cur = last;
}
--cur;
return *this;
}
```
迭代器可以直接跳过n个距离，以+=为例：
```
self& operator+=(difference_type n) {
difference_type offset = n + (cur - first);
if (offset >= 0 && offset < difference_type(buffer_size()))// 标的位置在同一缓冲区内
cur += n;
else {// 标的位置不在同一缓冲区内
difference_type node_offset =
offset > 0 ? offset / difference_type(buffer_size())
: -difference_type((-offset - 1) / buffer_size()) - 1;
set_node(node + node_offset);// 切换至正确的节点(亦即缓冲区)
cur = first + (offset - node_offset * difference_type(buffer_size()));// 切换至正确的元素
}
return *this;
}
```

### deque的数据结构
deque 除了维护一个先前说过的指向 map 的指针外,也维护 start, finish 两个迭代器,分别指向第一缓冲区的第一个元素和最后缓冲区的最后一个元素(的下一位置)。此外它当然也必须记住目前的 map 大小。因为一旦 map 所提供的节点不足,就必须重新配置更大的一块map。有了这些结构便可完成如下操作：
```
iterator begin() { return start; }
iterator end() { return finish; }
reference operator[](size_type n) {
return start[difference_type(n)]; // 唤起 __deque_iterator<>::operator[]
}
reference front() { return *start; } // 唤起 __deque_iterator<>::operator*
reference back() {
iterator tmp = finish;
--tmp;// 唤起 __deque_iterator<>::operator--
return *tmp;// 唤起 __deque_iterator<>::operator*
}

size_type size() const { return finish - start; }// 唤起 iterator::operator-
size_type max_size() const { return size_type(-1); }
bool empty() const { return finish == start; }
```

### deque的建构与内存管理
deque提供constructor如下：
```
deque(int n, const value_type& value): start(), finish(), map(0), map_size(0)
{
fill_initialize(n, value);
}
```

fill_initialize()函数负责产生并安排好deque的结构，并设置好元素的初值。
```
void deque<T, Alloc, BufSize>::fill_initialize(size_type n,
const value_type& value) {
create_map_and_nodes(n);// 把 deque 的结构都产生并安排好
map_pointer cur;
__STL_TRY {
// 为每个节点的缓冲区设定初值
for (cur = start.node; cur < finish.node; ++cur)
uninitialized_fill(*cur, *cur + buffer_size(), value);
// 最后一个节点的设定稍有不同(因为尾端可能有备用空间,不必设初值)
uninitialized_fill(finish.first, finish.cur, value);
}
}
```

create_map_and_nodes() 负责产生并安排好 deque 的结构，源码不在此列出。

以push_back为例展示如何在尾部插入元素：
```
void push_back(const value_type& t) {
if (finish.cur != finish.last - 1)// 最后缓冲区尚有一个以上的备用空间
construct(finish.cur, t); // 直接在备用空间上建构元素
++finish.cur;// 调整最后缓冲区的使用状态
}
else // 最后缓冲区已无(或只剩一个)元素备用空间。
push_back_aux(t);
}

void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
value_type t_copy = t;
reserve_map_at_back();// 若符合某种条件则必须重换一个 map
*(finish.node + 1) = allocate_node();// 配置一个新节点(缓冲区)
__STL_TRY {
construct(finish.cur, t_copy);// 针对标的元素设值
finish.set_node(finish.node + 1);// 改变 finish,令其指向新节点
finish.cur = finish.first;// 设定 finish 的状态
}
__STL_UNWIND(deallocate_node(*(finish.node + 1)));
}

void reserve_map_at_back (size_type nodes_to_add = 1) {
if (nodes_to_add + 1 > map_size - (finish.node - map))
// 如果 map 尾端的节点备用空间不足则必须重换一个 map(配置更大的,拷贝原来的,释放原来的)
reallocate_map(nodes_to_add, false);
}
```

reallocate_map源码不在此列出。

### deque的元素操作
pop_back, pop_front, clear, erase, insert等函数与list用法基本相同，源码不在此一一列出。



























