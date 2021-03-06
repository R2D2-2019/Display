#include <comm.hpp>
#include <hwlib.hpp>

#include <display_module.hpp>
#include <st7735_buffered.hpp>
#include <st7735_unbuffered.hpp>
#include <st7735_inverted_color_buffered.hpp>
#include <st7735_inverted_color_unbuffered.hpp>
#include <display_screen.hpp>
#include <ssd1306_oled_buffered.hpp>
#include <ssd1306_oled_unbuffered.hpp>

int main() {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    hwlib::wait_ms(10);
    
    // hardware spi
    auto spi_bus = due::hwspi(0, due::hwspi::SPI_MODE::SPI_MODE0, 3);
    auto pin_dummy = hwlib::pin_out_dummy;
    auto dc = hwlib::target::pin_out(hwlib::target::pins::d9);
    auto rst = hwlib::target::pin_out(hwlib::target::pins::d8);


    // use pin_dummy because chip select(cs) is controlled by the hardware spi
    r2d2::display::st7735_buffered_c<
        r2d2::display::st7735_128x160_s>
        color_display(spi_bus, pin_dummy, dc, rst);

    r2d2::comm_c comm;

    r2d2::display::module_c<
            r2d2::display::st7735_128x160_s> 
        module(comm, color_display);

    color_display.clear();
    color_display.flush();

    for (;;) {
        module.process();
    }
}