#pragma once

#include "render/Renderer.h"
#include "render/camera/camera.h"
#include "simulation/Simulation.h"

#include "interface/brush/Brush.h"
#include "interface/brush/Preview.h"
#include "interface/hud/HUD.h"
#include "interface/settings/data/SettingsData.h"
#include "interface/sim/SimUI.h"

// Try to avoid accessing these directly with globals and pass whenever possible
// Global access is mostly for UI and HUD
inline RenderCamera render_camera;
inline Simulation sim;
inline BrushRenderer brush_renderer(&sim, &render_camera);
inline Renderer renderer(&sim, &render_camera);
inline HUD hud(&sim, &render_camera, &renderer);
inline SimUI sim_ui(&brush_renderer, &renderer, &sim);
