#include "types.h"

// Global variable used to track raw enable/disable status
int raw_mode_enabled = 0;

void enable_raw_mode() {
    raw_mode_enabled = 1;
    // TODO: may need to put additional non-canonical / raw logic
}

void disable_raw_mode() {
    raw_mode_enabled = 0;
    // TODO: may need to put additional logic here to return the terminal back to norm.
}
