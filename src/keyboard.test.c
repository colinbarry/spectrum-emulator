#include "keyboard.h"
#include <picotest/picotest.h>
#include <stdbool.h>
#include <stdint.h>

/* The keyboard is read in rows of 5 values, where a reset bit indicates the key
is pressed and a set bit indicates the key is not. */
static bool test_row(int8_t const value,
                     bool const bit0,
                     bool const bit1,
                     bool const bit2,
                     bool const bit3,
                     bool const bit4)
{
    return (value & 0x01) ^ bit0 && (value & 0x02) ^ bit1 && (value & 0x04) ^ bit2
           && (value & 0x08) ^ bit3 && (value & 0x10) ^ bit4;
}

struct PortAndBit
{
    uint16_t port;
    uint8_t bit;
};

static const struct PortAndBit key_read_settings[40] = {
    /* spk_1 */ {0xf7fe, 0x00},
    /* spk_2 */ {0xf7fe, 0x01},
    /* spk_3 */ {0xf7fe, 0x02},
    /* spk_4 */ {0xf7fe, 0x03},
    /* spk_5 */ {0xf7fe, 0x04},
    /* spk_6 */ {0xeffe, 0x04},
    /* spk_7 */ {0xeffe, 0x03},
    /* spk_8 */ {0xeffe, 0x02},
    /* spk_9 */ {0xeffe, 0x01},
    /* spk_0 */ {0xeffe, 0x00},
    /* spk_q */ {0xfbfe, 0x00},
    /* spk_w */ {0xfbfe, 0x01},
    /* spk_e */ {0xfbfe, 0x02},
    /* spk_r */ {0xfbfe, 0x03},
    /* spk_t */ {0xfbfe, 0x04},
    /* spk_y */ {0xdffe, 0x04},
    /* spk_u */ {0xdffe, 0x03},
    /* spk_i */ {0xdffe, 0x02},
    /* spk_o */ {0xdffe, 0x01},
    /* spk_p */ {0xdffe, 0x00},
    /* spk_a */ {0xfdfe, 0x00},
    /* spk_s */ {0xfdfe, 0x01},
    /* spk_d */ {0xfdfe, 0x02},
    /* spk_f */ {0xfdfe, 0x03},
    /* spk_g */ {0xfdfe, 0x04},
    /* spk_h */ {0xbffe, 0x04},
    /* spk_j */ {0xbffe, 0x03},
    /* spk_k */ {0xbffe, 0x02},
    /* spk_l */ {0xbffe, 0x01},
    /* spk_enter */ {0xbffe, 0x00},
    /* spk_caps_shift */ {0xfefe, 0x00},
    /* spk_z */ {0xfefe, 0x01},
    /* spk_x */ {0xfefe, 0x02},
    /* spk_c */ {0xfefe, 0x03},
    /* spk_v */ {0xfefe, 0x04},
    /* spk_b */ {0x7ffe, 0x04},
    /* spk_n */ {0x7ffe, 0x03},
    /* spk_m */ {0x7ffe, 0x02},
    /* spk_symbol_shift */ {0x7ffe, 0x01},
    /* spk_space */ {0x7ffe, 0x00}};

static bool test_key_is_set(struct Keyboard const* keyboard,
                            enum SpectrumKey const key)
{
    struct PortAndBit const settings = key_read_settings[key];
    return (~kb_read(keyboard, settings.port) & (1 << settings.bit))
           == (1 << settings.bit);
}

static bool test_key_is_reset(struct Keyboard const* keyboard,
                              enum SpectrumKey const key)
{
    struct PortAndBit const settings = key_read_settings[key];
    return kb_read(keyboard, settings.port) & (1 << settings.bit);
}

DEF_TEST(can_press_single_keys)
{
    struct Keyboard kb;
    kb_construct(&kb);

    for (int i = 0; i < 40; ++i)
    {
        enum SpectrumKey key = i;
        ASSERT(test_key_is_reset(&kb, key));
        kb_on_keydown(&kb, (enum SpectrumKey)key);
        ASSERT(test_key_is_set(&kb, key));
    }
}

DEF_TEST(can_press_multiple_keys_in_same_row)
{
    struct Keyboard kb;
    kb_construct(&kb);

    ASSERT(test_key_is_reset(&kb, spk_q));
    ASSERT(test_key_is_reset(&kb, spk_w));
    ASSERT(test_key_is_reset(&kb, spk_e));
    ASSERT(test_key_is_reset(&kb, spk_r));
    ASSERT(test_key_is_reset(&kb, spk_t));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_q);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_reset(&kb, spk_w));
    ASSERT(test_key_is_reset(&kb, spk_e));
    ASSERT(test_key_is_reset(&kb, spk_r));
    ASSERT(test_key_is_reset(&kb, spk_t));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_w);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_w));
    ASSERT(test_key_is_reset(&kb, spk_e));
    ASSERT(test_key_is_reset(&kb, spk_r));
    ASSERT(test_key_is_reset(&kb, spk_t));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_e);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_w));
    ASSERT(test_key_is_set(&kb, spk_e));
    ASSERT(test_key_is_reset(&kb, spk_r));
    ASSERT(test_key_is_reset(&kb, spk_t));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_e);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_w));
    ASSERT(test_key_is_set(&kb, spk_e));
    ASSERT(test_key_is_reset(&kb, spk_r));
    ASSERT(test_key_is_reset(&kb, spk_t));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_r);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_w));
    ASSERT(test_key_is_set(&kb, spk_e));
    ASSERT(test_key_is_set(&kb, spk_r));
    ASSERT(test_key_is_reset(&kb, spk_t));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_t);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_w));
    ASSERT(test_key_is_set(&kb, spk_e));
    ASSERT(test_key_is_set(&kb, spk_r));
    ASSERT(test_key_is_set(&kb, spk_t));
}

DEF_TEST(can_press_multiple_keys_in_different_rows)
{
    struct Keyboard kb;
    kb_construct(&kb);

    ASSERT(test_key_is_reset(&kb, spk_q));
    ASSERT(test_key_is_reset(&kb, spk_1));
    ASSERT(test_key_is_reset(&kb, spk_2));
    ASSERT(test_key_is_reset(&kb, spk_i));
    ASSERT(test_key_is_reset(&kb, spk_enter));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_q);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_reset(&kb, spk_1));
    ASSERT(test_key_is_reset(&kb, spk_2));
    ASSERT(test_key_is_reset(&kb, spk_i));
    ASSERT(test_key_is_reset(&kb, spk_enter));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_1);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_1));
    ASSERT(test_key_is_reset(&kb, spk_2));
    ASSERT(test_key_is_reset(&kb, spk_i));
    ASSERT(test_key_is_reset(&kb, spk_enter));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_2);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_1));
    ASSERT(test_key_is_set(&kb, spk_2));
    ASSERT(test_key_is_reset(&kb, spk_i));
    ASSERT(test_key_is_reset(&kb, spk_enter));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_i);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_1));
    ASSERT(test_key_is_set(&kb, spk_2));
    ASSERT(test_key_is_set(&kb, spk_i));
    ASSERT(test_key_is_reset(&kb, spk_enter));

    kb_on_keydown(&kb, (enum SpectrumKey)spk_enter);
    ASSERT(test_key_is_set(&kb, spk_q));
    ASSERT(test_key_is_set(&kb, spk_1));
    ASSERT(test_key_is_set(&kb, spk_2));
    ASSERT(test_key_is_set(&kb, spk_i));
    ASSERT(test_key_is_set(&kb, spk_enter));
}

DEF_SUITE(keyboard)
{
    RUN_TEST(can_press_single_keys);
    RUN_TEST(can_press_multiple_keys_in_same_row);
    RUN_TEST(can_press_multiple_keys_in_different_rows);
}
