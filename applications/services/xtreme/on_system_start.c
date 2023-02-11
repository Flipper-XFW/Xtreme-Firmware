#include "settings.h"
#include "assets.h"

void xtreme_on_system_start() {
    XTREME_SETTINGS_LOAD();
    XTREME_ASSETS_LOAD();
}
