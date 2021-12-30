#pragma once
#include <stdint.h>
#include <clox/value.h>

typedef struct {
    ObjString *key;
    Value value;
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry *entries;
} Table;

void initTable(Table *table);
void freeTable(Table *table);
int tableSet(Table *table, ObjString *key, Value value);
void tableAddAll(Table *from, Table *to);
int tableGet(Table *table, ObjString *key, Value *value);
int tableDelete(Table *table, ObjString *key);
ObjString *tableFindString(Table *table, const char *chars, int length,
        uint32_t hash);
void tableRemoveWhite(Table *table);
void markTable(Table *table);
