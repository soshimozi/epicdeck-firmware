#include "windows.h"
#include <string.h>

extern Adafruit_ST7789 tft;

int16_t GlobalX(HWindow parent, int16_t x) {
    if (parent) {
        return x + parent->globalX;
    }
    return x;
}

int16_t GlobalY(HWindow parent, int16_t y) {
    if (parent) {
        return y + parent->globalY;
    }
    return y;
}

void DrawWindow(HWindow hw) {

    tft.drawRect(hw->globalX, hw->globalY, hw->w, hw->h, ST77XX_WHITE);

    tft.setTextSize(2);
    
    String str = "[ ";
    
    str += hw->title;
    str += " ]";

    uint16_t textWidth, textHeight;
    int16_t textX1, textY1;

  
    tft.getTextBounds(str, (int16_t)(hw->globalX + 5), hw->globalY, &textX1,
                     &textY1, &textWidth, &textHeight);

    tft.fillRect(textX1, textY1 - (textHeight / 2), textWidth, textHeight, ST77XX_BLACK);
    tft.setCursor(textX1, textY1 - (textHeight / 2));
    tft.print(str);
}


HWindow CreateWindow(HWindow parent, const char* title, int16_t x, int16_t y, int16_t w, int16_t h, int16_t zOrder) {
    HWindow hw = new Window;

    hw->title = title;
    hw->x = x;
    hw->y = y;
    hw->w = w;
    hw->h = h;
    hw->zOrder = zOrder;

    if (parent) {
        hw->parent = parent;
        hw->globalX = parent->globalX + x;
        hw->globalY = parent->globalY + y;
        //parent->children.add((void*)hw);
    } else {
        hw->parent = NULL;
        hw->globalX = x;
        hw->globalY = y;
        //mainWindows.add(hw);
    }

    return hw;
}

void PrintText(HWindow parent, const char* text, int16_t x, int16_t y, 
    int16_t textSize, int16_t color, TextAlignment alignment) {
    int16_t textX = x;
    int16_t textY = y;
    uint16_t textW = 0;
    uint16_t textH = 0;

    tft.setTextColor(color);
    tft.setTextSize(textSize);

    // Get text bounds
    tft.getTextBounds(text, 0, 0, &textX, &textY, &textW, &textH);

    // Adjust x-coordinate based on the alignment
    switch (alignment) {
        case CENTER:
            textX = x - textW / 2;
            break;
        case RIGHT:
            textX = x - textW;
            break;
        case LEFT:
        default:
            textX = x;
            break;
    }

    textY = GlobalY(parent, y);
    textX = GlobalX(parent, textX);

    // Print the text
    tft.setCursor(textX, textY);
    tft.print(text);
}

void ClearText(HWindow parent, const char* text, int16_t x, int16_t y, int16_t textSize, TextAlignment alignment) {
    int16_t textX = x;
    int16_t textY = y;
    uint16_t textW = 0;
    uint16_t textH = 0;

    tft.setTextSize(textSize);

    // Get text bounds
    tft.getTextBounds(text, 0, 0, &textX, &textY, &textW, &textH);

    // Adjust x-coordinate based on the alignment
    switch (alignment) {
        case CENTER:
            textX = x - textW / 2;
            break;
        case RIGHT:
            textX = x - textW;
            break;
        case LEFT:
        default:
            textX = x;
            break;
    }

    textY = GlobalY(parent, y);
    textX = GlobalX(parent, textX);

    tft.fillRect(textX, textY, textW, textH, ST77XX_BLACK);
}
