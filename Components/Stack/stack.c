#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"
#include "../types.h"

#define INIT_STACK_CAP 12

struct _stack
{
    size_t length;
    size_t cap;
    WORD* arr;
    WORD top;
};

stack* stack_create()
{
    stack* s = malloc(sizeof(stack));
    s->length = 0;
    s->cap = INIT_STACK_CAP;
    s->arr = malloc(s->cap * sizeof(WORD));
    s->top = 0;
    return s;
}

static void stack_embiggen(stack* s)
{
    s->cap *= 2;
    WORD* new_arr = realloc(s->arr, s->cap * sizeof(WORD));
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

void stack_push(stack* s, WORD val)
{
    if (s->length == s->cap) stack_embiggen(s);
    WORD val_copy = val;
    s->arr[s->length] = val_copy;
    s->top = s->arr[s->length++];
}

WORD stack_peek(stack* s)
{
    return s->top;
}

WORD stack_pop(stack* s)
{
    WORD val = stack_peek(s);
    s->length--;
    if (s->length != 0) s->top = s->arr[s->length - 1];
    return val;
}

void stack_destroy(stack* s)
{
    free(s->arr);
    free(s);
}
