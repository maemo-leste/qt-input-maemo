/*
* MIT License
 *
 * Copyright (c) 2023 Archer Pergande
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "xcb/xcb.h"

/**
 * @brief Returns a pointer to an array of all keysyms in a keyboard mapping
 *
 * @param R The mapping of a keyboard (xcb_get_keyboard_mapping_reply_t).
 *
 * @return xcb_keysym_t*.
 */
#define xcb_get_keysym_map(R) (xcb_keysym_t *) (R + 1)

/**
 * @brief Calculates the index number of the first keysym in a keysym array that matches with K.
 * @brief 2nd/3rd/4th/... keysyms follow directly after the first until keysyms_per_keycode is reached
 *
 * @param K The keycode to find the keysym of (xcb_keycode_t).
 * @param M The minimum keycode value (xcb_keycode_t).
 * @param P The number of keysyms per keycode (uint8_t).
 *
 * @return unsigned int
*/
#define xcb_keycode_to_keysym_index(K,M,P) ((K - M) * P)

/**
 * @brief Calculates a keycode based on the index of a keysym in a keysym array.
 *
 * @param I The index of the keysym to find the keycode of (unsigned int).
 * @param P The number of keysyms per keycode (uint8_t).
 * @param M The minimum keycode value (xcb_keycode_t).
 *
 * @return xcb_keycode_t
 */
#define xcb_keysym_index_to_keycode(I,M,P) ((I/P)+M)

/**
 * @brief Returns the first keysym found that corresponds to the specified keycode.
 *
 * @param setup The setup information about the x server. See xcb_get_setup().
 * @param mapping The mapping of the keyboard. See xcb_get_keyboard_mapping().
 * @param keycode The keycode to find the keysym of.
 *
 * @return =0 keycode is invalid or it has no corresponding keysym.
 * @return !=0 The keysym has been returned successfully.
 */
inline xcb_keysym_t xcb_keycode_to_keysym(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keycode_t keycode) {
    if (keycode < setup->min_keycode || keycode > setup->max_keycode)
        return 0;
    return (xcb_get_keysym_map(mapping))[xcb_keycode_to_keysym_index(keycode, setup->min_keycode, mapping->keysyms_per_keycode)];
}

/**
 * @brief Returns an array of keysyms which correspond to the specified keycode.
 * @brief Any array values outside of mapping->keysyms_per_keycode should be considered invalid.
 *
 * @param setup The setup information about the x server. See xcb_get_setup().
 * @param mapping The mapping of the keyboard. See xcb_get_keyboard_mapping().
 * @param index The keycode to find the keysyms of.
 *
 * @return =0 The keycode specified is invalid or it has no corresponding keysyms.
 * @return !=0 The array of keysyms were successfully returned.
 */
inline xcb_keysym_t *xcb_keycode_to_keysyms(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keycode_t keycode) {
    if (keycode < setup->min_keycode || keycode > setup->max_keycode)
        return nullptr;
    return (xcb_get_keysym_map(mapping))+xcb_keycode_to_keysym_index(keycode, setup->min_keycode, mapping->keysyms_per_keycode);
}

/**
 * @brief Returns the keycode that corresponds to the specified keysym.
 *
 * @param setup The setup information about the x server. See xcb_get_setup().
 * @param mapping The mapping of the keyboard. See xcb_get_keyboard_mapping().
 * @param keysym The keysym to find the keycode of.
 *
 * @return =0 keysym has no corresponding keysym
 * @return !=0 The keycode has been returned successfully.
 */
inline xcb_keycode_t xcb_keysym_to_keycode(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keysym_t keysym) {
    const xcb_keysym_t *keysym_map = xcb_get_keysym_map(mapping);
    const unsigned int precalc = setup->min_keycode*mapping->keysyms_per_keycode; /* pre-calculating part of xcb_keycode_to_keysym_index() equation to make the for loop faster */
    register unsigned char shift,keycode;
    for(shift = 0; shift < mapping->keysyms_per_keycode; shift++) {
        for(keycode = setup->min_keycode; keycode < setup->max_keycode;keycode++) {
            if(keysym_map[keycode*mapping->keysyms_per_keycode-precalc+shift] == keysym)
                return keycode;
        }
    }
    return 0;
}
