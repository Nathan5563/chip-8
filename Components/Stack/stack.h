#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>

struct _stack;
typedef struct _stack stack;

/**
 * Creates a dynamically allocated stack.
 * 
 * @param copy a pointer to a copy function
 * @return a pointer to a stack
 */
stack* stack_create(void* (*copy)(const void*));

/**
 * Checks if a stack is empty.
 * 
 * @param s a pointer to a stack
 */
bool stack_is_empty(stack* s);

/**
 * Copies and appends the copy of the provided value to the stack.
 * 
 * @param s a pointer to a stack
 * @param val a pointer to a value to push
 */
void stack_push(stack* s, const void* val);

/**
 * Returns the top value on the stack.
 * 
 * @param s a pointer to a stack
 * @return a pointer to the top value
 */
void* stack_peek(stack* s);

/**
 * Returns the top value and removes it from the stack. It is the
 * responsibility of the caller to free the returned value.
 * 
 * @param s a pointer to a stack
 * @return a pointer to the top value
 */
void* stack_pop(stack* s);

/**
 * Frees all memory associated with the stack.
 * 
 * @param s a pointer to a stack
 */
void stack_destroy(stack* s);

#endif