#include<stdio.h>
#include "tinyvector.h"
#include"list.h"
#include<iostream>
#include<functional>

using namespace std;
int main()
{

/**********************测试vector容器的基本功能**********************/
	typedef tiny_vector<int> INTVECTOR;//自定义类型INTVECTOR       
    //vec1对象初始为空  
    INTVECTOR vec1;    
    //vec2对象最初有10个值为6的元素    
    INTVECTOR vec2(6,6);   
    //vec3对象最初有3个值为6的元素，拷贝构造  
    INTVECTOR vec3(vec2.begin(),vec2.begin()+3);   
    //声明一个名为i的迭代器  
    INTVECTOR::iterator i;  
    
    //从前向后显示vec1中的数据  
    cout<<"vec1.begin()--vec1.end():"<<endl;  
    for (i =vec1.begin(); i !=vec1.end(); ++i)  
        cout << *i << " ";  
    cout << endl;  
    
    //从前向后显示vec2中的数据  
    cout<<"vec2.begin()--vec2.end():"<<endl;  
    for (i =vec2.begin(); i !=vec2.end(); ++i)  
       cout << *i << " ";  
    cout << endl;  
    //从前向后显示vec3中的数据  
    cout<<"vec3.begin()--vec3.end():"<<endl;  
    for (i =vec3.begin(); i !=vec3.end(); ++i)  
        cout << *i << " ";  
    cout << endl;  
    //测试添加和插入成员函数  
    vec1.push_back(2);//从后面添加一个成员  
    vec1.push_back(4);  
    vec1.insert(vec1.begin(),1,5);//在vec1第一个的位置上插入成员5   
    for (i =vec1.begin(); i !=vec1.end(); ++i)  
        cout << *i << " ";  
    cout << endl;  
    //测试移出和删除  
    vec1.pop_back();//将最后一个成员移出vec1 
    vec1.erase(vec1.begin(),vec1.end()-2);//删除成员  
    cout<<"vec1.pop_back() " <<endl;  
    for (i =vec1.begin(); i !=vec1.end(); ++i)  
        cout << *i << " ";  
    cout << endl;  
    vec1.erase(vec1.begin(),vec1.end()-1);//删除成员  
    cout<<"vec1.erase(): " <<endl;  
    for (i =vec1.begin(); i !=vec1.end(); ++i)  
        cout << *i << " ";  
    cout << endl;
    
    //显示序列的状态信息  
    cout<<"vec1.size(): "<<vec1.size()<<endl;//打印成员个数  
    cout<<"vec1.empty(): "<<vec1.empty()<<endl;//清空
    cout << "Hello World!" << endl;
/**********************测试list容器的基本功能**********************/
    tiny_list<char> l1(8,'a');
	l1.insertAsLast('b');
	l1.insertAsLast('d');
	l1.insertAsFirst('c');
	l1.insertAsFirst('h');
	l1.insertAsLast('k');
	cout<<"unsorted list:\n";
	for (int i = 0; i < l1.size(); i++)
	{
		cout << l1[i] << "  ";
	}
	cout << endl;
	l1.sort(0);
	cout<<"sorted list:\n";
	for (int i = 0; i < l1.size(); i++)
	{
		cout << l1[i] << "  ";
	}
	cout << endl;
	cout<<"reverseed list:\n";
	l1.reverse();
	for (int i = 0; i < l1.size(); i++)
	{
		cout << l1[i]<<"  ";
	}

	cout << endl;
	cout<<"uniqued list:\n";
	int cou = l1.uniquify();
	cout << cou << endl;
	cout << l1 << endl;

    return 0;
   
}