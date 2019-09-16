#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"

#define MAX_LIST_HEAD 2
#define MAX_NODE 6

// Statically allocated lists and nodes
static List lists[MAX_LIST_HEAD];
static Node nodes[MAX_NODE];

//
static int list_index = 0;
static int node_index = 0;

static int num_freeList = 0;
static int num_freeNode = 0;

// Free Node Pool - Using Stack
// Extra pointer to store free nodes
static Node *freeNode;
static void AddFreeNode(Node* freenode);
static Node *GetFreeNode();


// Free List Pool - Using stack
// Extra pointer to store free lists
static List *freeList;
static void AddFreeList(List* freelist);
static List *GetFreeList();

// Function implementations
List *ListCreate(){
	return GetFreeList();
}

int ListCount(List *list){
	assert(list != NULL);
	return list->count;
}

void *ListFirst(List *list){
	// No List
	assert(list != NULL);
	// Empty List
	if (list->count == 0){
		return NULL;
	}
	// Return the first item
	else{
		list->current = list->head;
		return list->head->item;
	}
}

void *ListLast(List *list){
	// No List
	assert(list != NULL);
	// Empty List
	if (list->count == 0){
		return NULL;
	}
	// Return the last item
	else{
		list->current = list->tail;
		return list->tail->item;
	}
}

void *ListNext(List *list){
	// No List
	assert(list != NULL);
	// Empty List
	if (list->count == 0){
		return NULL;
	}
	// Current is at the end of the list
	if (list->current == list->tail){
		list->current = NULL;
		list->beyond = true;
		return NULL;
	}
	// current point is NULL
	if (list->current == NULL){
		// current point is before the list
		if (!list->beyond){
			//advance current to head, and return head item
			list->current = list->head;
			return list->head->item;
		}
		else{
			return NULL;
		}
	}
	// Advance the current item by one, and return the next item
	else{
		list->current = list->current->next;
		return list->current->item;
	}
}

void *ListPrev(List *list){
	// No List
	assert(list != NULL);
	// Empty List
	if (list->count == 0){
		return NULL;
	}
	// Current is at the start of the list
	if (list->current == list->head){
		list->current = NULL;
		list->beyond = false;
		return NULL;
	}
	// current is null
	if (list->current == NULL){
		// current is beyond the list
		if (list->beyond){
			list->current = list->tail;
			return list->current->item;
		}
		// current is before the list
		else{
			return NULL;
		}
	}
	// Back up the current item by one, and return the previous item
	else{
		list->current = list->current->prev;
		return list->current->item;
	}
}

void *ListCurr(List *list){
	// No List
	assert(list != NULL);
	// Empty List
	// Current pointer is pointing at beyond or before the list
	if (list->current == NULL){
		return NULL;
	}
	// Return the current item
	else{
		return list->current->item;
	}
}

int ListAdd(List *list, void *item){
	assert(list != NULL);
	//Check if there is any available node
	Node *node = GetFreeNode();
	if (node == NULL){
		return -1;
	}
	else{
		node->item = item;
		// Empty List
		if (list->count == 0){
			// Add it to List
			list->head = node;
			list->tail = node;
			list->current = node;
			list->count++;
			return 0;
		}
		// NULL current point
		else if (list->current == NULL){
			// current point is beyond
			if (list->beyond){
				node->prev = list->tail;
				list->tail->next = node;
				list->tail = list->tail->next;
				list->current = list->tail;
				list->count++;
				return 0;
			}
			// current point is before
			else{
				node->next = list->head;
				list->head->prev = node;
				list->head = list->head->prev;
				list->current = list->head;
				list->count++;
				return 0;
			}
		}
		// List has more than 2 items
		else{
			node->next = list->current->next;
			node->prev = list->current;
			list->current->next->prev = node;
			list->current->next = node;
			// If current is tail, update tail
			if (list->current == list->tail){
				list->tail = node;
			}
			list->current = node;
			list->count++;
			return 0;
		}
	}
}

int ListInsert(List *list, void *item){
	assert(list != NULL);
	//Check if there is any available node
	Node *node = GetFreeNode();
	if (node == NULL){
		return -1;
	}
	else{
		node->item = item;
		// Empty List
		if (list->count == 0){
			// Add it to List
			list->head = node;
			list->tail = node;
			list->current = node;
			list->count++;
			return 0;
		}
		else if (list->current == NULL){
			// current point is beyond
			if (list->beyond){
				node->prev = list->tail;
				list->tail->next = node;
				list->tail = list->tail->next;
				list->current = list->tail;
				list->count++;
				return 0;
			}
			// current point is before
			else{
				node->next = list->head;
				list->head->prev = node;
				list->head = list->head->prev;
				list->current = list->head;
				list->count++;
				return 0;
			}
		}
		else{
			node->next = list->current;
			node->prev = list->current->prev;
			list->current->prev->next = node;
			list->current->prev = node;
			// If current is head, update head
			if (list->current == list->head){
				list->head = node;
			}
			list->current = node;
			list->count++;
			return 0;
		}
	}
}

int ListAppend(List *list, void *item){
	assert(list != NULL);
	//Check if there is any available node
	Node *node = GetFreeNode();
	if (node == NULL){
		return -1;
	}
	else{
		// Initialize node
		node->item = item;
		// Empty List
		if (list->count == 0){
			list->head = node;
		}
		else{
			node->prev = list->tail;
			list->tail->next = node;
		}
		list->tail = node;
		list->current = node;
		list->count++;
		return 0;
	}
}

int ListPrepend(List *list, void *item){
	assert(list != NULL);
	//Check if there is any available node
	Node *node = GetFreeNode();
	if (node == NULL){
		return -1;
	}
	else{
		// Initialize node
		node->item = item;
		// Empty List
		if (list->count == 0){
			list->tail = node;
		}
		else{
			node->next = list->head;
			list->head->prev = node;
		}
		list->head = node;
		list->current = node;
		list->count++;
		return 0;
	}
}

void *ListRemove(List *list){
	// No List
	assert(list != NULL);
	// Empty list or current is NULL
	if (list->count == 0 || list->current == NULL){
		return NULL;
	}
	else{
		Node *temp = list->current;
		// There is only one item in list; thus, list becomes empty
		if (list->count == 1){
			list->head = NULL;
			list->tail = NULL;
			list->current = NULL;
		}
		// Current is head
		else if (list->current == list->head){
			// Make a new head
			list->head = list->head->next;
			// Advance current
			list->current = list->head;
			list->current->prev = NULL;
		}
		// current is tail
		else if (list->current == list->tail){
			// New tail
			list->tail = list->tail->prev;
			list->tail->next = NULL;
			// Current item is now tail as TA said
			list->current = list->tail;
		}
		else{
			list->current->prev->next = list->current->next;
			list->current->next->prev = list->current->prev;
			list->current = list->current->next;
		}
		// Removing node
		void *ret = temp->item;
		list->count--;
		// Add a freed node to free node pool
		AddFreeNode(temp);
		return ret;
	}
}

void ListConcat(List *list1, List *list2){
	// At least one of the lists doesn't exist. Can't concat
	assert(list1 != NULL);
	assert(list2 != NULL);
	// List 1 is empty
	if (list1->count == 0){
		// soft copy to list1
		list1->head = list2->head;
		list1->tail = list2->tail;
		list1->count = list2->count;
		// current is before the list
		list1->beyond = false;
	}
	// When List2 is empty, do nothing
	else if (list2->count == 0){
	}
	// Both List1 and List2 have nodes
	else{
		// Concat list1 tail and list2 head
		list1->tail->next = list2->head;
		list2->head->prev = list1->tail;
		// New tail
		list1->tail = list2->tail;
		list1->count += list2->count;
	}
	// Add list2 to the free list pool
	AddFreeList(list2);
}

void ListFree(List *list, freeFunc itemFree){
	// No List
	assert(list != NULL);
	// No itemFree function
	assert(itemFree != NULL);
	while (list->head != NULL){
		// Free items
		(*itemFree)(list->head->item);
		Node *temp = list->head->next;
		// Add deleted nodes to free node pool
		AddFreeNode(list->head);
		// Advance
		list->head = temp;
	}
	// Add it to list pool
	AddFreeList(list);
}

void *ListTrim(List *list){
	assert(list != NULL);
	// Empty list
	if (list->count == 0){
		return NULL;
	}
	void *ret = list->tail->item;
	AddFreeNode(list->tail);
	// Only one item
	if (list->count == 1){
		list->head = NULL;
	}
	list->current = list->tail->prev;
	list->tail = list->current;
	list->count--;
	return ret;
}

void *ListSearch(List *list, compFunc comparator, void *comparisonArg){
	assert(list != NULL);
	assert(comparator != NULL);

	int match;
	while (list->current != NULL){
		match = (*comparator)(list->current->item, comparisonArg);
		if (match == 1){
			return list->current->item;
		}
		else{
			list->current = list->current->next;
		}
	}
	list->beyond = true;
	return NULL;
}

///////////////////////////////////////////////////////////////////////
static void AddFreeNode(Node* freenode){
	if (freeNode == NULL){
		freeNode = freenode;
	}
	else{
		Node *temp = freeNode;
		freeNode = freenode;
		freeNode->next = temp;
	}
	num_freeNode++;
}
static Node *GetFreeNode(){
	// No available node
	if (node_index >= MAX_NODE && num_freeNode == 0){
		printf("Nodes are exhausted. Returning NULL\n");
		return NULL;
	}
	else{
		if (node_index < MAX_NODE){
			node_index++;
			return &nodes[node_index-1];
		}
		else{
			Node *temp = freeNode;
			freeNode = temp->next;
			num_freeNode--;
			// Initialization
			temp->prev = NULL;
			temp->next = NULL;
			temp->item = NULL;
			return temp;
		}
	}
}

static void AddFreeList(List* freelist){
	if (freeList == NULL){
		freeList = freelist;
	}
	else{
		List *temp = freeList;
		freeList = freelist;
		freeList->next = temp;
	}
	num_freeList++;
}

static List *GetFreeList(){
	// No available list
	if (list_index >= MAX_LIST_HEAD && num_freeList == 0){
		printf("Lists are exhausted. Returning NULL\n");
		return NULL;
	}
	else{
		if (list_index < MAX_LIST_HEAD){
			list_index++;
			return &lists[list_index-1];
		}
		else{
			List *temp = freeList;
			freeList = temp->next;
			num_freeList--;
			// Initialize list
			temp->next = NULL;
			temp->count = 0;
			temp->head = NULL;
			temp->tail = NULL;
			temp->current = NULL;
			return temp;
		}
	}
}
