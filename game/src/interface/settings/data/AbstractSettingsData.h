#ifndef ABSTRACT_SETTINGS_DATA_H
#define ABSTRACT_SETTINGS_DATA_H

#include "../../../util/json.hpp"

namespace settings {
    class AbstractSettingsData {
    public:
        virtual ~AbstractSettingsData() {}
        virtual void loadFromJSON(const nlohmann::json &json) = 0;
        virtual void writeToJSON(nlohmann::json &json) const = 0;
    };
}

#endif