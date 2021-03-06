#pragma once

#include <display_adapter.hpp>
#include <hwlib.hpp>


namespace r2d2::display {

    template <class DisplayScreen>
    class st7735_c : public display_c<DisplayScreen> {
    protected:
        // all the commands for the display
        constexpr static uint8_t SWRESET = 0x01;
        constexpr static uint8_t RDDID = 0x04;
        constexpr static uint8_t RDDST = 0x09;
        constexpr static uint8_t SLPIN = 0x10;
        constexpr static uint8_t SLPOUT = 0x11;
        constexpr static uint8_t PTLON = 0x12;
        constexpr static uint8_t NORON = 0x13;
        constexpr static uint8_t INVOFF = 0x20;
        constexpr static uint8_t INVON = 0x21;
        constexpr static uint8_t DISPOFF = 0x28;
        constexpr static uint8_t DISPON = 0x29;
        constexpr static uint8_t CASET = 0x2A;
        constexpr static uint8_t RASET = 0x2B;
        constexpr static uint8_t RAMWR = 0x2C;
        constexpr static uint8_t RAMRD = 0x2E;
        constexpr static uint8_t PTLAR = 0x30;
        constexpr static uint8_t COLMOD = 0x3A;
        constexpr static uint8_t MADCTL = 0x36;
        constexpr static uint8_t FRMCTR1 = 0xB1;
        constexpr static uint8_t FRMCTR2 = 0xB2;
        constexpr static uint8_t FRMCTR3 = 0xB3;
        constexpr static uint8_t INVCTR = 0xB4;
        constexpr static uint8_t DISSET5 = 0xB6;
        constexpr static uint8_t PWCTR1 = 0xC0;
        constexpr static uint8_t PWCTR2 = 0xC1;
        constexpr static uint8_t PWCTR3 = 0xC2;
        constexpr static uint8_t NOP = 0x00;
        constexpr static uint8_t PWCTR4 = 0xC3;
        constexpr static uint8_t PWCTR5 = 0xC4;
        constexpr static uint8_t VMCTR1 = 0xC5;
        constexpr static uint8_t RDID1 = 0xDA;
        constexpr static uint8_t RDID2 = 0xDB;
        constexpr static uint8_t RDID3 = 0xDC;
        constexpr static uint8_t RDID4 = 0xDD;
        constexpr static uint8_t PWCTR6 = 0xFC;
        constexpr static uint8_t GMCTRP1 = 0xE0;
        constexpr static uint8_t GMCTRN1 = 0xE1;

        // since it uses a generic driver the display has a offset
        // When using the small screen, x_offset is 26;
        constexpr static uint8_t x_offset = DisplayScreen::x_offset;
        // When using the small screen, y_offset is 1;
        constexpr static uint8_t y_offset = DisplayScreen::y_offset;

        // display bus
        hwlib::spi_bus &bus;

        // pins for the display
        hwlib::pin_out &cs;
        hwlib::pin_out &dc;
        hwlib::pin_out &reset;

        /**
         * @brief Write a command to the screen
         *
         * @tparam Args
         * @param args
         */
        template <typename... Args>
        void write_command(Args &&... args) {
            // set display in command mode
            dc.write(false);

            // convert all commands to a array
            const uint8_t commands[] = {static_cast<uint8_t>(args)...};

            // write all commands on the bus
            auto transaction = bus.transaction(cs);
            transaction.write(sizeof(commands), commands);
        }

        /**
         * @brief Write display data/command data to the screen
         *
         * @param data
         * @param size
         */
        void write_data(const uint8_t *data, std::size_t size) {
            // set display in data mode
            dc.write(true);

            auto transaction = bus.transaction(cs);
            transaction.write(size, data);
        }

        /**
         * @brief Write display data/command data to the screen
         *
         * @tparam Args
         * @param data
         * @param args
         */
        template <typename... Args>
        void write_data(uint8_t data, Args &&... args) {
            // convert all data to a array
            const uint8_t commands[] = {data, static_cast<uint8_t>(args)...};

            // write all data on the bus
            write_data(commands, sizeof(commands));
        }

        /**
         * @brief inits the display
         *
         */
        void init() {
            // reset the display
            reset.write(true);
            hwlib::wait_ms(5);
            reset.write(false);
            hwlib::wait_ms(5);
            reset.write(true);
            hwlib::wait_ms(5);

            // do a software reset and stop sleep mode, voltage booster on
            write_command(SWRESET);
            hwlib::wait_ms(150);

            write_command(SLPOUT);
            hwlib::wait_ms(500);

            // frame rate control normal mode
            write_command(FRMCTR1);
            write_data(0x01, 0x2C, 0x2D);

            // frame rate control idle mode
            write_command(FRMCTR2);
            write_data(0x01, 0x2C, 0x2D);

            // frame rate control partial mode
            write_command(FRMCTR3);
            write_data(0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D);

            hwlib::wait_ms(20);

            // display invertion
            write_command(INVCTR);
            write_data(0x07);

            // power control settings
            write_command(PWCTR1);
            write_data(0xA2, 0x02, 0x84);
            write_command(PWCTR2);
            write_data(0xC5);
            write_command(PWCTR3);
            write_data(0xA0, 0x00);
            write_command(PWCTR4);
            write_data(0x8A, 0x2A);
            write_command(PWCTR5);
            write_data(0x8A, 0xEE);

            // vcomh voltage control
            write_command(VMCTR1);
            write_data(0x0E);

            // display inversion off, memory direction control
            write_command(INVOFF, MADCTL);
            write_data(0xC0);
            hwlib::wait_ms(20);

            // set screen in 8 bit bus mode with 16 bit color
            write_command(COLMOD);
            write_data(0x05);
            hwlib::wait_ms(10);

            // set the cursor to the maximum of the screen
            set_cursor(0x00, 0x00, width - 1, height - 1);

            // set gamma adjustment + polarity
            write_command(GMCTRP1);
            write_data(0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29,
                       0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10);

            // set gamma adjustment - polarity
            write_command(GMCTRN1);
            write_data(0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E,
                       0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10);

            // normal display on
            write_command(NORON);
            hwlib::wait_ms(10);

            // screen on
            write_command(DISPON);
            hwlib::wait_ms(100);
        }

        /**
         * @brief Set the cursor object
         *
         * @param x_min
         * @param y_min
         * @param x_max
         * @param y_max
         */
        void set_cursor(uint16_t x_min, uint16_t y_min, uint16_t x_max,
                        uint16_t y_max) {
            x_min += x_offset;
            x_max += x_offset;
            y_min += y_offset;
            y_max += y_offset;

            // set the min and max row
            write_command(CASET);
            write_data(static_cast<uint8_t>(x_min >> 8), static_cast<uint8_t>(x_min),
                       static_cast<uint8_t>(x_max >> 8), static_cast<uint8_t>(x_max));

            // set the min and max column
            write_command(RASET);
            write_data(static_cast<uint8_t>(y_min >> 8), static_cast<uint8_t>(y_min),
                       static_cast<uint8_t>(y_max >> 8), static_cast<uint8_t>(y_max));
        }

        /**
         * @brief Construct a new st7735_c object
         *
         * @param bus
         * @param cs
         * @param dc
         * @param reset
         */
        st7735_c(hwlib::spi_bus &bus, hwlib::pin_out &cs, hwlib::pin_out &dc,
                 hwlib::pin_out &reset)
            : display_c<DisplayScreen>(hwlib::xy(width, height)),
              bus(bus),
              cs(cs),
              dc(dc),
              reset(reset) {
            init();
        }

    public:
        /**
         * @brief width of display
         *
         */
        constexpr static uint8_t width = DisplayScreen::width;

        /**
         * @brief height of display
         *
         */
        constexpr static uint8_t height = DisplayScreen::height;

        /**
         * @brief Converst a hwlib::color to a uint16_t in the format the screen
         * wants
         *
         * @param col
         * @return constexpr uint16_t
         */
        uint16_t color_to_pixel(hwlib::color col) override {
            return (uint16_t(col.red) * 0x1F / 0xFF) << 11 |
                   (uint16_t(col.green) * 0x3F / 0xFF) << 5 |
                   (uint16_t(col.blue) * 0x1F / 0xFF);
        }
    };
} // namespace r2d2::display
