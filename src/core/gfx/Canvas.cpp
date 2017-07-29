#include "Canvas.h"
#include "Blit.h"

#include <stdio.h>

// Font definitions
#include "fonts/tiny_5x5.h"

static const BitmapFont &bitmapFont(Font font) {
    switch (font) {
    case Font::Tiny: return tiny_5x5;
    default: return tiny_5x5;
    }
}

void Canvas::fill() {
    _frameBuffer.fill(_color);
}

void Canvas::point(int x, int y) {
    switch (_blendMode) {
    case BlendMode::Set: point<blit::set>(x, y); break;
    case BlendMode::Add: point<blit::add>(x, y); break;
    case BlendMode::Sub: point<blit::sub>(x, y); break;    
    }
}

void Canvas::hline(int x, int y, int w) {
    switch (_blendMode) {
    case BlendMode::Set: hline<blit::set>(x, y, w); break;
    case BlendMode::Add: hline<blit::add>(x, y, w); break;
    case BlendMode::Sub: hline<blit::sub>(x, y, w); break;    
    }
}

void Canvas::vline(int x, int y, int h) {
    switch (_blendMode) {
    case BlendMode::Set: vline<blit::set>(x, y, h); break;
    case BlendMode::Add: vline<blit::add>(x, y, h); break;
    case BlendMode::Sub: vline<blit::sub>(x, y, h); break;    
    }
}

void Canvas::line(float x0, float y0, float x1, float y1) {
    switch (_blendMode) {
    case BlendMode::Set: line<blit::set>(x0, y0, x1, y1); break;
    case BlendMode::Add: line<blit::add>(x0, y0, x1, y1); break;
    case BlendMode::Sub: line<blit::sub>(x0, y0, x1, y1); break;    
    }
}

void Canvas::drawRect(int x, int y, int w, int h) {
    switch (_blendMode) {
    case BlendMode::Set: drawRect<blit::set>(x, y, w, h); break;
    case BlendMode::Add: drawRect<blit::add>(x, y, w, h); break;
    case BlendMode::Sub: drawRect<blit::sub>(x, y, w, h); break;    
    }
}

void Canvas::fillRect(int x, int y, int w, int h) {
    switch (_blendMode) {
    case BlendMode::Set: fillRect<blit::set>(x, y, w, h); break;
    case BlendMode::Add: fillRect<blit::add>(x, y, w, h); break;
    case BlendMode::Sub: fillRect<blit::sub>(x, y, w, h); break;    
    }
}

void Canvas::drawBitmap1bit(int x, int y, int w, int h, const uint8_t *bitmap) {
    switch (_blendMode) {
    case BlendMode::Set: drawBitmap<blit::set, 1>(x, y, w, h, bitmap); break;
    case BlendMode::Add: drawBitmap<blit::add, 1>(x, y, w, h, bitmap); break;
    case BlendMode::Sub: drawBitmap<blit::sub, 1>(x, y, w, h, bitmap); break;    
    }
}

void Canvas::drawBitmap4bit(int x, int y, int w, int h, const uint8_t *bitmap) {
    switch (_blendMode) {
    case BlendMode::Set: drawBitmap<blit::set, 4>(x, y, w, h, bitmap); break;
    case BlendMode::Add: drawBitmap<blit::add, 4>(x, y, w, h, bitmap); break;
    case BlendMode::Sub: drawBitmap<blit::sub, 4>(x, y, w, h, bitmap); break;    
    }
}

void Canvas::drawText(int x, int y, const char *str) {
    const auto &font = bitmapFont(_font);

    int ox = x;
    // for (int i = 0; i < 2; ++i) {
    //     char c = str[i];
    while (*str != '\0') {
        auto c = *str++;
        if (c == '\n') {
            x = ox;
            y += font.yAdvance;
            continue;
        }
        if (c < font.first || c > font.last) {
            continue;
        }
        const auto &g = font.glyphs[c - font.first];
        const uint8_t *bitmap = &font.bitmap[g.offset];
        switch (font.bpp) {
        case 1: drawBitmap1bit(x + g.xOffset, y + g.yOffset, g.width, g.height, bitmap); break;
        case 4: drawBitmap4bit(x + g.xOffset, y + g.yOffset, g.width, g.height, bitmap); break;
        }
        x += g.xAdvance;
    }
}

int Canvas::textWidth(const char *str) {
    return 0;
}