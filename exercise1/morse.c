/**
 * Exercise 1.4 - Morse code
 * Blinks "help!" in Morse code on the Pico W's on-board LED, forever.
 *
 * Built on the Blink example: pico_led_init() / pico_set_led() are unchanged.
 *
 * Timing rules (1 unit = 200 ms):
 *   dot = 1 unit, dash = 3 units
 *   gap between dots/dashes inside a letter = 1 unit
 *   gap between letters = 3 units
 *   gap between words   = 7 units
 */

#include <ctype.h>   // tolower()
#include <stdio.h>   // printf()
#include "pico/stdlib.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// ---- Timing (everything is a multiple of one unit) ----
#define UNIT_MS        200
#define DOT_MS         (1 * UNIT_MS)
#define DASH_MS        (3 * UNIT_MS)
#define SYMBOL_GAP_MS  (1 * UNIT_MS)   // between dots/dashes in one letter
#define LETTER_GAP_MS  (3 * UNIT_MS)   // between letters
#define WORD_GAP_MS    (7 * UNIT_MS)   // between words (and between repeats)

// ---- LED abstraction from the Blink example ----
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    return cyw43_arch_init();
#endif
}

void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

// ---- Morse table: index 0 = 'a', 1 = 'b', ... 25 = 'z' ----
static const char *const MORSE_LETTERS[26] = {
    ".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",  "....", "..",
    ".---", "-.-",  ".-..", "--",   "-.",   "---",  ".--.", "--.-", ".-.",
    "...",  "-",    "..-",  "...-", ".--",  "-..-", "-.--", "--.."
};

// Translate one character to its Morse string. Returns NULL if unknown.
static const char *to_morse(char c) {
    c = (char)tolower((unsigned char)c);
    if (c >= 'a' && c <= 'z') {
        return MORSE_LETTERS[c - 'a'];   // 'h' - 'a' = 7 -> "...."
    }
    if (c == '!') {
        return "-.-.--";
    }
    return NULL;
}

// One dot or dash: LED on for 1 or 3 units, then off.
static void blink_symbol(char symbol) {
    pico_set_led(true);
    sleep_ms(symbol == '-' ? DASH_MS : DOT_MS);
    pico_set_led(false);
}

// All dots/dashes of one letter, with a 1-unit gap BETWEEN them (not after the last).
static void blink_letter(const char *code) {
    for (int i = 0; code[i] != '\0'; i++) {
        if (i > 0) {
            sleep_ms(SYMBOL_GAP_MS);
        }
        blink_symbol(code[i]);
    }
}

// A whole message. Gaps go BETWEEN letters/words, so nothing extra at the start.
static void blink_message(const char *message) {
    bool start_of_word = true;

    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i];

        if (c == ' ') {                 // word boundary
            sleep_ms(WORD_GAP_MS);
            start_of_word = true;
            continue;
        }

        const char *code = to_morse(c);
        if (code == NULL) {             // character we can't send: skip it
            continue;
        }

        if (!start_of_word) {
            sleep_ms(LETTER_GAP_MS);
        }
        printf("%c  %s\n", c, code);    // shows up in the Serial Monitor
        blink_letter(code);
        start_of_word = false;
    }
}

int main() {
    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    while (true) {
        printf("--- sending: help! ---\n");
        blink_message("help!");
        sleep_ms(WORD_GAP_MS);          // treat each repeat as a new word
    }
}
