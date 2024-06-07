#ifndef INTERFACE_SETTINGS_DATA_ABSTRACTSETTINGSDATA_H_
#define INTERFACE_SETTINGS_DATA_ABSTRACTSETTINGSDATA_H_

#include "../../../util/json.hpp"

namespace settings {
    class AbstractSettingsData {
    public:
        virtual ~AbstractSettingsData() {}
        virtual void loadFromJSON(const nlohmann::json &json) = 0;
        virtual void writeToJSON(nlohmann::json &json) const = 0;
    };
} // namespace settings

#endif // INTERFACE_SETTINGS_DATA_ABSTRACTSETTINGSDATA_H_
