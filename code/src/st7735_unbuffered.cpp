#include "st7735_unbuffered.hpp"

namespace r2d2::display {

    st7735_unbuffered_c::st7735_unbuffered_c(hwlib::spi_bus &bus,
                                             hwlib::pin_out &cs,
                                             hwlib::pin_out &dc,
                                             hwlib::pin_out &reset)
        : st7735_c(bus, cs, dc, reset) 
    {
        init();
    }

    void st7735_unbuffered_c::set_cursor(uint16_t x_min, uint16_t y_min,
                                         uint16_t x_max, uint16_t y_max) {
        // set the min and max row
        write_command(_CASET);
        write_data(uint8_t(x_min >> 8), uint8_t(x_min & 0xFF),
                   uint8_t(x_max >> 8), uint8_t(x_max & 0xFF));

        // set the min and max column
        write_command(_RASET);
        write_data(uint8_t(y_min >> 8), uint8_t(y_min & 0xFF),
                   uint8_t(y_max >> 8), uint8_t(y_max & 0xFF));
    }

    void st7735_unbuffered_c::set_pixel(uint16_t x, uint16_t y,
                                        const uint16_t data) {
        // set cursor on correct position
        set_cursor(x, y, 1, 1);

        // write to ram
        write_command(_RAMWR);

        // write pixel data to the screeen
        write_data((uint8_t *)&data, 2);
    }

    void st7735_unbuffered_c::set_pixels(uint16_t x, uint16_t y, uint16_t w,
                                         uint16_t h, const uint16_t *data) {
        // set the window to the size we want to write to
        set_cursor(x, y, x + w, y + h);

        // write to ram
        write_command(_RAMWR);

        // unfortunaly the arduino due is little endian otherwise we could put
        // all the data directly to the write_data function
        for (size_t i = 0; i < w * h; i++) {
            const uint8_t arr[2] = {uint8_t(data[i] >> 8),
                                    uint8_t(data[i] & 0xFF)};
            write_data(arr, 2);
        }
    }

    void st7735_unbuffered_c::set_pixels(uint16_t x, uint16_t y, uint16_t w,
                                         uint16_t h, const uint16_t data) {
        set_cursor(x, y, x + w, y + h);

        // write to ram
        write_command(_RAMWR);

        const uint8_t arr[2] = {uint8_t(data >> 8), uint8_t(data & 0xFF)};

        for (size_t i = 0; i < size_t((w + 1) * (h + 1)); i++) {
            write_data(arr, 2);
        }
    }

    void st7735_unbuffered_c::init() {
        // reset the display
        reset.write(true);
        hwlib::wait_ms(5);
        reset.write(false);
        hwlib::wait_ms(5);
        reset.write(true);
        hwlib::wait_ms(5);

        write_command(_SWRESET, _SLPOUT, _FRMCTR1);
        // frame rate control normal mode
        write_data(0x01, 0x2C, 0x2D);

        write_command(_FRMCTR2);
        // frame rate control idle mode
        write_data(0x01, 0x2C, 0x2D);

        write_command(_FRMCTR3);
        // frame rate control partial mode
        write_data(0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D);

        write_command(_INVCTR);
        write_data(0x07);

        write_command(_PWCTR1);
        write_data(0xA2, 0x02, 0x84);
        write_command(_PWCTR2);
        write_data(0xC5);
        write_command(_PWCTR3);
        write_data(0xA0, 0x00);
        write_command(_PWCTR4);
        write_data(0x8A, 0x2A);
        write_command(_PWCTR5);
        write_data(0x8A, 0xEE);

        write_command(_VMCTR1);
        write_data(0x0E);

        write_command(_INVOFF, _MADCTL);
        write_data(0x00);

        // set screen in 8 bit bus mode with 16 bit color
        write_command(_COLMOD);
        write_data(0x05);

        set_cursor(0x00, 0x00, width - 1, height - 1);

        write_command(_GMCTRP1);
        write_data(0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25,
                   0x2B, 0x39, 0x00, 0x01, 0x03, 0x10);

        write_command(_GMCTRN1);
        write_data(0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E,
                   0x37, 0x3F, 0x00, 0x00, 0x02, 0x10);

        write_command(_NORON, _DISPON);
        hwlib::wait_ms(10);
    }

} // namespace r2d2::display