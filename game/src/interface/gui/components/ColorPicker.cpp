#include "ColorPicker.h"
#include "Window.h"
#include "TextButton.h"
#include "TextInput.h"
#include "Label.h"
#include "../Scene.h"

#include "../../EventConsumer.h"
#include "../../../util/vector_op.h"
#include "../../../util/graphics.h"

#include <format>
#include "rlgl.h"

using namespace ui;

// Window for picking the actual color - temporarily added
// to the parent scene
class ColorPickerWindow : public Window {
public:
    ColorPickerWindow(const Vector2 &pos, const Vector2 &size, ColorPicker * colorPicker, Color initialColor, bool hasAlpha):
            Window(pos, size, Window::Settings {
                .draggable = true,
                .title = "Color Picker"
            }, Style::getDefault()), colorPicker(colorPicker), initialColor(initialColor), hasAlpha(hasAlpha) {

        if (!IsTextureReady(alphaBg)) {
            auto bg = GenImageChecked(HUE_RECT_WIDTH, COLOR_PICKER_RECT_HEIGHT,
                HUE_RECT_WIDTH / 2, HUE_RECT_WIDTH / 2,
                Color{70,70,70,255}, BLACK);
            alphaBg = LoadTextureFromImage(bg);
            UnloadImage(bg);
        }
        if (!IsShaderReady(colorRectShader)) {
            colorRectShader = LoadShaderFromMemory(nullptr, R"(
#version 430
in vec2 fragTexCoord;
uniform vec3 hueColor;

void main() {
    vec4 x1 = mix(vec4(1.0), vec4(hueColor, 1.0), fragTexCoord.x);
    gl_FragColor = mix(x1, vec4(0.0, 0.0, 0.0, 1.0), fragTexCoord.y);
}
            )");
            hueColorLoc = GetShaderLocation(colorRectShader, "hueColor");
        }

        if (!IsShaderReady(hueRectShader)) {
            hueRectShader  = LoadShaderFromMemory(nullptr, R"(
#version 430
in vec2 fragTexCoord;

vec3 hueWithMaxSVToRGB(float hue) {
    hue *= 360.0;
    float X = 1.0 - abs(mod(hue / 60.0, 2.0) - 1.0);
    if (hue < 60.0) return vec3(1.0, X, 0.0);
    else if (hue < 120.0) return vec3(X, 1.0, 0.0);
    else if (hue < 180.0) return vec3(0, 1.0, X);
    else if (hue < 240.0) return vec3(0, X, 1.0);
    else if (hue < 300.0) return vec3(X, 0, 1.0);
    return vec3(1.0, 0, X);
}
void main() {gl_FragColor = vec4(hueWithMaxSVToRGB(fragTexCoord.y), 1.0); }
            )");
        }

        constexpr Vector2 SMALL_INPUT_SIZE { 40.0f, 25.0f };
        auto makeRGBAInput = [this, SMALL_INPUT_SIZE](int idx, Vector2 pos) -> ui::TextInput * {
            auto r = new TextInput(pos, SMALL_INPUT_SIZE);
            r = r->setMaxLength(3)
                ->setInputAllowed([](const std::string &c) { return c.length() == 1 && isdigit(c[0]); })
                ->setInputValidation([](const std::string &s) {
                    if (!s.length()) return false;
                    for (auto c : s)
                        if (!isdigit(c))
                            return false;
                    int val = std::stoi(s);     
                    return val >= 0 && val <= 255;
                })
                ->setOnValueChange([this, r, idx](const std::string &newVal) {
                    if (r->isInputValid()) {
                        if (idx == 0) finalColor.r = std::stoi(r->getValue());
                        else if (idx == 1) finalColor.g = std::stoi(r->getValue());
                        else if (idx == 2) finalColor.b = std::stoi(r->getValue());
                        else if (idx == 3) finalColor.a = std::stoi(r->getValue());
                        updateAllValuesFromColor(true);
                    }
                });
            return r;
        };

        rInput = makeRGBAInput(0, Vector2{ PAD, PAD * 2 + COLOR_PICKER_RECT_HEIGHT });
        gInput = makeRGBAInput(1, Vector2{ PAD + (SMALL_INPUT_SIZE.x + 5.0f), PAD * 2 + COLOR_PICKER_RECT_HEIGHT });
        bInput = makeRGBAInput(2, Vector2{ PAD + 2 * (SMALL_INPUT_SIZE.x + 5.0f), PAD * 2 + COLOR_PICKER_RECT_HEIGHT });
        aInput = makeRGBAInput(3, Vector2{ PAD + 3 * (SMALL_INPUT_SIZE.x + 5.0f), PAD * 2 + COLOR_PICKER_RECT_HEIGHT });

        if (!hasAlpha)
            aInput->setReadOnly(true);

        panel->addChild(rInput);
        panel->addChild(gInput);
        panel->addChild(bInput);
        panel->addChild(aInput);
        panel->addChild(new Label(Vector2{ PAD, PAD * 2 + COLOR_PICKER_RECT_HEIGHT + SMALL_INPUT_SIZE.y },
            Vector2{ SMALL_INPUT_SIZE.x * 4 + 5.0f * 3, SMALL_INPUT_SIZE.y }, "RGBA",
            Style { .horizontalAlign = Style::Align::Center }));

        hexInput = (new TextInput(
            Vector2{ PAD + 4 * (SMALL_INPUT_SIZE.x + 5.0f), PAD * 2 + COLOR_PICKER_RECT_HEIGHT },
            Vector2{ SMALL_INPUT_SIZE.x * 4, SMALL_INPUT_SIZE.y }))
                ->setMaxLength(8)
                ->setInputAllowed([](const std::string &c) { return c.length() == 1 && isxdigit(c[0]); })
                ->setInputValidation([](const std::string &s) {
                    if (s.length() != 8) return false;
                    for (auto c : s)
                        if (!isxdigit(c))
                            return false;
                    unsigned int val = static_cast<unsigned int>(std::stoul(s, nullptr, 16));    
                    return val >= 0 && val <= 0xFFFFFFFF;
                })
                ->setOnValueChange([this](const std::string &newVal) {
                    if (hexInput->isInputValid()) {
                        finalColor = GetColor(static_cast<unsigned int>(std::stoul(newVal, nullptr, 16)));
                        updateAllValuesFromColor(true);
                    }
                });

        panel->addChild(hexInput);
        panel->addChild(new Label(Vector2{ PAD + 4 * (SMALL_INPUT_SIZE.x + 5.0f), PAD * 2 + COLOR_PICKER_RECT_HEIGHT + SMALL_INPUT_SIZE.y },
            Vector2{ SMALL_INPUT_SIZE.x * 4, SMALL_INPUT_SIZE.y }, "Hex (#RGBA)",
            Style { .horizontalAlign = Style::Align::Center }));

        addChild((new TextButton(
            Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x * 2 / 3, styles::SETTINGS_BUTTON_HEIGHT },
            "Cancel"
        ))->setClickCallback([this]() { tryClose(ui::Window::CloseReason::BUTTON); }));

        addChild((new TextButton(
            Vector2{ size.x * 2 / 3, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x / 3, styles::SETTINGS_BUTTON_HEIGHT },
            "Ok"
        ))->setClickCallback([this, colorPicker]() {
            colorPicker->setValue(finalColor);
            tryClose(ui::Window::CloseReason::BUTTON);
        }));

        finalColor = initialColor;
        updateAllValuesFromColor(true);
    }

    virtual void draw(const Vector2 &screenPos) override {
        Window::draw(screenPos);

        Vector2 topLeft = { screenPos.x + PAD, screenPos.y + settings.headerHeight + PAD };
        Vector2 bottomRight = {
            screenPos.x + PAD + COLOR_PICKER_RECT_WIDTH,
            screenPos.y + settings.headerHeight + PAD + COLOR_PICKER_RECT_HEIGHT };
        
        // Big box
        BeginShaderMode(colorRectShader);
        rlBegin(RL_QUADS);
            rlNormal3f(0.0f, 0.0f, 1.0f);
            rlTexCoord2f(0.0f, 0.0f); rlVertex2f(topLeft.x, topLeft.y);
            rlTexCoord2f(0.0f, 1.0f); rlVertex2f(topLeft.x, bottomRight.y);
            rlTexCoord2f(1.0f, 1.0f); rlVertex2f(bottomRight.x, bottomRight.y);
            rlTexCoord2f(1.0f, 0.0f); rlVertex2f(bottomRight.x, topLeft.y);
        rlEnd();
        EndShaderMode();

        DrawRectangleLines(topLeft.x, topLeft.y, COLOR_PICKER_RECT_WIDTH, COLOR_PICKER_RECT_HEIGHT, WHITE);
        DrawCircleLines(colorX + PAD + screenPos.x, colorY + PAD + screenPos.y + settings.headerHeight, 6.0f, WHITE);

        // Hue strip
        topLeft = { screenPos.x + 2 * PAD + COLOR_PICKER_RECT_WIDTH, screenPos.y + settings.headerHeight + PAD };
        bottomRight = {
            screenPos.x + 2 * PAD + COLOR_PICKER_RECT_WIDTH + HUE_RECT_WIDTH,
            screenPos.y + settings.headerHeight + PAD + COLOR_PICKER_RECT_HEIGHT };
        
        BeginShaderMode(hueRectShader);
        rlBegin(RL_QUADS);
            rlNormal3f(0.0f, 0.0f, 1.0f);
            rlTexCoord2f(0.0f, 0.0f); rlVertex2f(topLeft.x, topLeft.y);
            rlTexCoord2f(0.0f, 1.0f); rlVertex2f(topLeft.x, bottomRight.y);
            rlTexCoord2f(1.0f, 1.0f); rlVertex2f(bottomRight.x, bottomRight.y);
            rlTexCoord2f(1.0f, 0.0f); rlVertex2f(bottomRight.x, topLeft.y);
        rlEnd();
        EndShaderMode();

        DrawRectangleLines(topLeft.x, topLeft.y, HUE_RECT_WIDTH, COLOR_PICKER_RECT_HEIGHT, WHITE);
        DrawCircle(topLeft.x + HUE_RECT_WIDTH / 2, hueY + PAD + screenPos.y + settings.headerHeight, 8.0f, pureHueColor);
        DrawCircleLines(topLeft.x + HUE_RECT_WIDTH / 2, hueY + PAD + screenPos.y + settings.headerHeight, 8.0f, WHITE);

        // Alpha strip
        if (hasAlpha) {
            topLeft = { screenPos.x + 3 * PAD + + HUE_RECT_WIDTH + COLOR_PICKER_RECT_WIDTH, screenPos.y + settings.headerHeight + PAD };
            DrawTexture(alphaBg, topLeft.x, topLeft.y, WHITE);
            DrawRectangleGradientV(topLeft.x, topLeft.y, HUE_RECT_WIDTH, COLOR_PICKER_RECT_HEIGHT, WHITE, Color{0, 0, 0, 0});
            DrawRectangleLines(topLeft.x, topLeft.y, HUE_RECT_WIDTH, COLOR_PICKER_RECT_HEIGHT, WHITE);

            DrawCircle(topLeft.x + HUE_RECT_WIDTH / 2, alphaY + PAD + screenPos.y + settings.headerHeight, 8.0f, GRAY);
            DrawCircleLines(topLeft.x + HUE_RECT_WIDTH / 2, alphaY + PAD + screenPos.y + settings.headerHeight, 8.0f, WHITE);
        }

        // Color preview
        DrawRectangle(screenPos.x + size.x - PAD - PREVIEW_RECT_WIDTH, screenPos.y + settings.headerHeight + PAD,
            PREVIEW_RECT_WIDTH, PREVIEW_RECT_HEIGHT, finalColor);
        DrawRectangleLines(screenPos.x + size.x - PAD - PREVIEW_RECT_WIDTH, screenPos.y + settings.headerHeight + PAD,
            PREVIEW_RECT_WIDTH, PREVIEW_RECT_HEIGHT, WHITE);
        DrawRectangle(screenPos.x + size.x - PAD - PREVIEW_RECT_WIDTH, screenPos.y + settings.headerHeight + PAD + PREVIEW_RECT_HEIGHT,
            PREVIEW_RECT_WIDTH, PREVIEW_RECT_HEIGHT, initialColor);
        DrawRectangleLines(screenPos.x + size.x - PAD - PREVIEW_RECT_WIDTH, screenPos.y + settings.headerHeight + PAD + PREVIEW_RECT_HEIGHT,
            PREVIEW_RECT_WIDTH, PREVIEW_RECT_HEIGHT, WHITE);
    }

    void onMouseClick(Vector2 localPos, unsigned button) override {
        Window::onMouseClick(localPos, button);

        Vector2 rectPos = { localPos.x - PAD, localPos.y - PAD - settings.headerHeight };
        if (rectPos.x >= 0.0f && rectPos.x <= COLOR_PICKER_RECT_WIDTH && rectPos.y >= 0.0f && rectPos.y <= COLOR_PICKER_RECT_HEIGHT)
            draggingMainRect = true;
        else if (posInHueBar(localPos))
            draggingHueRect = true;
        else if (posInAlphaBar(localPos))
            draggingAlphaRect = true;
    }

    virtual void onMouseWheelInside(Vector2 localPos, float d) {
        Window::onMouseWheelInside(localPos, d);
        if (posInHueBar(localPos)) {
            hueY = util::clamp(hueY - 3.0f * d, 0.0f, COLOR_PICKER_RECT_HEIGHT);
            updateColors(false);
        } else if (posInAlphaBar(localPos)) {
            alphaY = util::clamp(alphaY - 3.0f * d, 0.0f, COLOR_PICKER_RECT_HEIGHT);
            updateColors(false);
        }
    }

    void onMouseRelease(Vector2 localPos, unsigned int button) override {
        Window::onMouseRelease(localPos, button);
        draggingMainRect = false;
        draggingHueRect = false;
        draggingAlphaRect = false;
    }

    void onMouseMoved(Vector2 localPos) override {
        Window::onMouseMoved(localPos);
        if (EventConsumer::ref()->isMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 rectPos = { localPos.x - PAD, localPos.y - PAD - settings.headerHeight };
            if (draggingMainRect) {
                colorX = util::clamp(rectPos.x, 0.0f, COLOR_PICKER_RECT_WIDTH);
                colorY = util::clamp(rectPos.y, 0.0f, COLOR_PICKER_RECT_HEIGHT);
                updateColors(false);
            } else if (draggingHueRect) {
                hueY = util::clamp(rectPos.y, 0.0f, COLOR_PICKER_RECT_HEIGHT);
                updateColors(false);
            } else if (hasAlpha && draggingAlphaRect) {
                alphaY = util::clamp(rectPos.y, 0.0f, COLOR_PICKER_RECT_HEIGHT);
                updateColors(false);
            }
        }
    }

    void onUnfocus() override {
        Window::onUnfocus();
        //removeFromParent(this);
    }
private:
    inline static Shader colorRectShader = {0};
    inline static Shader hueRectShader = {0};
    inline static int hueColorLoc = 0;
    inline static Texture2D alphaBg = {0};

    constexpr static float COLOR_PICKER_RECT_WIDTH = 300.0f;
    constexpr static float COLOR_PICKER_RECT_HEIGHT = 200.0f;
    constexpr static float HUE_RECT_WIDTH = 10.0f;
    constexpr static float PREVIEW_RECT_WIDTH = 80.0f;
    constexpr static float PREVIEW_RECT_HEIGHT = 50.0f;
    constexpr static float PAD = 15.0f;

    ColorPicker * colorPicker = nullptr;
    float colorX = 0.0f;
    float colorY = 0.0f;
    float hueY = 0.0f;
    float alphaY = 0.0f;

    bool draggingMainRect = false;
    bool draggingHueRect = false;
    bool draggingAlphaRect = false;

    Color initialColor = RED;
    Color pureHueColor = RED; // Current selected color with S=V=1
    Color finalColor = RED;   // Actual current selected color
    bool hasAlpha = true;

    ui::TextInput
        * rInput,
        * gInput,
        * bInput,
        * aInput,
        * hexInput;

    bool posInHueBar(const Vector2 localPos) const {
        return localPos.x >= 2 * PAD + COLOR_PICKER_RECT_WIDTH &&
                localPos.x <= 2 * PAD + COLOR_PICKER_RECT_WIDTH + HUE_RECT_WIDTH &&
                localPos.y >= settings.headerHeight + PAD &&
                localPos.y <= settings.headerHeight + PAD + COLOR_PICKER_RECT_HEIGHT;
    }

    bool posInAlphaBar(const Vector2 localPos) const {
        return localPos.x >= 3 * PAD + COLOR_PICKER_RECT_WIDTH + HUE_RECT_WIDTH &&
                localPos.x <= 3 * PAD + COLOR_PICKER_RECT_WIDTH + 2 * HUE_RECT_WIDTH &&
                localPos.y >= settings.headerHeight + PAD &&
                localPos.y <= settings.headerHeight + PAD + COLOR_PICKER_RECT_HEIGHT;
    }

    void updateColors(bool isTextboxEdit) {
        // Update hue --------
        const float hue = hueY / COLOR_PICKER_RECT_HEIGHT * 360.0f;
        const float X = 1.0 - abs(fmod(hue / 60.0, 2.0) - 1.0);
        auto setHueColor = [this](float r, float g, float b) {
            pureHueColor = Color{
                static_cast<unsigned char>(r * 255),
                static_cast<unsigned char>(g * 255),
                static_cast<unsigned char>(b * 255), 255 };
        };

        if (hue < 60.0) setHueColor(1.0, X, 0.0);
        else if (hue < 120.0) setHueColor(X, 1.0, 0.0);
        else if (hue < 180.0) setHueColor(0, 1.0, X);
        else if (hue < 240.0) setHueColor(0, X, 1.0);
        else if (hue < 300.0) setHueColor(X, 0, 1.0);
        else setHueColor(1.0, 0, X);

        // Get actual color -----------
        const float ix = colorX / COLOR_PICKER_RECT_WIDTH;  // Interpolation variables
        const float iy = colorY / COLOR_PICKER_RECT_HEIGHT;

        const Vector3 hueColorV = { (float)pureHueColor.r, (float)pureHueColor.g, (float)pureHueColor.b };
        constexpr Vector3 WHITE_V = Vector3{255.0f, 255.0f, 255.0f};
        constexpr Vector3 BLACK_V = Vector3{0.0f, 0.0f, 0.0f};
        const Vector3 x1 = WHITE_V + ix * (hueColorV - WHITE_V);
        const Vector3 finalColorV = x1 + (BLACK_V - x1) * iy;
        if (!hasAlpha) alphaY = 0.0f;

        finalColor = Color{
                static_cast<unsigned char>(finalColorV.x),
                static_cast<unsigned char>(finalColorV.y),
                static_cast<unsigned char>(finalColorV.z),
                static_cast<unsigned char>(255.0 - alphaY / COLOR_PICKER_RECT_HEIGHT * 255.0)};
        updateAllValuesFromColor(isTextboxEdit);
    }

    void updateAllValuesFromColor(bool isTextboxEdit) {
        if (!hasAlpha) finalColor.a = 255;

        auto rStr = std::to_string(finalColor.r);
        auto gStr = std::to_string(finalColor.g);
        auto bStr = std::to_string(finalColor.b);
        auto aStr = std::to_string(finalColor.a);
        auto hexStr = std::format("{:X}", (unsigned int)ColorToInt(finalColor));
        if (hexStr.length() < 8)
            hexStr = std::string(8 - hexStr.length(), '0') + hexStr;

        if (rInput->getValue() != rStr)
            rInput->setValue(rStr, !isTextboxEdit);
        if (gInput->getValue() != gStr)
            gInput->setValue(gStr, !isTextboxEdit);
        if (bInput->getValue() != bStr)
            bInput->setValue(bStr, !isTextboxEdit);
        if (aInput->getValue() != aStr)
            aInput->setValue(aStr, !isTextboxEdit);
        if (hexInput->getValue() != hexStr)
            hexInput->setValue(hexStr, !isTextboxEdit);

        Vector3 hsv = ColorToHSV(finalColor);
        colorX = hsv.y * COLOR_PICKER_RECT_WIDTH; // Saturation
        colorY = (1.0f - hsv.z) * COLOR_PICKER_RECT_HEIGHT; // Value

        if (isTextboxEdit) { hueY = hsv.x / 360.0f * COLOR_PICKER_RECT_HEIGHT; }
        else { hsv.x = hueY / COLOR_PICKER_RECT_HEIGHT * 360.0f; }

        alphaY = (1.0f - finalColor.a / 255.0f) * COLOR_PICKER_RECT_HEIGHT;

        hsv.y = hsv.z = 1.0f;
        pureHueColor = ColorFromHSV(hsv.x, hsv.y, hsv.z);
        util::set_shader_value(colorRectShader, hueColorLoc, Vector3{ pureHueColor.r / 255.0f, pureHueColor.g / 255.0f, pureHueColor.b / 255.0f });
    }
};

void ColorPicker::draw(const Vector2 &screenPos) {
    DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
    DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
        style.borderThickness,
        style.getBorderColor(this));

    constexpr float PAD = 5.0f;
    DrawRectangle(screenPos.x + PAD, screenPos.y + PAD, size.x - 2 * PAD, size.y - 2 * PAD, value);
}

void ColorPicker::onMouseClick(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseClick(localPos, button);
    parentScene->addChild(new ColorPickerWindow(
        Vector2{ GetScreenWidth() / 2 - 250.0f, GetScreenHeight() / 2 - 150.0f },
        Vector2{500.0f, 370.0f}, this, value, hasAlpha));
}
