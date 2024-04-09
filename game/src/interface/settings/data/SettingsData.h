#ifndef SETTINGS_DATA_H
#define SETTINGS_DATA_H

#include "GraphicsSettingsData.h"
#include "SimSettingsData.h"

namespace settings {
    class data {
    public:
        data() {}
        data(data&other) = delete;
        void operator=(const data&) = delete;
        ~data() {
            delete graphics;
            delete sim;
            graphics = nullptr;
            sim = nullptr;
        }

        Graphics * graphics = nullptr;
        Sim * sim = nullptr;

        static data * ref() {
            if (single == nullptr) { [[unlikely]]
                single = new data;
                single->graphics = new Graphics;
                single->sim = new Sim;
            }
            return single;
        };
    private:
        inline static data * single;
    };
}

#endif