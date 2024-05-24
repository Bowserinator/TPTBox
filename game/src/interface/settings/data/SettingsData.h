#ifndef SETTINGS_DATA_H
#define SETTINGS_DATA_H

#include "GraphicsSettingsData.h"
#include "SimSettingsData.h"
#include "UISettingsData.h"

namespace settings {
    class data {
    public:
        data() {}
        data(data&other) = delete;
        void operator=(const data&) = delete;
        ~data() {
            delete graphics;
            delete sim;
            delete ui;
            graphics = nullptr;
            sim = nullptr;
            ui = nullptr;
        }

        Graphics * graphics = nullptr;
        Sim * sim = nullptr;
        UI * ui = nullptr;

        static data * ref() {
            if (single == nullptr) { [[unlikely]]
                single = new data;
                single->graphics = new Graphics;
                single->sim = new Sim;
                single->ui = new UI;
            }
            return single;
        };
    private:
        inline static data * single;
    };
}

#endif