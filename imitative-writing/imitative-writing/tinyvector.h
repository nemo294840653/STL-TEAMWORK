#pragma once
#ifndef tiny_VECTOR_H
#define tiny_VECTOR_H
#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
//#include <algorithm>
#include <memory>
using namespace std;

template<class T>
void destroy(T* pointer)
{
	pointer->~T();
}
template<class ForwardIterator>

void destroy(ForwardIterator first, ForwardIterator last)
{
	for (ForwardIterator it = first; it != last; ++it)
	{
		destroy(&*it);
	}
}
template<class T>
//定义tiny_vector模版类
class tiny_vector {
	
	friend ostream& operator<<(ostream &out, tiny_vector<T> &obj)
	{
		for (iterator it = obj.start; it != obj.finish; it++)
		{
			out << *it << " ";
		}
		out << endl;
		return out;
	}
public:
	typedef	T value_type;
	typedef T*	iterator;
	typedef	const T* const_iterator ;
	typedef T*	pointer ;
	typedef	T& reference ;
	typedef	const T& const_reference ;
	typedef	size_t size_type;
public:
	tiny_vector();
	tiny_vector(size_t n, T  obj);
	tiny_vector(iterator begin, iterator end);
	~tiny_vector();
	tiny_vector(const tiny_vector& obj);
	tiny_vector& operator=(const tiny_vector& obj);
	bool empty() const { return begin() == end(); }											//判断容器是否为空
	size_type size() const { return (size_type)(finish - start); }							//返回容器的元素个数
	size_type capacity() const { return (size_type)(end_of_storage - start); }				//返回容器的当前内存大小
	iterator begin() { return start; }														//返回首部迭代器
	const_iterator begin() const { return start; }											
	iterator end() { return finish; }														//返回尾部迭代器
	const_iterator end() const { return finish; }
	reference operator[](size_type i) { return *(start + i); }								//重载[]迭代器，返回第i的元素的值
	const_reference operator[](size_type i)const { return *(start + i); }
	void push_back(const T& value);															//在vector的最后添加一个数据, 示例： c.push_back(5);
	void insert(iterator position, size_type n, const T& value);							//在迭代器之前插入n个值为value的元素	
    void pop_back();																		//去掉vector的最后一个数据, 示例：c.pop_back();
	void erase(iterator first, iterator last);												//删除迭代器first与last之间的数据项
    void clear();																			//清空当前的vecto
	void reserve(size_type n);																//改变当前vecotr所分配空间的大小
	void resize(size_type n, value_type &value);											//改变当前使用数据的大小，如果它比当前使用的大，则填充默认值
	
protected:
	iterator start;  																		//空间的头
	iterator finish;  																		//空间的尾
	iterator end_of_storage; 																//可用空间的尾
private:
	static std::allocator<T> alloc; 														// object to get raw memory

};
template<class T>
allocator<T> tiny_vector<T>::alloc;

template<class T>
//新建一个vector，start,finish,end_of_storage都初始化为空
tiny_vector<T>::tiny_vector()
{
	start = nullptr;
	finish = nullptr;
	end_of_storage = nullptr;
}

template<class T>
//新建一个vector,大小为n
tiny_vector<T>::tiny_vector(size_t n, T  obj)
{
	//alloc 是SGI STL的标准配置器，allocate用于配制空间，足以存储n个T对象
	start = alloc.allocate(n);
	end_of_storage = finish = start + n;
	for (iterator i = start; i != finish; ++i)
		//等同于 new(const void*) p) T(x) 
		alloc.construct(i, obj);
}
template<class T>
tiny_vector<T>::tiny_vector(iterator begin, iterator end)
{
	const size_type n = end - begin;
	start = alloc.allocate(n);
	end_of_storage = finish = start + n;
	/*uninitialized_copy函数接受三个迭代器参数：迭代器begin是输入的起始地址，
	*迭代器end是输入的结束地址，迭代器start是输出的起始地址
	*即把数据复制到[start,start+(end-begin)]这个范围
	*/
	std::uninitialized_copy(begin, end, start);
}
template<class T>
//vector的析构
tiny_vector<T>::~tiny_vector()
{
	::destroy(start, finish);
	//归还配置空间
	alloc.deallocate(start, end_of_storage - start);
}
template<class T>
//vector obj的复制
tiny_vector<T>::tiny_vector(const tiny_vector &obj)
{
    const	size_type n = obj.capacity();
	start = alloc.allocate(n);
	std::uninitialized_copy(obj.start, obj.finish, start);
	finish = start + (obj.finish - obj.start);
	end_of_storage = start + (obj.end_of_storage - obj.start);
}
template<class T>
tiny_vector<T>& tiny_vector<T>::operator=(const tiny_vector& obj)
{
	if (this->capacity()) {
		destroy(start, finish);
		alloc.deallocate(start, end_of_storage - start);
		start = finish = end_of_storage = nullptr;
	}
	const	size_type n = obj.capacity();
	start = alloc.allocate(n);
	std::uninitialized_copy(obj.start, obj.finish, start);
	finish = start + (obj.finish - obj.start);
	end_of_storage = start + (obj.end_of_storage - obj.start);
	return *this;
}
template<class T>
//对vector的插入操作
void tiny_vector<T>::insert(iterator position, size_type n, const T& value)
{
	if(n!=0)
	{	//当n!=0时才有下面的操作
		if (n<end_of_storage-finish)
		{
			//备用空间大于新增元素的个数
			if (n < finish - position) {
				//插入点之后的现有元素个数大于新增元素个数
				std::uninitialized_copy(finish - n, finish, finish);
				std::copy(position, finish - n, position + n);
				std::fill_n(position, n, value);
			}
			else {
				//插入点之后的现有元素个数小于新增元素个数
				std::uninitialized_fill_n(finish, n - (finish - position), value);
				std::uninitialized_copy(position,finish,position+n);
				std::fill(position, finish, value);
			}
				finish += n;//vector尾端标记后移
		}
		else
		{	//备用空间小于新增元素的个数，需要配置额外的内存
			iterator starttemp = start, finishtemp = finish, end_of_storagetemp = end_of_storage;
			size_t oldspace = end_of_storage - start;
			size_t new_size = oldspace + max(n, oldspace);//新长度为旧长度的两倍或者旧长度🏠新增元素个数
			starttemp = alloc.allocate(new_size);
			finishtemp= std::uninitialized_copy(start, position, starttemp);  //带有copy和fill的，第二个迭代器是最后一个元素的下一个迭代器
			std::uninitialized_fill_n(finishtemp, n, value);		
			finishtemp += n;
			finishtemp = std::uninitialized_copy(position, finish, finishtemp);
			destroy(start, finish);//清除并释放旧的vector
			alloc.deallocate(start, end_of_storage - start);
			start = starttemp;
			finish = finishtemp;
			end_of_storagetemp = start + new_size;
		}
	}
}

template<class T>
void tiny_vector<T>::push_back(const T& value)
{
	insert(this->end(), 1, value);
}

template<class T>
//将尾端元素拿掉，并调整大小
void tiny_vector<T>::pop_back() 
{
	if (finish > start) {
		::destroy(finish - 1, finish);
		finish--;
	}
	else
	{
		cerr<< "pop err!" << endl;
	}
}
template<class T>
//清除【first，last】中的元素
void tiny_vector<T>::erase(iterator first, iterator last)
{
	iterator finishtemp=finish;
    finish=copy(last,finish,first);
	::destroy(finish, finishtemp);
}
template<class T>
//清除所有元素
void tiny_vector<T>::clear()
{
	 //erase(start, finish);	 //作用和下面两句一样
	::destroy(start, finish);
	finish = start;
}


template<class T>
//reserve的功能是如果当前容量小于n，则分配空间，如果当前容量大于n,则不作处理
void tiny_vector<T>::reserve(size_type n)
{

	if (capacity() < n)
	{
		size_t len = finish - start;
		iterator starttemp = alloc.allocate(n);
		std:: uninitialized_copy(start, finish, starttemp);
		::destroy(start, finish);
		alloc.deallocate(start, size());
		start = starttemp;
		finish = start+len;
		end_of_storage = starttemp + n;
	}
}
template<class T>
void tiny_vector<T>::resize(size_type n, value_type &value)
{
	if (n < capacity())
	{
		erase(start + n, finish);
	}
	else
	{
		insert(end(), n - size(), value);
	}
}

#endif