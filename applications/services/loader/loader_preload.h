#pragma once

#include <storage/storage.h>

void* loader_preload(Storage* storage, const char* path);
void loader_preload_start(void* app, const char* path);
