#include "../ElementClasses.h"

#include <iostream>

static int update(UPDATE_FUNC_ARGS);

void Element::Element_GOL() {
    State = ElementState::TYPE_SOLID;
    Color = 0xFF0000AA;

    Update = &update;
};

static int update(UPDATE_FUNC_ARGS) {
    if (sim->frame_count % 2 == 0) { // Collect neighbors phase
        int neighbors = 0;
        for (int dz = -1; dz <= 1; dz++)
        for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            if (!dx && !dz && !dy) continue;
            if (parts[pmap[z + dz][y + dy][x + dx]].type == PT_GOL) {
                neighbors++;
            }
        }
        //std::cout << neighbors << "\n";
        parts[i].life = neighbors;
    } else { // Compute phase
        if (parts[i].life > 16) {
            sim->kill_part(i);
            return -1;
        }
    }
    
    return 0;
}
