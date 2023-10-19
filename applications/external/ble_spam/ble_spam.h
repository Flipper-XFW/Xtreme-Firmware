#pragma once

#include <gui/view_dispatcher.h>

#include "scenes/_setup.h"

enum {
    ViewMain,
};

typedef struct {
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
} Ctx;
