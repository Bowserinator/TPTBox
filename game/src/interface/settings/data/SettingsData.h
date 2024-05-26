#ifndef SETTINGS_DATA_H
#define SETTINGS_DATA_H

#include "../../../util/json.hpp"
#include "GraphicsSettingsData.h"
#include "SimSettingsData.h"
#include "UISettingsData.h"

#include <fstream>

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

        void load_settings_from_file() {
            nlohmann::json j;
            std::ifstream i("powder.json");
            if (i) {
                i >> j;
                graphics->loadFromJSON(j);
                sim->loadFromJSON(j);
                ui->loadFromJSON(j);
            }
        }

        void save_settings_to_file() const {
            nlohmann::json j;
            graphics->writeToJSON(j);
            sim->writeToJSON(j);
            ui->writeToJSON(j);

            std::ofstream o("powder.json");
            o << std::setw(4) << j << std::endl;
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
                single->load_settings_from_file();
            }
            return single;
        };
    private:
        inline static data * single;
    };
}

#endif