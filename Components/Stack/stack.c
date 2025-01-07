#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"

#define INIT_STACK_CAP 10

struct _stack
{
    size_t length;
    size_t cap;
    void** arr;
    void* top;

    void* (*copy)(const void*);
};

stack* stack_create(void* (*copy)(const void*))
{
    stack* s = malloc(sizeof(stack));
    s->length = 0;
    s->cap = INIT_STACK_CAP;
    s->arr = malloc(s->cap * sizeof(void*));
    s->top = NULL;
    s->copy = copy;
    return s;
}

static void stack_embiggen(stack* s)
{
    s->cap *= 2;
    void** new_arr = realloc(s->arr, s->cap * sizeof(void*));
    // realistically, memory allocation will never fail
    // in my use case. Still, will probably handle later.
    //
    // TODO: Handle memory allocation failure
    s->arr = new_arr;
    s->top = s->arr[s->length - 1];
}

bool stack_is_empty(stack* s)
{
    return (s->length == 0);
}

void stack_push(stack* s, const void* val)
{
    if (s->length == s->cap) stack_embiggen(s);
    void* val_copy = s->copy(val);
    s->arr[s->length] = val_copy;
    s->top = s->arr[s->length++];
}

void* stack_peek(stack* s)
{
    return s->top;
}

void* stack_pop(stack* s)
{
    void* val = stack_peek(s);
    s->length--;
    if (s->length != 0) s->top = s->arr[s->length - 1];
    return val;
}

void stack_destroy(stack* s)
{
    for (size_t i = 0; i < s->length; ++i)
    {
        free(s->arr[i]);
    }
    free(s->arr);
    free(s);
}
