# Notes during development

## AI-Thinker's ESP32-CAM

- Reset button on the "bottom" of the board so it is inaccessible when in breadboard

- No reset pin

- Out of 16 pins 6 pins are used for either GND or 5V or 3.3V.

## Display

- Documentation of esp-idf on SPI is sometimes a bit confusing
    - `MOSI` pin is the same thing as `data0_io_num` in `spi_bus_config_t`

- Datasheet for ILI9488 states that 5-6-5 color coding is available for 4-wire SPI however it is not really supported

- Minimizing pins used by the display

- Compromise on SPI "ballance" buffer size

## Camera

- Buffer orientation -> printing on display into square ends up being 2 triangles -> was caused by error in setting width of drawable area (+-1 error)

- Choose the right resolution to fit in memory yet have good quality

## Thermometer

- Decision between communicating one-wire over U(S)ART or bare timer

    - difficulty of implementation
    - 1 vs 2 pins needed
    

    