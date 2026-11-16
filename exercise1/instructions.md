# Exercise Session 1

Work through the exercises in order. Write your answers directly in this file, in the
`_Answer:_` blocks under each question, and commit it together with any `.cpp` files
you are asked to attach, to your own copy of the repository.

---

## Exercise 1: Verify your setup

Confirm that the [`SETUP.md`](../SETUP.md)
instructions have been followed and your Pico development environment is in place.

**Anything that did not work, and how you solved it:**

> _Answer:_
> - The build failed at the last step with *"picotool.exe was blocked by your organization's Device Guard policy"*.
>   Cause: Windows **Smart App Control** blocks the picotool/pioasm executables the Pico extension downloads.
>   Fix: Windows Security -> App & browser control -> Smart App Control settings -> **Off**. After that Blink compiled (`blink.uf2`, `blink.elf`, `blink.bin`).
> - The Python extension shows an "invalid interpreter" warning. Harmless: we build C/C++, not Python.
> - Telemetry and experiments were turned off in the `SDU PHCE` VS Code profile as the course requires.

---

## Exercise 2: Build and run the blinker

Create, build and flash the example **Blink** project to confirm that your development
environment works from source code all the way to running firmware on the Pico W.

### Instructions

1. Open **Visual Studio Code**.
2. Open the **Raspberry Pi Pico** extension from the sidebar.
3. Create a new project and select the **Blink** example.
   - Select **Pico W** as the target board.
   - Choose a folder for the project.
   - Create/open the project in VS Code.
4. Add `stdio_init_all();` as the first step inside main()
5. Add `printf("Test");` inside while loop in main()
6. Build the project.
   - Use the **Compile / Build** command from the Raspberry Pi Pico extension.
   - Watch the terminal output and make sure the build finishes without errors.
7. Connect the Pico W to your computer using USB.
8. Flash the program to the Pico W using the **Run / Flash** command in the
   Raspberry Pi Pico extension or by manually uploading the .uf2 file.
9. Check the board. The LED should now blink continuously.
10. Connect the **Debug Probe** (hardware) and open the **Serial Monitor**, then look for the output.

### Checklist
- [x] Blink project created and opened
- [x] Project builds without errors
- [x] Program flashed to the Pico W
- [x] The LED is blinking
- [x] Serial Monitor output is visible

**If the build or flash failed, what was the error and what fixed it?**

> _Answer:_
> - Build: first failed with *"picotool.exe was blocked by your organization's Device Guard policy"* -> turned off Windows Smart App Control. Then it built `blink.elf` and `blink.uf2`.
> - Flash 1: *"No accessible RP-series devices in BOOTSEL mode were found"*. The USB cable was in the **Debug Probe**, not in the Pico W, so the PC never saw the Pico. Fix: plug the cable into the Pico W's own micro-USB port.
> - Flash 2: *"RP2040 device ... appears to be in BOOTSEL mode, but picotool was unable to connect. You may need to install a driver via Zadig"*. Windows has no driver for the BOOTSEL interface picotool uses. Workaround: drag-and-drop `build/blink.uf2` onto the **RPI-RP2** USB drive (BOOTSEL mass storage). Permanent fix: install the WinUSB driver with Zadig.
> - With both the Pico and the Debug Probe plugged in, the laptop screen went black once; unplugging fixed it -> check the probe wiring (only GP0, GP1, GND) before connecting it again.
> - `stdio_init_all()` must be the first line of `main()`, otherwise `printf` prints nothing.
> - Serial Monitor: no output at first because only the Debug Probe was plugged in. The probe does **not** power the Pico, so both USB cables are needed
>   (Pico W + probe). Then COM7 (Debug Probe, 115200 baud) showed the `printf` output, e.g. `--- sending: help! ---`, `h  ....`, `e  .`.

---

## Exercise 3: Explore PicoBricks

For each hardware module on the PicoBricks board, answer the following
questions:

1. What is the name of this module?
2. What does this module do?
3. Is it an input or output?

| # | Name | Purpose | Input / Output | Pico pin |
|---|------|---------|----------------|----------|
| 1 | Raspberry Pi Pico W (RP2040) | The microcontroller board: runs our program, has Wi-Fi/Bluetooth (CYW43 chip) and the on-board LED | Controller (neither) | - |
| 2 | Red LED | Simple light we can switch on/off (digital) | Output | GP7 |
| 3 | Push button | Gives a digital 1/0 when pressed/released | Input | GP10 |
| 4 | Potentiometer | Turning knob = variable resistor -> analog voltage read by the ADC | Input (analog) | GP26 (ADC0) |
| 5 | LDR (light sensor) | Resistance drops as light increases -> analog voltage read by the ADC | Input (analog) | GP27 (ADC1) |
| 6 | Buzzer | Makes a tone when driven with a square wave (PWM) | Output | GP20 |
| 7 | RGB LED (WS2812) | "Smart" LED, any colour, controlled with a serial data protocol | Output | GP6 |
| 8 | OLED display (SSD1306, 128x64) | Small screen for text/graphics, I2C address 0x3C | Output | I2C0: SDA GP4, SCL GP5 |
| 9 | Temperature & humidity sensor (SHTC3) | Measures temperature and relative humidity, I2C address 0x70 | Input | I2C0: SDA GP4, SCL GP5 |
| 10 | Relay | Electrically controlled switch: a small signal from the Pico switches a separate (bigger) circuit | Output | (read the label on the board) |
| 11 | Motor driver | Lets the Pico drive DC motors / the SG90 servo, which need more current than a GPIO pin can give | Output | (read the label on the board) |


---

## Exercise 4: Morse Code

Make a program that blinks `help!` as Morse
code using the LED abstractions in the blink project (`pico_set_led`). How you represent
and translate Morse code is up to you.

Rules:

- dot = 1 time unit (200ms)
- dash = 3 time units
- gap between dot/dash elements within one letter = 1 unit
- gap between letters = 3 units
- gap between words = 7 units

Use the Morse-code reference below and the C++ book/documentation, Google as needed.

Reference: <https://en.wikipedia.org/wiki/Morse_code>

Simply copy the modified .c file(s) containing your solution to the exercise folder.

**Attached file(s):**

> _Filename:_ `morse.c` (in this folder)
>
> Design: a lookup table `MORSE_LETTERS[26]` maps 'a'..'z' to strings of '.' and '-' (index = `c - 'a'`), '!' is handled separately (`-.-.--`).
> Three small functions build on each other: `blink_symbol()` (one dot/dash), `blink_letter()` (adds the 1-unit gap *between* symbols),
> `blink_message()` (adds the 3-unit gap between letters and the 7-unit gap at spaces). Gaps are only inserted *between* elements, so there is no extra pause after the last symbol.
> All timings are `#define`d as multiples of `UNIT_MS = 200`.
