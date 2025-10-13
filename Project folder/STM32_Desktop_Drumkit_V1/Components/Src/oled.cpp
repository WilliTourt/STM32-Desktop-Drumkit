/**
 * @file oled.cpp
 * @brief OLED display driver library for STM32 MCUs
 * 
 * Provides basic control functions for I2C-based OLED displays, supporting:
 * - Text display
 * - Graphics drawing (Dots)
 * - Variable display
 * - Screen control
 * 
 * @author WilliTourt, willitourt@foxmail.com
 * @copyright Copyright (c) 2025 by WilliTourt
 * 
 * @note Comments are mostly written by AI
 */

#include "oled.h"

/**
 * @brief Construct a new OLED object
 * @param hi2c Pointer to I2C handle
 * @param height OLED display height in pixel rows
 */
OLED::OLED(I2C_HandleTypeDef* hi2c, uint8_t height)
    : _hi2c(hi2c), _height(height) {
    memset(displayBuffer, 0, sizeof(displayBuffer));
}

/**
 * @brief Initialize OLED display
 * @return true if initialization succeeded, false otherwise
 * 
 * Performs the following initialization sequence:
 * 1. Display off command
 * 2. Set display clock divide ratio/oscillator frequency
 * 3. Set multiplex ratio
 * 4. Set display offset
 * 5. Set display start line
 * 6. Charge pump setting
 * 7. Set memory addressing mode
 * 8. Set segment re-map
 * 9. Set COM pins hardware configuration
 * 10. Set contrast control
 * 11. Set pre-charge period
 * 12. Set VCOMH deselect level
 * 13. Entire display on
 * 14. Set normal display
 * 15. Display on command
 */
bool OLED::begin() {
	bool status = true;
    HAL_Delay(20);

    _wCmd(0xAE); // Display off
    _wCmd(0xD5); _wCmd(0x80); // Set display clock divide ratio/oscillator frequency
    _wCmd(0xA8); _wCmd(_height == 32 ? 0x1F : 0x3F); // Set multiplex ratio
    _wCmd(0xD3); _wCmd(0x00); // Set display offset
    _wCmd(0x40); // Set display start line
    _wCmd(0x8D); _wCmd(0x14); // Charge pump setting
    _wCmd(0x20); _wCmd(0x00); // Set memory addressing mode
    _wCmd(0xA1); _wCmd(0xC8); // Set segment re-map
    _wCmd(0xDA); _wCmd(_height == 32 ? 0x02 : 0x12); // Set COM pins hardware configuration
    _wCmd(0x81); _wCmd(0xCF); // Set contrast control
    _wCmd(0xD9); _wCmd(0xF1); // Set pre-charge period
    _wCmd(0xDB); _wCmd(0x40); // Set VCOMH deselect level
    _wCmd(0xA4); // Entire display on
    _wCmd(0xA6); // Set normal display
    status = _wCmd(0xAF); // Display on

    HAL_Delay(10);

    clear();
    return status;
}

/**
 * @brief Send command to OLED display
 * @param command Command byte to send
 * @return true if command sent successfully, false otherwise
 * 
 * Writes a command byte to the OLED's command register (0x00)
 * using I2C memory write operation.
 */
bool OLED::_wCmd(uint8_t command) {
	bool status = true;
    // 0x00: command register
    if (HAL_I2C_Mem_Write(_hi2c, OLED_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, &command, 1, 10) != HAL_OK) {
		status = false;
	}

	return status;
}

/**
 * @brief Send data to OLED display
 * @param data Data byte to send
 * 
 * Writes a data byte to the OLED's data register (0x40)
 * using I2C memory write operation.
 */
void OLED::_wData(uint8_t data) {
    // 0x40: data register
    HAL_I2C_Mem_Write(_hi2c, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 10);
}

/**
 * @brief Set a single pixel on/off on OLED display
 * @param x Column position (0-127)
 * @param y Row position (0-height-1)
 * @param state true to turn pixel on, false to turn off
 * 
 * Updates the display buffer and immediately writes the change to OLED.
 * Handles coordinate bounds checking.
 */
void OLED::setPixel(uint8_t x, uint8_t y, bool state) {
    if (x >= 128 || y >= _height) return;
    uint8_t page = y / 8;
    uint8_t bit = y % 8;

    if (state) {
        displayBuffer[page][x] |= (1 << bit);
    } else {
        displayBuffer[page][x] &= ~(1 << bit);
    }

    setCursor(x, page);
    _wData(displayBuffer[page][x]);
}

/**
 * @brief Set the cursor position on OLED display
 * @param x Column position (0-127)
 * @param y Page position (0-7 for 64px height, 0-3 for 32px height)
 * 
 * Sets the display start position by:
 * 1. Setting page address (0xB0 + y)
 * 2. Setting lower column address (x & 0x0F)
 * 3. Setting higher column address ((x & 0xF0) >> 4)
 */
void OLED::setCursor(uint8_t x, uint8_t y) {
    uint8_t lines = _height == 32 ? 3 : 7;
    if (y > lines) y = lines;
    if (x > 127) x = 127;
    
    _wCmd(0xB0 + y); // Set page address
    _wCmd(0x00 | (x & 0x0F)); // Set lower column address
    _wCmd(0x10 | ((x & 0xF0) >> 4)); // Set higher column address
}

/**
 * @brief Update the entire display buffer to OLED
 * 
 * Writes the entire display buffer (8 pages x 128 columns) to the OLED
 * by iterating through each page and column, sending the buffer data.
 */
void OLED::updateBufferArea() {
    for (uint8_t page = 0; page < 8; page++) {
        setCursor(0, page);
        for (uint8_t col = 0; col < 128; col++) {
            _wData(displayBuffer[page][col]);
        }
    }
}

/**
 * @brief Clear the entire OLED display
 * 
 * Fills the display buffer with zeros and updates the OLED.
 * Automatically adjusts for display height (32 or 64 pixels).
 * Resets cursor to (0,0) position after clearing.
 */
void OLED::clear() {
    uint8_t i, j;
    uint8_t lines = 8;

    if (_height == 32) {
        lines = 4;
    }

    for (i = 0; i < lines; i++) {
        setCursor(0, i);
        for (j = 0; j < 128; j++) {
            _wData(0x00);
            displayBuffer[i][j] = 0;
        }
    }
    setCursor(0, 0);
}

/**
 * @brief Clear a specific area of the OLED display
 * @param x1 Starting column (0-127)
 * @param page1 Starting page (0-7)
 * @param x2 Ending column (0-127)
 * @param page2 Ending page (0-7)
 * 
 * Clears the specified rectangular area by:
 * 1. Bounds checking the input parameters
 * 2. Filling the specified area with zeros
 * 3. Updating both display buffer and OLED
 * 4. Resetting cursor to (x1,page1) after clearing
 */
void OLED::clearPart(uint8_t x1, uint8_t page1, uint8_t x2, uint8_t page2) {
    if (x1 >= 128 || x2 >= 128 || page1 >= 8 || page2 >= 8) return;
    
    for (uint8_t i = page1; i < page2; i++) {
        setCursor(x1, i);
        for (uint8_t j = x1; j < x2; j++) {
            _wData(0x00);
            displayBuffer[i][j] = 0;
        }
    }
    setCursor(x1, page1);
}

/**
 * @brief Print text on OLED display
 * @param x Starting column position (0-127)
 * @param y Starting row position (0-3/7)
 * @param str String to display
 * @param size Font size (8 for 6x8 font, 16 for 8x16 font)
 * 
 * Handles:
 * - Newline characters (\n)
 * - Automatic line wrapping when reaching display edge
 * - Character range 0x20-0x7E (printable ASCII)
 * - Two font sizes: 6x8 and 8x16
 */
void OLED::printText(uint8_t x, uint8_t y, const char* str, uint8_t size) {
    while (*str) {
        if (*str == '\n') {
            x = 0;
            y += size;
            if (y >= _height) y = 0;
            setCursor(x, y);
        } else if (*str >= 0x20 && *str <= 0x7E) {
            uint8_t charIndex = *str - 0x20;
            if (size == 8) {
                // 6x8 font
                setCursor(x, y);
                for (uint8_t i = 0; i < 6; i++) {
                    _wData(_font6x8[charIndex][i]);
                }
                x += 6;
            } else if (size == 16) {
                // 8x16 font (spans two pages)
                for (uint8_t row = 0; row < 2; row++) {
                    setCursor(x, y + row);
                    for (uint8_t i = 0; i < 8; i++) {
                        _wData(_font8x16[charIndex][row*8 + i]);
                    }
                }
                x += 8;
            }
            
            if (x >= 128) {
                x = 0;
                y += size;
                if (y >= _height) y = 0;
            }
        }
        str++;
    }
}

/**
 * @brief Print an image on OLED display
 * @param x Starting column position (0-127)
 * @param y Starting row position (0-3/7)
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param image Pointer to image data array
 * 
 * The image data should be organized as a 2D array in row-major order:
 * image[height][width]
 * Each byte represents 8 vertical pixels (1 bit per pixel)
 */
void OLED::printImage(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image) {
    for (uint8_t i = 0; i < _height; i++) {
        setCursor(x, y + i);
        for (uint8_t j = 0; j < width; j++) {
            _wData(image[i * width + j]);
        }
    }
}

/**
 * @brief Compare two strings
 * @param str1 First string to compare
 * @param str2 Second string to compare
 * @return 1 if strings are equal, 0 otherwise
 * 
 * Internal helper function for string comparison.
 * Compares strings character by character until null terminator.
 */
uint8_t OLED::_cmpStrs(const char* str1, const char* str2) {
    while (*str1 && *str2) {    // 当两个字符串都没结束时
        if (*str1 != *str2) {
            return 0;           // 如果字符不同，返回 false
        }
        str1++;
        str2++;
    }
    return (*str1 == *str2);    // 如果两者都到达字符串末尾，则相等
}

/**
 * @brief Print a variable value on OLED display
 * @param x Column position (0-127)
 * @param y Row position (0-3/7)
 * @param num Value to display
 * @param type Data type ("int", "uint", "float", "hex")
 * @param length Display length
 * @param zeroPad Whether to pad with zeros (true) or spaces (false)
 * 
 * Supports multiple data formats:
 * - "int": Signed integer, right-aligned
 * - "uint": Unsigned integer, right-aligned
 * - "float": Floating point with 2 decimal places
 * - "hex": Hexadecimal, right-aligned
 * - Other: Displays "ERR"
 * 
 * Handles:
 * - Zero padding or space padding
 * - Negative number formatting
 * - Length truncation if necessary
 */
void OLED::printVar(uint8_t x, uint8_t y, float num, const char* type, uint8_t length, bool zeroPad) {
    char buffer[16];

    if (zeroPad) {
        memset(buffer, '0', length);
    } else {
        memset(buffer, ' ', length);
    }
    
    if (_cmpStrs(type, "int")) {
        // Signed integer display, right-aligned
        int32_t value = (int32_t)num;
        char temp[16];
        sprintf(temp, "%ld", value);

        uint8_t len = strlen(temp);
        if (len > length) { len = length; }

        if (zeroPad && value < 0) {
            // Handle zero padding for negative numbers
            memcpy(buffer + (length - len + 1), temp + 1, len - 1);
            buffer[length - len] = '-';
        } else {
            memcpy(buffer + (length - len), temp, len);
        }
		buffer[length] = '\0';
    } else if (_cmpStrs(type, "uint")) {
        // Unsigned integer display, right-aligned
        uint32_t value = (uint32_t)num;
        char temp[16];
        sprintf(temp, "%lu", value);

        uint8_t len = strlen(temp);
        if (len > length) len = length;
        memcpy(buffer + (length - len), temp, len);
		buffer[length] = '\0';
    } else if (_cmpStrs(type, "float")) {
        // Floating point display with 2 decimal places
        snprintf(buffer, sizeof(buffer), "%.2f", num);
        // Ensure length doesn't exceed specified value
        if (strlen(buffer) > length) {
            buffer[length] = '\0';
        }
    } else if (_cmpStrs(type, "hex")) {
        // Hexadecimal display, right-aligned
        uint16_t value = (uint16_t)num;
        char temp[16];
        sprintf(temp, "%X", value);
        uint8_t len = strlen(temp);
        if (len > length) len = length;
        memcpy(buffer + (length - len), temp, len);
		buffer[length] = '\0';
    } else {
        // Unknown type, display error
        sprintf(buffer, "ERR");
		buffer[length] = '\0';
    }
    
    printText(x, y, buffer, 8);
}

/**
 * @brief Draw a horizontal line on OLED display
 * @param x Starting column position (0-127)
 * @param y Row position (0-3/7)
 * @param width Line width in pixels
 * 
 * Draws a solid horizontal line by filling pixels with 1s.
 */
void OLED::printHLine(uint8_t x, uint8_t y, uint8_t width) {
    setCursor(x, y);
    for (uint8_t i = 0; i < width; i++) {
        _wData(0xFF);
    }
}

// void OLED::printVLine(uint8_t x, uint8_t y, uint8_t height) {
//     for (uint8_t i = 0; i < height; i++) {
//         setPixel(x, y + i, true);
//     }
//     updateBufferArea();
// }

/**
 * @brief Set OLED display brightness
 * @param enable true for low brightness, false for normal brightness
 * 
 * Controls display contrast by sending appropriate commands.
 */
void OLED::lowBrightness(bool enable) {
    _wCmd(0x81);
    _wCmd(enable ? 0x10 : 0xCF);
}

/**
 * @brief Turn OLED display on/off
 * @param state true to turn on, false to turn off
 * 
 * Sends display on/off command (0xAF/0xAE) to control power state.
 */
void OLED::power(bool state) {
	_wCmd(0x8D);
	_wCmd(state ? 0x14 : 0x10); // Enable/disable charge pump
    _wCmd(state ? 0xAF : 0xAE);
}
