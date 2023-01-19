#include "xtreme_settings.h"
#include "xtreme_assets.h"

void xtreme_on_system_start() {
    XTREME_SETTINGS_LOAD();
    XTREME_ASSETS_UPDATE();
}
