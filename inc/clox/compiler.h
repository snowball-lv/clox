#pragma once
#include <clox/vm.h>

ObjFunction *compile(const char *source);
void markCompilerRoots();
