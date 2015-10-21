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

//����raii�࣬���Զ���

class Traii {
public:
	Traii(){}
	Traii(int rhs){ val = rhs; }
	int getRaii() { return val; }
	void setRaii(int rhs) { val = rhs; }

	//����
	~Traii()
	{
		std::cout << "destroyed" << std::endl;
	}
private:
	int val;
};

//ģ��ṹ
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

/*�ڴ������������STL��*/
class MyMemoryAllocator 
{
public:
	MyMemoryAllocator ();
	~MyMemoryAllocator ();

	/*ָ���ռ��ڴ����*/
	static inline void* Allocate(size_t size)
	{
		return ::operator new(size, ::std::nothrow);
	}

	/*ָ����ʼ��ַ�Լ��ڴ���С������*/
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

	//���������ÿһ���ڵ���ڴ��
	//�������ϵ��

	void NewNodeMemory(bool bExpand);	//��ǰ�ڵ��ڴ治��ʱ�����½ڵ�
	U* zygoteObjFromRecycle();  //�ӻ��ճ��л��ն���

	bool deleteObj(U* obj);	//���״Ӷ���������ٶ���
	bool RecycleObjToPool(U* objptr);	//������ŵ�������н��л���

	void InitObjPool(size_t initSize);	//����صĳ�ʼ��
	void destroyObjPool();	//���ٶ����

private:
	/*��ǰ�ڵ��ڴ��Ĵ�С*/
	size_t _nodeCapacity;
	//�����ڴ�����
	struct LinkNode
	{
		/*�ڴ��ָ��*/
		void* _memory;
		size_t _countInNode;		//ÿһ���ڵ㣨�ڴ�飩���еĶ�������
		size_t _capacity;
		/*next*/
		LinkNode* _nextNode;

		//���캯��
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

		//����
		~LinkNode()
		{
			TAllocator::deAllocate(_memory, _capacity * (sizeof(U)));
		}

	};
	std::list<U*> recycledMemoryList;
	LinkNode* QHeader = NULL;
	LinkNode* Qtail = NULL;
};

