#include "colored_text.h"

void text_format::draw_text_colored_ex(Font font, const char * text, Vector2 position, float fontSize,
        float spacing, Color tintBase) {
    if (font.texture.id == 0) font = GetFontDefault(); // Security check in case of not valid font

    Color tint = tintBase;
    int size = TextLength(text); // Total size in bytes of the text, scanned by codepoints in loop
    int textOffsetY = 0;       // Offset between lines (on linebreak '\n')
    float textOffsetX = 0.0f;  // Offset X to next character to draw
    float scaleFactor = fontSize / font.baseSize;  // Character quad scaling factor

    bool prevSpecialEscape = false;
    int codepointByteCount = 0;

    for (int i = 0; i < size; i += codepointByteCount) {
        // Get next codepoint from byte string and glyph index in font
        codepointByteCount = 0;
        int codepoint = GetCodepointNext(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        if (codepoint == text_format::C_ESCAPE) {
            prevSpecialEscape = true;
            continue;
        } else if (prevSpecialEscape) {
            prevSpecialEscape = false;
            if (codepoint < sizeof(text_format::C_COLORS) / sizeof(text_format::C_COLORS[0]))
                tint = text_format::C_COLORS[codepoint];
            else if (codepoint == text_format::C_RESET)
                tint = tintBase;
            continue;
        }

        if ((codepoint != ' ') && (codepoint != '\t')) {
            DrawTextCodepoint(font, codepoint,
                Vector2{ position.x + textOffsetX, position.y + textOffsetY },
                fontSize, tint);
        }
        if (font.glyphs[index].advanceX == 0) textOffsetX += ((float)font.recs[index].width * scaleFactor + spacing);
        else textOffsetX += ((float)font.glyphs[index].advanceX * scaleFactor + spacing);
    }
}
