#pragma once

#include <hwlib.hpp>
#include <i2c_bus.hpp>

namespace r2d2::display {
    enum class ssd1306_command : uint8_t {
        set_contrast = 0x81,
        display_all_on_resume = 0xa4,
        display_all_on = 0xa5,
        normal_display = 0xa6,
        invert_display = 0xa7,
        display_off = 0xae,
        display_on = 0xaf,
        set_display_offset = 0xd3,
        set_compins = 0xda,
        set_vcom_detect = 0xdb,
        set_display_clock_div = 0xd5,
        set_precharge = 0xd9,
        set_multiplex = 0xa8,
        set_low_column = 0x00,
        set_high_column = 0x10,
        set_start_line = 0x40,
        memory_mode = 0x20,
        column_addr = 0x21,
        page_addr = 0x22,
        com_scan_inc = 0xc0,
        com_scan_dec = 0xc8,
        seg_remap = 0xa0,
        charge_pump = 0x8d,
        external_vcc = 0x01,
        switch_cap_vcc = 0x02,
        activate_scroll = 0x2f,
        deactivate_scroll = 0x2e,
        set_vertical_scroll_area = 0xa3,
        right_horizontal_scroll = 0x26,
        left_horizontal_scroll = 0x27,
        vertical_and_right_horizontal_scroll = 0x29,
        vertical_and_left_horizontal_scroll = 0x2A
    };

    /// value to send over i2c before a command
    constexpr uint8_t ssd1306_cmd_prefix = 0x80;

    /// value to send over i2c before data
    constexpr uint8_t ssd1306_data_prefix = 0x40;

    /// SSD1306 chip initialization
    constexpr const uint8_t ssd1306_initialization[] = {
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::display_off,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_display_clock_div,
        0x80,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_multiplex,
        0x3f,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_display_offset,
        0x00,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_start_line | 0x00,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::charge_pump,
        0x14,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::memory_mode,
        0x00,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::seg_remap | 0x01,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::com_scan_dec,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_compins,
        0x12,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_contrast,
        0xcf,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_precharge,
        0xf1,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::set_vcom_detect,
        0x40,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::display_all_on_resume,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::normal_display,
        ssd1306_cmd_prefix,
        (uint8_t)ssd1306_command::display_on};

    class ssd1306_i2c_c {
    protected:
        /// The I2C bus
        r2d2::i2c::i2c_bus_c bus;
        /// The device address
        uint8_t address;
        /// The current cursor location in the controller
        hwlib::xy cursor;

    public:
        // construct by providing the i2c channel
        ssd1306_i2c_c(r2d2::i2c::i2c_bus_c &bus, const uint8_t &address);

        /// send a command without data
        void command(ssd1306_command c);

        /// send a command with one data byte
        void command(ssd1306_command c, uint8_t d0);

        /// send a command with two data bytes
        void command(ssd1306_command c, uint8_t d0, uint8_t d1);

        /// send one byte of data
        void data(uint8_t d);
        /// write the pixel byte d at column x page y
        void pixels_byte_write(hwlib::xy location, uint8_t d);
    };

} // namespace r2d2::display