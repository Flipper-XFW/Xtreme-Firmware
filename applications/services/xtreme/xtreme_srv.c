#include "settings.h"
#include "assets.h"

int32_t xtreme_srv(void* p) {
    UNUSED(p);

    XTREME_SETTINGS_LOAD();
    XTREME_ASSETS_LOAD();

    return 0;
}
