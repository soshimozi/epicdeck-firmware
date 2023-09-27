#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

//#include <LinkedList.h>

typedef enum {
    LEFT,
    CENTER,
    RIGHT
} TextAlignment;

typedef struct {
    const char* title;
    int16_t x, y, w, h;
    int16_t globalX, globalY;
    //LinkedList<Window*> children;
    void* parent;
    int16_t zOrder;
} Window;

typedef Window* HWindow;

void DrawWindow(HWindow hw);

HWindow CreateWindow(HWindow parent, const char* title, 
    int16_t x, int16_t y, int16_t w, int16_t h, int16_t zOrder = 0);

void PrintText(HWindow parent, const char* text, int16_t x, int16_t y, 
    int16_t textSize, int16_t color, TextAlignment alignment);
  
void ClearText(HWindow parent, const char* text, int16_t x, int16_t y, int16_t textSize, TextAlignment alignment);


