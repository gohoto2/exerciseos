#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

void StackFree(void *item){
	// Variables in stack cannot be freed
	// Do NOTHING
}

void HeapFree(void *item){
	free(item);
}

int CompDouble(void *item, void *comparisonArg){
	if (*(double *)item == *(double *)comparisonArg){
		return 1;
	}
	else{
		return 0;
	}
}

int CompFloat(void *item, void *comparisonArg){
	if (*(float *)item == *(float *)comparisonArg){
		return 1;
	}
	else{
		return 0;
	}
}

int CompString(void *item, void *comparisonArg){
	if ((char *)item == (char *)comparisonArg){
		return 1;
	}
	else{
		return 0;
	}
}

int CompInt(void *item, void *comparisonArg){
	if (*(int *)item == *(int *)comparisonArg){
		return 1;
	}
	else{
		return 0;
	}
}

typedef enum{
	INT,
	FLOAT,
	DOUBLE,
	CHAR
}Type;

void print(void *item, Type type){
	if (type == INT){
		printf("%d ", *(int *)item);
	}
	else if (type == FLOAT){
		printf("%f ", *(float *)item);
	}
	else if (type == DOUBLE){
		printf("%f ", *(double *)item);
	}
	else if (type == CHAR){
		printf("%s ", (char*)item);
	}
}

int main(){
	printf("#################################\n");
	printf("#####   Name: Jun Goo Kim   #####\n##### Student ID: 301046893 #####\n");
	printf("#################################\n\n");
	printf("\"Testing my list implementation\"\nMax List Heads: 2\nMax Nodes: 6\n\n");
	printf("Passing NULL to any function I've implemented will stop this test driver by assert\n\n");

	Type type[6];
	List *list1 = ListCreate();
	void **item = (void**)malloc(6*sizeof(void*));
	// Testing ListAdd()
	printf(".....................................Testing ListAdd()........................................\n");
	printf("Adding int 1 to an empty list\n");
	int a = 1;
	item[4] = &a;
	type[4] = INT;
	int res = ListAdd(list1, item[4]);
	if (res == -1){
		printf("Adding failed...\n");
		return -1;
	}
	if (list1->head->item != item[4]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->tail->item != item[4]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->current->item != item[4]){
		printf("Not properly added...\n");
		return -1;
	}
	printf("Adding float 4.33 to the list when current pointer is beyond the end\n");
	float b = 4.33;
	item[5] = &b;
	type[5] = FLOAT;
	list1->current = NULL;
	list1->beyond = true;
	res = ListAdd(list1, item[5]);
	if (res == -1){
		printf("Adding failed...\n");
		return -1;
	}
	if (list1->head->item != item[4]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->tail->item != item[5]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->current != list1->tail){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->head->next != list1->tail){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->tail->prev != list1->head){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->tail->next != NULL){
		printf("Not properly added...\n");
		return -1;
	}
	if(list1->head->prev != NULL){
		printf("Not properly added...\n");
		return -1;
	}
	printf("Adding double 4.33 to the list when current pointer is before the start\n");
	list1->current = NULL;
	list1->beyond = false;
	double c = 4.33;
	item[0] = &c;
	type[0] = DOUBLE;
	res = ListAdd(list1, item[0]);
	if (res == -1){
		printf("Adding failed...\n");
		return -1;
	}
	if (list1->head->item != item[0]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->head->prev != NULL){
		printf("Not properly added...\n");
		return -1;
	}

	printf("Adding char A+ between two nodes\n");
	char d[5] = "A+";
	item[1] = d;
	type[1] = CHAR;
	res = ListAdd(list1, item[1]);
	if (res == -1){
		printf("Adding failed...\n");
		return -1;
	}
	if (list1->current->prev->item != item[0]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->current->item != item[1]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->current->next->item != item[4]){
		printf("Not properly added...\n");
		return -1;
	}
	printf("Adding int 4\n");
	int e = 4;
	item[2] = &e;
	type[2] = INT;
	res = ListAdd(list1, item[2]);
	if (res == -1){
		printf("Adding failed...\n");
		return -1;
	}
	if (list1->current->item != item[2]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->current->prev->item != item[1]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->current->next->item != item[4]){
		printf("Not properly added...\n");
		return -1;
	}
	if (list1->current->next->prev->item != item[2]){
		printf("Not properly added...\n");
		return -1;
	}

	printf("Adding int 5\n");
	int f = 5;
	item[3] = &f;
	type[3] = INT;
	res = ListAdd(list1, item[3]);
	if (res == -1){
		printf("Adding failed...\n");
		return -1;
	}
	printf("Adding an item without any free node: ");
	int g = 44;
	void *fail = &g;
	res = ListAdd(list1, fail);
	if (res == 0){
		printf("ListAdd() is not working...\n");
		return -1;
	}


	printf("\n");
	printf("Items must be in this order: ");
	printf("[ ");
	for (int ii = 0; ii < 6; ii++){
		print(item[ii],type[ii]);
	}
	printf("]\n");
	printf("Printing the current list: ");
	Node *temp = list1->head;
	printf("[ ");
	for (int ii = 0; ii < 6; ii++){
		print(temp->item, type[ii]);
		temp = temp->next;

	}
	printf("]\n");
	printf("...................................Testing ListAdd() End......................................\n\n");


	printf("....................................Testing ListCount().......................................\n");
	if (ListCount(list1) != 6){
		printf("ListCount() is not working\n");
		return -1;
	}
	printf("The current number of items: %d\n", ListCount(list1));
	printf("..................................Testing ListCount() End.....................................\n\n");
	
	printf("....................................Testing ListFirst().......................................\n");
	void *check = ListFirst(list1);
	if (check != item[0]){
		printf("ListFirst() is not working...\n");
		return -1;
	}
	printf("Printing the first item: %lf\n", *(double *)check);
	printf("..................................Testing ListFirst() End.....................................\n\n");

	printf("....................................Testing ListLast()........................................\n");
	check = ListLast(list1);
	if (check != item[5]){
		printf("ListLast() is not working...\n");
		return -1;
	}
	printf("Printing the last item: %f\n", *(float *)check);
	printf("..................................Testing ListLast() End......................................\n\n");

	printf("...................Testing ListPrev() from current beyond the end of list.....................\n");
	list1->beyond = true;
	list1->current = NULL;
	check = ListPrev(list1);
	if (check != item[5]){
		printf("ListPrev()1 failed...\n");
		return -1;
	}
	printf("Printing the returned item: %f\n", *(float*)check);
	check = ListPrev(list1);
	if (check != item[4]){
		printf("ListPrev()2 failed...\n");
		return -1;
	}
	printf("Printing the returned item: %d\n", *(int*)check);	
	check = ListPrev(list1);
	if (check != item[3]){
		printf("ListPrev()3 failed...\n");
		return -1;
	}
	printf("Printing the returned item: %d\n", *(int*)check);
	check = ListPrev(list1);
	if (check != item[2]){
		printf("ListPrev()4 failed...\n");
		return -1;
	}
	printf("Printing the returned item: %d\n", *(int*)check);
	check = ListPrev(list1);
	if (check != item[1]){
		printf("ListPrev()5 failed...\n");
		return -1;
	}
	printf("Printing the returned item: %s\n", (char*)check);
	check = ListPrev(list1);
	if (check != item[0]){
		printf("ListPrev()6 failed...\n");
		return -1;
	}
	printf("Printing the returned item: %f\n", *(double*)check);
	check = ListPrev(list1);
	if (check != NULL){
		printf("ListPrev()7 failed...\n");
		return -1;
	}
	printf("Printing the returned item: NULL\n");
	check = ListPrev(list1);
	if (check != NULL){
		printf("ListPrev()8 failed...\n");
		return -1;
	}
	printf("Printing the returned item: NULL\n");
	printf("................Testing ListPrev() from current beyond the end of list Done...................\n");
	printf("\n");

	printf("..................Testing ListNext() from current before the start of list....................\n");
	list1->beyond = false;
	list1->current = NULL;
	check = ListNext(list1);
	if (check != item[0]){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: %f\n", *(double*)check);
	check = ListNext(list1);	
	if (check != item[1]){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: %s\n", (char*)check);
	check = ListNext(list1);
	if (check != item[2]){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: %d\n", *(int*)check);
	check = ListNext(list1);
	if (check != item[3]){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: %d\n", *(int*)check);
	check = ListNext(list1);
	if (check != item[4]){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: %d\n", *(int*)check);
	check = ListNext(list1);
	if (check != item[5]){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: %f\n", *(float*)check);
	check = ListNext(list1);
	if (check != NULL){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: NULL\n");
	check = ListNext(list1);
	if (check != NULL){
		printf("ListNext() failed...\n");
		return -1;
	}
	printf("Printing the returned item: NULL\n");	
	printf("...............Testing ListNext() from current before the start of list Done..................\n\n");

	printf("....................................Testing ListCurr()........................................\n");
	printf("Testing ListCurr() when current is NULL: ");
	if (ListCurr(list1) != NULL){
		printf("ListCurr() failed...\n");
		return -1;
	}
	printf("NULL\n");
	printf("Testing ListCurr() in the empty list: ");
	List *list4 = ListCreate();
	if (ListCurr(list4) != NULL){
		printf("ListCurr() failed...\n");
		return -1;
	}
	printf("NULL\n");
	freeFunc itemFree = StackFree;
	ListFree(list4, itemFree);
	printf("Testing ListCurr() when current is Head: ");
	list1->current = list1->head;
	check = ListCurr(list1);
	if (check != item[0]){
		printf("ListCurr() failed...\n");
		return -1;
	}
	printf("%f\n", *(double*)check);

	printf("..................................Testing ListCurr() End......................................\n\n");


	printf("...................................Testing ListRemove().......................................\n");
	printf("Testing ListRemove() when current is head: ");
	check = ListRemove(list1);
	if (check != item[0]){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	if (list1->current->item != item[1]){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	if (list1->head != list1->current){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	printf("%f returned\n", *(double*)check);
	printf("Testing ListRemove() when current is tail: ");
	list1->current = list1->tail;
	check = ListRemove(list1);
	if (check != item[5]){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	if (list1->current != list1->tail){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	if (list1->tail->item != item[4]){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	printf("%f returned\n", *(float*)check);
	printf("Testing ListRemove() when current points at the second node: ");
	list1->current = list1->head->next;
	check = ListRemove(list1);
	if (check != item[2]){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	if (list1->current->item != item[3]){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	if (list1->current->prev->item != item[1]){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	if (list1->current->next->item != item[4]){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	printf("%d returned\n", *(int*)check);

	printf("Testing ListRemove() when current is NULL: ");
	list1->current = NULL;
	check = ListRemove(list1);
	if (check != NULL){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	printf("NULL returned\n");
	printf("ListRemove() until empty...\n");
	list1->current = list1->head;
	if (ListRemove(list1) != item[1]){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	if (ListRemove(list1) != item[3]){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	if (ListRemove(list1) != item[4]){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	printf("Testing ListRemove() when list is empty: ");
	if (ListRemove(list1) != NULL){
		printf("ListRemove() is not working...\n");
		return -1;
	}
	printf("NULL returned\n");
	if (ListCount(list1) != 0){
		printf("ListRemove() is not properly working...\n");
		return -1;
	}
	printf("The number of nodes: %d\n", ListCount(list1));
	printf(".................................Testing ListRemove() End.....................................\n\n");

	printf("...................................Testing ListInsert().......................................\n");
	printf("Inserting char A+ to an empty list...\n");
	res = ListInsert(list1, item[1]);
	if (res == -1){
		printf("ListInsert() failed...\n");
		return -1;
	}
	if (list1->head->item != item[1]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->tail->item != item[1]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->current->item != item[1]){
		printf("Not properly inserted...\n");
		return -1;
	}
	printf("Inserting double 4.3 when current is before the start...\n");
	list1->current = NULL;
	list1->beyond = false;
	res = ListInsert(list1, item[0]);
	if (res == -1){
		printf("ListInsert() failed...\n");
		return -1;
	}
	if (list1->head->item != item[0]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->current->item != item[0]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->current->next->item != item[1]){
		printf("Not properly inserted...\n%f", *(double*)list1->current->next->item);
		return -1;
	}
	printf("Inserting float 4.3 when current is beyond the end...\n");
	list1->current = NULL;
	list1->beyond = true;
	res = ListInsert(list1, item[5]);
	if (res == -1){
		printf("ListInsert() failed...\n");
		return -1;
	}
	if (list1->tail->item != item[5]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->current->item != item[5]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->current->prev->item != item[1]){
		printf("Not properly inserted...\n");
		return -1;
	}
	printf("Inserting int 1 between nodes...\n");
	res = ListInsert(list1, item[4]);
	if (res == -1){
		printf("ListInsert() failed...\n");
		return -1;
	}
	if (list1->current->item != item[4]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->current->prev->item != item[1]){
		printf("Not properly inserted...\n");
		return -1;
	}
	if (list1->current->next->item != item[5]){
		printf("Not properly inserted...\n");
		return -1;
	}
	printf("Inserting int 5 between nodes...\n");
	res = ListInsert(list1, item[3]);
	if (res == -1){
		printf("ListInsert() failed...\n");
		return -1;
	}
	printf("Inserting int 4 between nodes...\n");
	res = ListInsert(list1, item[2]);
	if (res == -1){
		printf("ListInsert() failed...\n");
		return -1;
	}
	printf("Inserting an item when there is no node: ");
	res = ListInsert(list1, fail);
	if (res == 0){
		printf("ListInsert() failed...\n");
		return -1;
	}
	printf("Items must be in this order: ");
	printf("[ ");
	for (int ii = 0; ii < 6; ii++){
		print(item[ii],type[ii]);
	}
	printf("]\n");
	printf("Printing the current list: ");
	temp = list1->head;
	printf("[ ");
	for (int ii = 0; ii < 6; ii++){
		print(temp->item, type[ii]);
		temp = temp->next;

	}
	printf("]\n");
	printf("Printing backward list1 to make sure that nodes are correctly linked by prev: ");
	temp = list1->tail;
	printf("[ ");
	for (int ii = 5; ii > -1; ii--){
		print(temp->item, type[ii]);
		temp = temp->prev;
	}
	printf("]\n");
	printf(".................................Testing ListInsert() End.....................................\n\n");

	printf("....................................Testing ListFree()........................................\n");
	ListFree(list1, itemFree);
	printf("2 list heads and 6 nodes must be available after calling this function. It will be tested below\n");
	printf("..................................Testing ListFree() End......................................\n\n");

	printf(".....................................Testing ListCreate().........................................\n");
	printf("Creating list1...\n");
	list1 = ListCreate();
	if (list1 == NULL){
		printf("ListCreate() failed...\n");
		return -1;
	}
	printf("Creating list2...\n");
	List *list2 = ListCreate();
	if (list2 == NULL){
		printf("ListCreate() failed...\n");
		return -1;
	}
	if (list1 - list2 == 0){
		printf("ListCreate() is not properly working...\n");
		return -1;
	}
	printf("Attempting to create another list must fail: ");
	List *list3 = ListCreate();
	if (list3 != NULL){
		printf("ListCreate() is not working properly...\n");
		return -1;
	}
	printf("...................................Testing ListCreate() End.......................................\n\n");

	// Testing ListPrepend()
	printf("....................................Testing ListPrepend().........................................\n");
	printf("Prepending int 4 to the empty list1...\n");
	res = ListPrepend(list1, item[2]);
	if (res == -1){
		printf("ListPrepend() failed...\n");
		return -1;
	}
	if (list1->head->item != item[2]){
		printf("ListPrepend() is not properly working...\n");
		return -1;
	}
	printf("Printing the first item: %d\n",*(int*) list1->head->item);
	printf("Prepending char A+ to list1...\n");
	res = ListPrepend(list1, item[1]);
	if (res == -1){
		printf("ListPrepend() failed...\n");
		return -1;
	}
	if (list1->head->item != item[1]){
		printf("ListPrepend() is not properly working...\n");
		return -1;
	}
	printf("Printing the first item: %s\n",(char*) list1->head->item);
	printf("Prepending double 4.33 to list1...\n");
	res = ListPrepend(list1, item[0]);
	if (res == -1){
		printf("ListPrepend() failed...\n");
		return -1;
	}
	if (list1->head->item != item[0]){
		printf("ListPrepend() is not properly working...\n");
		return -1;
	}
	printf("Printing the first item: %f\n",*(double*) list1->head->item);
	if (ListCount(list1) != 3){
		printf("Not properly implemented\n");
		return -1;
	}
	printf("\nThe number of nodes in list1: %d\n\n", ListCount(list1));
	printf("Printing forward the current list: ");
	temp = list1->head;
	printf("[ ");
	for (int ii = 0; ii < 3; ii++){
		print(temp->item, type[ii]);
		temp = temp->next;

	}
	printf("]\n");
	printf("Printing backward the current list: ");
	temp = list1->tail;
	printf("[ ");
	for (int ii = 2; ii > -1; ii--){
		print(temp->item, type[ii]);
		temp = temp->prev;

	}
	printf("]\n");

	printf("..................................Testing ListPrepend() End.......................................\n\n");

	// Testing ListAppend()
	printf(".....................................Testing ListAppend().........................................\n");
	printf("Appending int 5 to the empty list2...\n");
	res = ListAppend(list2, item[3]);
	if (res == -1){
		printf("ListAppend() failed...\n");
		return -1;
	}
	if (list2->tail->item != item[3]){
		printf("ListAppend() is not properly working...\n");
		return -1;
	}
	printf("Printing the last item: %d\n",*(int*) list2->tail->item);
	printf("Appending int 1 to list2...\n");
	res = ListAppend(list2, item[4]);
	if (res == -1){
		printf("ListAppend() failed...\n");
		return -1;
	}
	if (list2->tail->item != item[4]){
		printf("ListAppend() is not properly working...\n");
		return -1;
	}
	printf("Printing the last item: %d\n",*(int*) list2->tail->item);
	printf("Appending float 4.33 to list2...\n");
	res = ListAppend(list2, item[5]);
	if (res == -1){
		printf("ListAppend() failed...\n");
		return -1;
	}
	if (list2->tail->item != item[5]){
		printf("ListAppend() is not properly working...\n");
		return -1;
	}
	printf("Printing the last item: %f\n",*(float*) list2->tail->item);
	if (ListCount(list2) != 3){
		printf("Not properly implemented\n");
		return -1;
	}
	printf("\nThe number of nodes in list2: %d\n\n", ListCount(list2));

	printf("Printing forward the current list: ");
	temp = list2->head;
	printf("[ ");
	for (int ii = 3; ii < 6; ii++){
		print(temp->item, type[ii]);
		temp = temp->next;

	}
	printf("]\n");
	printf("Printing backward the current list: ");
	temp = list2->tail;
	printf("[ ");
	for (int ii = 5; ii > 2; ii--){
		print(temp->item, type[ii]);
		temp = temp->prev;

	}
	printf("]\n");
	printf("...................................Testing ListAppend() End.......................................\n\n");

	// Testing ListConcat
	printf(".....................................Testing ListConcat().........................................\n");
	printf("Concatenating two existing lists...\n");
	ListConcat(list1, list2);
	if (list1->count != 6){
		printf("ListConcat() is not properly working...\n");
		return -1;
	}
	printf("Printing forward list1:  ");
	temp = list1->head;
	printf("[ ");
	for (int ii = 0; ii < 6; ii++){
		print(temp->item, type[ii]);
		temp = temp->next;
	}
	printf("]\n");

	printf("Printing backward list1:  ");
	temp = list1->tail;
	printf("[ ");
	for (int ii = 5; ii > -1; ii--){
		print(temp->item, type[ii]);
		temp = temp->prev;
	}
	printf("]\n");

	list2 = ListCreate();
	if (list2 == NULL){
		printf("ListConcat() did not free up list2...\n");
		return -1;
	}

	printf("Concatenating empty list1 and list2...\n");
	ListConcat(list2, list1);
	if (list2->current != NULL){
		printf("Not working...\n");
		return -1;
	}
	printf("Printing forward list:  ");
	temp = list2->head;
	printf("[ ");
	for (int ii = 0; ii < 6; ii++){
		print(temp->item, type[ii]);
		temp = temp->next;
	}
	printf("]\n");

	list1 = ListCreate();
	if (list1 == NULL){
		printf("ListConcat() did not free up list2...\n");
		return -1;
	}
	printf("Concatenating list1 and empty list2...\n");
	ListConcat(list2, list1);
	printf("Printing forward list:  ");
	temp = list2->head;
	printf("[ ");
	for (int ii = 0; ii < 6; ii++){
		print(temp->item, type[ii]);
		temp = temp->next;
	}
	printf("]\n");
	list1 = list2;
	printf("\nThe number of nodes in list1: %d\n\n", ListCount(list1));
	printf("...................................Testing ListConcat() End.......................................\n\n");

	// Testing ListSearch
	printf(".....................................Testing ListSearch().........................................\n");

	printf("Searching from list head...\n");
	list1->current = list1->head;

	compFunc comparator = CompDouble;
	check = ListSearch(list1, comparator, item[0]);
	if (check != item[0]){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	if (list1->current->item != item[0]){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	printf("Searching 4.33: %f returned\n", *(double*)check);
	comparator = CompString;
	check = ListSearch(list1, comparator, item[1]);
	if (check != item[1]){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	if (list1->current->item != item[1]){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	printf("Searching A+: %s returned\n", (char*)check);
	comparator = CompInt;
	check = ListSearch(list1, comparator, item[2]);
	if (check != item[2]){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	if (list1->current->item != item[2]){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	printf("Searching 4: %d returned\n", *(int*)check);
	printf("Searching 44: ");
	check = ListSearch(list1, comparator, &g);
	if (check != NULL){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	if (list1->current == NULL && !list1->beyond){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	printf("NULL returned\n");
	printf("Searching 4.33 when current is NULL: ");
	check = ListSearch(list1, comparator, item[0]);
	if (check != NULL){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	printf("NULL returned\n");
	printf("Searching 4.33 in an empty list: ");

	list2 = ListCreate();
	check = ListSearch(list2, comparator, item[0]);
	if (check != NULL){
		printf("ListSearch() is not properly working...\n");
		return -1;
	}
	printf("NULL returned\n");
	printf("...................................Testing ListSearch() End.......................................\n\n");
	// Testing ListTrim()
	printf("......................................Testing ListTrim()..........................................\n");
	printf("Trimming until empty...\n");
	check= ListTrim(list1);
	if (check != item[5]){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	if (list1->tail->item != item[4]){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	if (list1->current != list1->tail){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	printf("Returned value: %f\n", *(float*)check);
	check= ListTrim(list1);
	if (check != item[4]){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	printf("Returned value: %d\n", *(int*)check);
	check= ListTrim(list1);
	if (check != item[3]){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	printf("Returned value: %d\n", *(int*)check);
	check= ListTrim(list1);
	if (check != item[2]){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	printf("Returned value: %d\n", *(int*)check);
	check= ListTrim(list1);
	if (check != item[1]){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	printf("Returned value: %s\n", (char*)check);
	check= ListTrim(list1);
	if (check != item[0]){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	printf("Returned value: %f\n", *(double*)check);
	printf("Trimming an empty list: ");
	check= ListTrim(list1);
	if (check != NULL){
		printf("ListTrim() is not properly working...\n");
		return -1;
	}
	printf("NULL returned\n");

	printf("....................................Testing ListTrim() End........................................\n\n");
	printf("...................................All functions passed Test......................................\n\n");
	printf("Thank you!!!!!!!!!!!!!!!!!!!!\n");
	free(item);
	return 0;
}



