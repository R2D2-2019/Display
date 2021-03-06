#pragma once

#include <hwlib.hpp>
#include <i2c_bus.hpp>
#include <ssd1306.hpp>

namespace r2d2::display {
    /**
     * SSD1306 unbuffered interface for an oled
     * Implements hwlib::window to easily use text and drawing functions that
     * are already implemented. Extends from r2d2::display::ssd1306_i2c_c
     *
     * The template parameters are used for the parent class.
     */
    template <class DisplayScreen>
    class ssd1306_oled_unbuffered_c
        : public ssd1306_i2c_c<DisplayScreen> {
    private:
        /**
         * The buffer with the pixel data
         * The first byte is used for the data-prefix that the display driver
         * requires to be sent when sending pixel data.
         */
        uint8_t buffer[DisplayScreen::width * DisplayScreen::height / 8 + 1] =
            {};

    public:
        /**
         * Construct the display driver by providing the communication bus and
         * the address of the display.
         */
        ssd1306_oled_unbuffered_c(r2d2::i2c::i2c_bus_c &bus,
                                  uint8_t address)
            : ssd1306_i2c_c<DisplayScreen>(bus, address) {

            // set the command for writing to the screen
            buffer[0] = this->ssd1306_data_prefix;

            // write the initalisation sequence to the screen
            bus.write(address, this->ssd1306_initialization,
                      sizeof(this->ssd1306_initialization) / sizeof(uint8_t));
        }

        /**
         * @brief Write a pixel to the screen
         *
         * @param x
         * @param y
         * @param data Data > 0 will set the pixel. Data = 0 will clear the
         * pixel
         */
        void set_pixel(uint16_t x, uint16_t y, const uint16_t data) override {

            // calculate the index of the pixel
            uint16_t t_index = (x + (y / 8) * this->size.x) + 1;

            // set or clear the pixel
            if (data) {
                buffer[t_index] |= (0x01 << (y % 8));
            } else {
                buffer[t_index] &= ~(0x01 << (y % 8));
            }

            // write pixel byte to screen
            this->pixels_byte_write(hwlib::xy(x, y / 8), buffer[t_index]);
        }

        /**
         * This clears the display this overrides the default clear of hwlib
         * because it is realy inefficient for this screen.
         */
        void clear(hwlib::color col) override {
            // get a the data for the screen
            const uint8_t clear_color = (col == hwlib::white) ? 0xFF : 0x00;

            // clear the internal buffer with the screen color
            for (uint16_t i = 1; i < sizeof(buffer); i++) {
                buffer[i] = clear_color;
            }

            // update cursor of the display
            ssd1306_oled_unbuffered_c::command(
                ssd1306_oled_unbuffered_c::ssd1306_command::column_addr, 0,
                127);
            ssd1306_oled_unbuffered_c::command(
                ssd1306_oled_unbuffered_c::ssd1306_command::page_addr, 0, 7);

            // write data to the screen
            this->bus.write(this->address, this->buffer, sizeof(this->buffer));

            // update the cursor
            this->cursor = hwlib::xy(0, 0);
        }

        /**
         * This clears the display this overrides the default clear of hwlib
         * because it is realy inefficient for this screen.
         */
        void clear() override {
            // clear the display
            clear(this->background);
        }
    };

} // namespace r2d2::display