
#include<stdio.h>
#include "tinyvector.h"
#include"list.h"
#include<iostream>
#include<functional>

using namespace std;
int main()
{
	//测试vector
	cout << "vector test\n";
	tiny_vector<int> vec;
    vec.push_back(10);
    vec.push_back(9);
    vec.push_back(8);
    for(auto &w : vec) {
        cout<<w<<endl;
    }
    cout << "list test"<< endl;
	tiny_list<int> l1(8,5);
	l1.insertAsLast(1);
	l1.insertAsLast(2);
	l1.insertAsFirst(20);
	l1.insertAsFirst(70);
	l1.insertAsLast(62);
	l1.sort();
	for (int i = 0; i < l1.size(); i++)
	{
		cout << l1[i] << "  ";
	}
	cout << endl;
	l1.reverse();
	for (int i = 0; i < l1.size(); i++)
	{
		cout << l1[i]<<"  ";
	}

	cout << endl;
	int cou = l1.uniquify();
	cout << cou << endl;
	cout << l1 << endl;


    cout << "test finished" << endl;
    return 0;
   
}