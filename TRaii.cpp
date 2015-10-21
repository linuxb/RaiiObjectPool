// TRaii.cpp : �������̨Ӧ�ó������ڵ㡣
//
//RAII�������ú���scopeʵ�ֶ����Զ�����
//�Զ�����ڴ����

#include "stdafx.h"
#include "raii.h"

ObjectPool<Traii> objPool;
std::vector<Traii*> objList;

template<class U, class TAllocator = MyMemoryAllocator>
ObjectPool<U, TAllocator>::ObjectPool()
{
}

template<class U, class TAllocator = MyMemoryAllocator>
ObjectPool<U, TAllocator>::~ObjectPool()
{
	destroyObjPool();
}

template<class T>
raiiTMaker<T>::raiiTMaker()
{
}

template<class T>
raiiTMaker<T>::~raiiTMaker()
{
	//��ȫ����
	while (!ptrList.empty())
	{
		T* p = ptrList.back();
		ptrList.pop_back();
		delete p;
	}
}

template<class T>
T* raiiTMaker<T>::create(T* obj)
{
	if (obj == NULL)
	{
		std::cerr << "error: Ivalid Param" << std::endl;
		return NULL;
	}
	raiiTMaker::ptrList.push_back(obj);
	return obj;
}

/*�ռ������*/
MyMemoryAllocator::MyMemoryAllocator()
{
}

MyMemoryAllocator::~MyMemoryAllocator()
{
}

template<class U, class TAllocator = MyMemoryAllocator>
void ObjectPool<U, TAllocator>::InitObjPool(size_t initSize)
{
	bool bExp = (initSize < EXPAND_THELDSHOLD) ? true : false;
	_nodeCapacity = initSize;
	NewNodeMemory(bExp);
}

/*����һ���µĶ��������ڵ�*/
template<class U, class TAllocator = MyMemoryAllocator>
void ObjectPool<U, TAllocator>::NewNodeMemory(bool bExpand)
{
	if (_nodeCapacity > MAX_MEMORYBLOCK)
	{
		_nodeCapacity = MAX_MEMORYBLOCK;
	}
	else
	{
		if (bExpand)
		{
			_nodeCapacity *= MEMORY_RATE;
			if (_nodeCapacity > MAX_MEMORYBLOCK)
			{
				_nodeCapacity = MAX_MEMORYBLOCK;
			}
		}
	}
	/*memory block size*/
	LinkNode* node = new LinkNode(_nodeCapacity);
	if (node == NULL) throw std::bad_alloc();
	if (QHeader == NULL)
	{
		QHeader = node;
		Qtail = QHeader;
	}
	else
	{
		if (Qtail != NULL)
		{
			Qtail->_nextNode = node;
			Qtail = node;
		}
	}
	node->_nextNode = NULL;
}

template<class U,class TAllocator = MyMemoryAllocator>
U* ObjectPool<U, TAllocator>::zygoteObjFromRecycle()
{
	if (!recycledMemoryList.empty())
	{
		/*memory controlled by operator*/
		U* result = (U*)recycledMemoryList.front();
		recycledMemoryList.pop_front();
		/*initialization*/
		new(result) U(0);
		return result;
	}
	/*need to allocate new memory in the current node*/
	if ((Qtail->_countInNode) >= (Qtail->_capacity)) NewNodeMemory(true);
	char* addr = (char*)Qtail->_memory;
	addr += (Qtail->_countInNode)*(sizeof(U));
	Qtail->_countInNode++;
	U* result = new(addr) U(0);
	return result;
}

template<class U, class TAllocator = MyMemoryAllocator>
bool ObjectPool<U, TAllocator>::deleteObj(U* obj)
{
	//������������
	obj->~U();
	recycledMemoryList.push_back(obj);
	return true;
}

template<class U, class TAllocator = MyMemoryAllocator>
bool ObjectPool<U, TAllocator>::RecycleObjToPool(U* objptr)
{
	recycledMemoryList.push_back(objptr);
	return true;
}

template<class U, class TAllocator = MyMemoryAllocator>
void ObjectPool<U, TAllocator>::destroyObjPool()
{
	//��������
	LinkNode* p = QHeader;
	LinkNode* q = p;
	if (p == NULL) return;
	while (p != NULL)
	{
		p = p->_nextNode;
		delete q;
		q = p;
	}
}

void DoWork();

void TestObjPool(int times)
{
	for (int i = 0; i < times; i++)
	{
		Traii* obj = objPool.zygoteObjFromRecycle();
		//to do some work
		objList.push_back(obj);
	}
	Sleep(800);
	for (int j = 0; j < (times / 2); j++)
	{
		Traii* p = objList.back();
		objList.pop_back();
		objPool.deleteObj(p);
	}
	Sleep(800);
	for (int k = 0; k < (times / 2); k++)
	{
		Traii* obj = objPool.zygoteObjFromRecycle();
		objList.push_back(obj);
	}
	Sleep(800);
	while (!objList.empty())
	{
		Traii* p = objList.back();
		objList.pop_back();
		objPool.deleteObj(p);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	objPool.InitObjPool(15);
	/*����ز���*/
	TestObjPool(36);
	/***raii***/
	//DoWork();
	system("pause");
	return 0;
}

void DoWork()
{
	raiiTMaker<Traii> maker;
	for (int i = 0; i < 5; i++)
	{
		Traii* t = maker.create(new Traii());
	}
	//������
	//�Զ��ڴ����
}



