#pragma once

#include <display_adapter.hpp>
#include <hwlib.hpp>

namespace r2d2::display {

    /**
     * This class inherits from the display_c class, and implements set_pixel
     * to do nothing. This is primarily used for testing purposes.
     *
     * @tparam DisplayScreen One of the display structs from display_screen.hpp
     */
    template <class DisplayScreen>
    class display_dummy_c : public display_c<DisplayScreen> {
    public:
        display_dummy_c()
            : display_c<DisplayScreen>(
                  hwlib::xy(DisplayScreen::width, DisplayScreen::height)) {
        }
        /**
         * @brief Converts a hwlib::color to the pixel data for the screen with
         * a maximum of two bytes for every pixel
         *
         * @param col
         */
        virtual uint16_t color_to_pixel(hwlib::color col) override {
            return 0;
        };

        /**
         * @brief Write a pixel to the screen
         *
         * @param x
         * @param y
         * @param data
         */
        virtual void set_pixel(uint16_t x, uint16_t y,
                               const uint16_t data) override{};

        r2d2::display::display_cursor_s get_cursor(uint8_t cursor_id) {
            return this->cursors[cursor_id];
        };
    };
} // namespace r2d2::display