/* Test-only allocation wrappers; production allocation code is unchanged. */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

static int allocations = 0;
static bool failAllocation = false;
static void *trackedMalloc(size_t size){
    if(failAllocation) return NULL;
    void *pointer = malloc(size);
    if(pointer != NULL) allocations++;
    return pointer;
}
static void trackedFree(void *pointer){
    if(pointer != NULL) allocations--;
    free(pointer);
}
#define malloc trackedMalloc
#define free trackedFree
#define main queueApplicationMain
#include "lab9.c"
#undef main
#undef malloc
#undef free

int main(void){
    Queue list = {NULL};
    char name[101] = "Demo";
    failAllocation = true;
    assert(!addPatient(1, name, 3, &list));
    assert(list.head == NULL && allocations == 0);
    failAllocation = false;
    assert(addPatient(1, name, 3, &list));
    Node *originalHead = list.head;
    failAllocation = true;
    assert(!addPatient(2, name, 5, &list));
    assert(!addPatient(3, name, 1, &list));
    assert(list.head == originalHead && list.head->next == NULL && allocations == 1);
    failAllocation = false;
    assert(addPatient(2, name, 5, &list));
    assert(addPatient(3, name, 3, &list));
    assert(list.head->ID == 2 && list.head->next->ID == 1
           && list.head->next->next->ID == 3);
    assert(!addPatient(1, name, 4, &list) && allocations == 3);
    assert(treatPatient(&list) && allocations == 2);
    assert(removePatient(3, &list) && allocations == 1);
    endProgram(&list);
    assert(list.head == NULL && allocations == 0);
    endProgram(&list);
    assert(allocations == 0);
    return 0;
}
