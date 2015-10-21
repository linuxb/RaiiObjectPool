#ifndef RAII_H
#define RAII_H
#endif

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <Windows.h>
#include <process.h>

#define MEMORY_RATE 2
#define MAX_MEMORYBLOCK 60
#define EXPAND_THELDSHOLD 20

//定义raii类，测试对象

class Traii {
public:
	Traii(){}
	Traii(int rhs){ val = rhs; }
	int getRaii() { return val; }
	void setRaii(int rhs) { val = rhs; }

	//析构
	~Traii()
	{
		std::cout << "destroyed" << std::endl;
	}
private:
	int val;
};

//模板结构
template<class T>
class raiiTMaker
{
public:
	raiiTMaker();
	~raiiTMaker();

	T* create(T* obj);

private:
	std::vector<T*> ptrList;
};

/*内存分配器（参照STL）*/
class MyMemoryAllocator 
{
public:
	MyMemoryAllocator ();
	~MyMemoryAllocator ();

	/*指定空间内存分配*/
	static inline void* Allocate(size_t size)
	{
		return ::operator new(size, ::std::nothrow);
	}

	/*指定初始地址以及内存块大小的析构*/
	static inline void deAllocate(void* ptr, size_t size)
	{
		::operator delete(ptr, ::std::nothrow);
	}

private:
	//
};

template<class U, class TAllocator = MyMemoryAllocator>
class ObjectPool
{
public:
	ObjectPool();
	ObjectPool(ObjectPool<U, TAllocator>& source);
	~ObjectPool();

	void operator = (const ObjectPool<U, TAllocator>& rhs);

	//用链表管理每一个节点的内存块
	//定义递增系数

	void NewNodeMemory(bool bExpand);	//当前节点内存不足时创建新节点
	U* zygoteObjFromRecycle();  //从回收池中回收对象

	bool deleteObj(U* obj);	//彻底从对象池中销毁对象
	bool RecycleObjToPool(U* objptr);	//将对象放到对象池中进行回收

	void InitObjPool(size_t initSize);	//对象池的初始化
	void destroyObjPool();	//销毁对象池

private:
	/*当前节点内存块的大小*/
	size_t _nodeCapacity;
	//定义内存链表
	struct LinkNode
	{
		/*内存块指针*/
		void* _memory;
		size_t _countInNode;		//每一个节点（内存块）含有的对象数量
		size_t _capacity;
		/*next*/
		LinkNode* _nextNode;

		//构造函数
		LinkNode(size_t capacity)
		{
			if (capacity < 1)
			{
				throw std::invalid_argument("capacity must be at least 1");
			}
			if (capacity > MAX_MEMORYBLOCK)
			{
				throw std::invalid_argument("capacity can not out of the limit memory!");
				exit(1);
			}
			/*Allocate*/
			_memory = TAllocator::Allocate(capacity * (sizeof(U)));
			if (_memory == NULL) throw std::bad_alloc();
			/*init*/
			_capacity = capacity;
			_countInNode = 0;
		}

		//析构
		~LinkNode()
		{
			TAllocator::deAllocate(_memory, _capacity * (sizeof(U)));
		}

	};
	std::list<U*> recycledMemoryList;
	LinkNode* QHeader = NULL;
	LinkNode* Qtail = NULL;
};

