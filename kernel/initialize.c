#include <definitions.h>
#include <graphics.h>
#include <process.h>
#include <types.h>

#include "colors.h"
#include "font.h"
#include "screen.h"

void test_process1();
void test_process2();

BREW_EXPORT BREW_API
void initialize() {
    Font* font = load_kernel_font();

    GraphicsDeviceContext* gdc = create_graphics_device_context();
    gdc->begin_draw(gdc);
    gdc->clear(gdc, COLOR_BLACK);

    Screen* screen = create_screen(gdc);
    screen->draw_text(screen, (GraphicsPoint) { 0, 0 }, COLOR_WHITE, COLOR_BLACK, font, "BrewOS v0.1 pre-alpha");
    gdc->end_draw(gdc);

    create_kernel_process("Test Process 1", test_process1);
    create_kernel_process("Test Process 2", test_process2);

    destroy_process(1);

    while (true);
}

const char spinner[4] = { '|', '/', '-', '\\' };

void test_process1() {
    Font* font = get_kernel_font();
    GraphicsDeviceContext* gdc = create_graphics_device_context();
    Screen* screen = create_screen(gdc);

    uint8_t n = 0;
    char text[17] = "Test Process 1  ";

    while (true) {
        text[15] = spinner[n++];
        if (n > 3) {
            n = 0;
        }

        gdc->begin_draw(gdc);
        screen->draw_text(screen, (GraphicsPoint) { 0, 16 }, COLOR_RED, COLOR_BLACK, font, text);
        gdc->end_draw(gdc);

        sleep_process(25);
    }
}

void test_process2() {
    Font* font = get_kernel_font();
    GraphicsDeviceContext* gdc = create_graphics_device_context();
    Screen* screen = create_screen(gdc);

    uint8_t n = 0;
    char text[17] = "Test Process 2  ";

    while (true) {
        text[15] = spinner[n++];
        if (n > 3) {
            n = 0;
        }

        gdc->begin_draw(gdc);
        screen->draw_text(screen, (GraphicsPoint) { 0, 32 }, COLOR_GREEN, COLOR_BLACK, font, text);
        gdc->end_draw(gdc);

        sleep_process(50);
    }
}
