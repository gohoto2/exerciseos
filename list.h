#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>

// Node Struct
typedef struct _Node{
	void *item;
	struct _Node *next;
	struct _Node *prev;
}Node;

// List Struct
typedef struct _List{
	Node *head;
	Node *tail;
	Node *current;
	// the number of items in this list
	int count;
	// if true, the current pointer is beyond the end of the list; if false, before the start of the list
	bool beyond;
	// only to keep track of free list in free pool
	struct _List *next;
}List;

// Routines for ListFree and ListSearch
typedef void (*freeFunc)(void*);
typedef int (*compFunc)(void*, void*);

// List manipulation routines
List *ListCreate();
int ListCount(List *list);
void *ListFirst(List *list);
void *ListLast(List *list);
void *ListNext(List *list);
void *ListPrev(List *list);
void *ListCurr(List *list);
int ListAdd(List *list, void *item);
int ListInsert(List *list, void *item);
int ListAppend(List *list, void *item);
int ListPrepend(List *list, void *item);
void *ListRemove(List *list);
void ListConcat(List *list1, List *list2);
void ListFree(List *list, freeFunc itemFree);
void *ListTrim(List *list);
void *ListSearch(List *list, compFunc comparator, void *comparisonArg);

#endif /* LIST_H_ */
