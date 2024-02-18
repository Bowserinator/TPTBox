#ifndef SETTINGS_DATA_H
#define SETTINGS_DATA_H

#include "GraphicsSettingsData.h"

namespace settings {
    class data {
    public:
        data() {}
        data(data&other) = delete;
        void operator=(const data&) = delete;
        ~data() {
            delete graphics;
            graphics = nullptr;
        }

        Graphics * graphics;

        static data * ref() {
            if (single == nullptr) { [[unlikely]]
                single = new data;
                single->graphics = new Graphics;
            }
            return single;
        };
    private:
        inline static data * single;
    };
}

#endif