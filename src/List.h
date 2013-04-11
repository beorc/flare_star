#pragma once

#include "MemAllocator.h"

template <class T>
class List
{
public:
	
	struct ListNode {
		T Value;
		ListNode* Next;
	};

	MemAllocator ListNodeAllocator;

	List(unsigned pool_size=100);
	~List(void);

	ListNode *GetBegin() const;
	ListNode *GetEnd() const;
	ListNode *PushBack(T value);
	ListNode *PushFront(T value);
	bool Remove(T value);
	bool Remove(ListNode *);
	void Clear();
	bool IsEmpty() const;
	void PopBack();
	void PopFront();
	void Insert(ListNode *after, T value);
	int GetSize();

protected:		

	ListNode *Front, *End;
};

template <class T>
List<T>::List(unsigned pool_size) :
Front(NULL),
End(NULL),
ListNodeAllocator( sizeof(ListNode) )
{
	if (pool_size)
		ListNodeAllocator.Init(pool_size);
}

template <class T>
List<T>::~List(void)
{
	Clear();	
}

template <class T>
void List<T>::Clear()
{
	ListNode *node = Front, *prev=NULL;
	while (node)
	{		
		prev = node;
		node = node->Next;
		//delete prev;
		ListNodeAllocator.Free(prev);
	}
	Front = NULL;
	End = NULL;
}

template <class T>
typename List<T>::ListNode *List<T>::GetBegin() const
{
	return Front;
}

template <class T>
typename List<T>::ListNode *List<T>::GetEnd() const
{
	return End;
}

template <class T>
typename List<T>::ListNode *List<T>::PushFront(T value)
{
	//ListNode *node = new ListNode;
	ListNode *node = (ListNode *)ListNodeAllocator.Alloc();
	node->Value = value;	
	if (Front==NULL)
	{	
		End = node;
		node->Next = NULL;
	} else
	{		
		node->Next = Front;		
	}
	Front = node;
	
	return node;
}

template <class T>
typename List<T>::ListNode *List<T>::PushBack(T value)
{
	//ListNode *node = new ListNode;
	ListNode *node = (ListNode *)ListNodeAllocator.Alloc();
	node->Value = value;
	node->Next = NULL;
	if (Front==NULL)
	{		
		Front = node;		
	} else
	{
		End->Next = node;		
	}
	End = node;
	return node;
}

template <class T>
bool List<T>::Remove(T value)
{
	ListNode *node = Front, *prev=NULL;
	while (node)
	{
		if (node->Value==value)
		{	
			if (node==Front)
				Front=node->Next;
			else				
				prev->Next = node->Next;

			if (node==End)
				End = prev;			

			//delete node;
			ListNodeAllocator.Free(node);
			return true;
		}
		prev = node;
		node = node->Next;
	}
	return false;
}

template <class T>
bool List<T>::Remove(ListNode *node_to_del)
{
	ListNode *node = Front, *prev=NULL;
	while (node)
	{
		if (node==node_to_del)
		{	
			if (node==Front)
				Front=node->Next;
			else				
				prev->Next = node->Next;

			if (node==End)
				End = prev;			

			//delete node;
			ListNodeAllocator.Free(node);
			return true;
		}
		prev = node;
		node = node->Next;
	}
	return false;
}

template <class T>
bool List<T>::IsEmpty() const
{
	return Front==NULL ? true : false;
}

template <class T>
void List<T>::PopBack()
{
	if (End==NULL)
		return;
	if (End==Front)
	{	
		//delete End;
		ListNodeAllocator.Free(End);
		Front = NULL;
		End = NULL;
		return;
	}
	ListNode *node = Front;
	while (node->Next!=End)
	{	
		node = node->Next;
	}
	//delete End;
	ListNodeAllocator.Free(End);
	node->Next = NULL;
	End = node;
}

template <class T>
void List<T>::PopFront()
{
	if (Front==NULL)
		return;
	if (End==Front)
	{	
		//delete End;
		ListNodeAllocator.Free(Front);
		Front = NULL;
		End = NULL;
		return;
	}
	ListNode *node = Front->Next;
		
	ListNodeAllocator.Free(Front);
	Front = node;	
}

template <class T>
int List<T>::GetSize()
{	
	ListNode *node = Front;
	int size=0;
	while (node)
	{	
		++size;
		node = node->Next;
	}
	
	return size;
}

template <class T>
void List<T>::Insert(ListNode *after, T value)
{
	ListNode *node = (ListNode *)ListNodeAllocator.Alloc();
	node->Value = value;
	node->Next = after->Next;

	after->Next = node;

	if (after==End)
		End = node;
	return;
}

