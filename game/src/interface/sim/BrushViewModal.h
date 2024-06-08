#ifndef INTERFACE_SIM_BRUSHVIEWMODAL_H_
#define INTERFACE_SIM_BRUSHVIEWMODAL_H_

#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"
#include "./mini_panels/ViewPanel.h"
#include "./mini_panels/BrushPanel.h"

class Renderer;
class BrushRenderer;

class BrushViewModal : public ui::Modal {
public:
    BrushViewModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer, BrushRenderer * brushRenderer);
private:
    void switchToView();
    void switchToBrush();

    ui::Button * viewBtn = nullptr;
    ui::Button * brushBtn = nullptr;

    ViewPanel * viewPanel = nullptr;
    BrushPanel * brushPanel = nullptr;

    enum class CurrentPanel { View, Brush, None };
    CurrentPanel currentPanel = CurrentPanel::None;
};

#endif // INTERFACE_SIM_BRUSHVIEWMODAL_H_
