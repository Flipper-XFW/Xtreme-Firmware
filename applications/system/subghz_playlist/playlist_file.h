#pragma once

#include <furi.h>

#include <storage/storage.h>

#define PLAYLIST_FOLDER EXT_PATH("subghz/playlist")
#define PLAYLIST_EXT ".txt"

int playlist_count_playlist_items(Storage* storage, const char* file_path);
