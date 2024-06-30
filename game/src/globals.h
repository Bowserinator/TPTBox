#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "src/render/camera/camera.h"
#include "src/render/Renderer.h"
#include "src/simulation/Simulation.h"

#include "src/interface/hud/HUD.h"
#include "src/interface/sim/SimUI.h"
#include "src/interface/brush/Brush.h"
#include "src/interface/brush/Preview.h"
#include "src/interface/settings/data/SettingsData.h"

// Try to avoid accessing these directly with globals and pass whenever possible
// Global access is mostly for UI and HUD
inline RenderCamera render_camera;
inline Simulation sim;
inline BrushRenderer brush_renderer(&sim, &render_camera);
inline Renderer renderer(&sim, &render_camera);
inline HUD hud(&sim, &render_camera, &renderer);
inline SimUI sim_ui(&brush_renderer, &renderer, &sim);

#endif // GLOBALS_H_
