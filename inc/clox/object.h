#pragma once
#include <stdint.h>
#include <clox/value.h>
#include <clox/chunk.h>
#include <clox/table.h>

#define IS_BOUND_METHOD(value) isObjType(value, OBJ_BOUND_METHOD)
#define IS_INSTANCE(value) isObjType(value, OBJ_INSTANCE)
#define IS_CLASS(value) isObjType(value, OBJ_CLASS)
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define OBJ_TYPE(value)   (AS_OBJ(value)->type)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_STRING(value)  isObjType(value, OBJ_STRING)
#define AS_FUNCTION(value) ((ObjFunction *) AS_OBJ(value))
#define AS_STRING(value)  ((ObjString *) AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *) AS_OBJ(value))->chars)
#define AS_NATIVE(value) (((ObjNative *) AS_OBJ(value))->function)
#define AS_CLOSURE(value)  ((ObjClosure *) AS_OBJ(value))
#define AS_CLASS(value) ((ObjClass *) AS_OBJ(value))
#define AS_INSTANCE(value) ((ObjInstance *) AS_OBJ(value))
#define AS_BOUND_METHOD(value) ((ObjBoundMethod *) AS_OBJ(value))

typedef enum {
    OBJ_CLASS,
    OBJ_BOUND_METHOD,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_INSTANCE,
} ObjType;

struct Obj {
    ObjType type;
    char isMarked;
    struct Obj *next;
};

typedef struct {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString *name;
} ObjFunction;

typedef struct ObjUpvalue {
    Obj obj;
    Value *location;
    Value closed;
    struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalueCount;
} ObjClosure;

typedef struct {
    Obj obj;
    Value receiver;
    ObjClosure *method;
} ObjBoundMethod;

typedef struct {
    Obj obj;
    ObjString *name;
    Table methods;
} ObjClass;

typedef struct {
    Obj obj;
    ObjClass *klass;
    Table fields;
} ObjInstance;

typedef Value (*NativeFn)(int argCount, Value *args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

struct ObjString {
    Obj obj;
    int length;
    char *chars;
    uint32_t hash;
};

ObjBoundMethod *newBoundMethod(Value receiver, ObjClosure *method);
ObjInstance *newInstance(ObjClass *klass);
ObjClass *newClass(ObjString *name);
ObjUpvalue *newUpvalue(Value *slot);
ObjClosure *newClosure(ObjFunction *function);
ObjNative *newNative(NativeFn function);
ObjFunction *newFunction();
ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
void printObject(Value value);

static inline int isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}
