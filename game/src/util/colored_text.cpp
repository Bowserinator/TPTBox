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
                tint = text_format::C_COLORS[codepoint - 1];
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

Vector2 text_format::measure_text_ex(Font font, const char *text, float fontSize, float spacing) {
    Vector2 textSize = { 0 };

    if ((font.texture.id == 0) || (text == NULL)) return textSize;

    int size = TextLength(text); // Get size in bytes of text
    int tempByteCounter = 0;     // Used to count longer text line num chars
    int byteCounter = 0;

    float textWidth = 0.0f;
    float tempTextWidth = 0.0f; // Used to count longer text line width

    float textHeight = fontSize;
    float scaleFactor = fontSize/(float)font.baseSize;

    int letter = 0; // Current character
    int index = 0;  // Index position in sprite font
    bool prevSpecialEscape = false;
    int next = 0;

    for (int i = 0; i < size; i += next) {
        byteCounter++;

        next = 0;
        letter = GetCodepointNext(&text[i], &next);
        index = GetGlyphIndex(font, letter);

        if (letter == text_format::C_ESCAPE) {
            prevSpecialEscape = true;
            continue;
        } else if (prevSpecialEscape) {
            prevSpecialEscape = false;
            continue;
        }

        if (font.glyphs[index].advanceX != 0) textWidth += font.glyphs[index].advanceX;
        else textWidth += (font.recs[index].width + font.glyphs[index].offsetX);
        if (tempByteCounter < byteCounter) tempByteCounter = byteCounter;
    }

    if (tempTextWidth < textWidth) tempTextWidth = textWidth;
    textSize.x = tempTextWidth*scaleFactor + (float)((tempByteCounter - 1)*spacing);
    textSize.y = textHeight;

    return textSize;
}
