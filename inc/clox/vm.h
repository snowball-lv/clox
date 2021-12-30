#pragma once
#include <clox/chunk.h>
#include <clox/table.h>
#include <clox/object.h>

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
    ObjClosure *closure;
    uint8_t *ip;
    Value *slots;
} CallFrame;

typedef struct {
    CallFrame frames[FRAMES_MAX];
    int frameCount;
    Value stack[STACK_MAX];
    Value *stackTop;
    Table globals;
    Table strings;
    ObjString *initString;
    ObjUpvalue *openUpvalues;
    size_t bytesAllocated;
    size_t nextGc;
    Obj *objects;
    int grayCount;
    int grayCapacity;
    Obj **grayStack;
} Vm;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern Vm vm;

void initVm();
void freeVm();
InterpretResult interpret(const char *source);
void push(Value value);
Value pop();
