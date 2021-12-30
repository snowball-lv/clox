#include <string.h>
#include <clox/table.h>
#include <clox/memory.h>

#define TABLE_MAX_LOAD 0.75

void initTable(Table *table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = 0;
}

void freeTable(Table *table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
    uint32_t index = key->hash & (capacity - 1);
    Entry *tombstone = 0;
    for (;;) {
        Entry *entry = &entries[index];
        if (entry->key == 0) {
            if (IS_NIL(entry->value)) {
                return tombstone != 0 ? tombstone : entry;
            }
            else {
                if (tombstone == 0) tombstone = entry;
            }
        }
        else if (entry->key == key) {
            return entry;
        }
        index = (index + 1) & (capacity - 1);
    }
}

static void adjustCapacity(Table *table, int capacity) {
    Entry *entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = 0;
        entries[i].value = NIL_VAL;
    }
    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        if (entry->key == 0) continue;
        Entry *dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }
    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

int tableSet(Table *table, ObjString *key, Value value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }
    Entry *entry = findEntry(table->entries, table->capacity, key);
    int isNewKey = entry->key == 0;
    if (isNewKey && IS_NIL(entry->value)) table->count++;
    entry->key = key;
    entry->value = value;
    return isNewKey;
}

void tableAddAll(Table *from, Table *to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry *entry = &from->entries[i];
        if (entry->key != 0) {
            tableSet(to, entry->key, entry->value);
        }
    }
}

int tableGet(Table *table, ObjString *key, Value *value) {
    if (table->count == 0) return 0;
    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == 0) return 0;
    *value = entry->value;
    return 1;
}

int tableDelete(Table *table, ObjString *key) {
    if (table->count == 0) return 0;
    Entry *entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == 0) return 0;
    entry->key = 0;
    entry->value = BOOL_VAL(1);
    return 1;
}

ObjString *tableFindString(Table *table, const char *chars, int length,
        uint32_t hash) {
    if (table->count == 0) return 0;
    int32_t index = hash & (table->capacity - 1);
    for (;;) {
        Entry *entry = &table->entries[index];
        if (entry->key == 0) {
            if (IS_NIL(entry->value)) return 0;
        }
        else if (entry->key->length == length
                && entry->key->hash == hash 
                && memcmp(entry->key->chars, chars, length) == 0) {
            return entry->key;
        }
        index = (index + 1) & (table->capacity - 1);
    }
    return 0;
}

void markTable(Table *table) {
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        markObject((Obj *) entry->key);
        markValue(entry->value);
    }
}

void tableRemoveWhite(Table *table) {
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        if (entry->key != 0 && !entry->key->obj.isMarked) {
            tableDelete(table, entry->key);
        }
    }
}
