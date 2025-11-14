#pragma once
#ifndef _HID_MAP_H_
#define _HID_MAP_H_


#include <cstdint>
#include <cwchar>

namespace HIDWide {
    // 获取宽字符的HID键码
    inline uint8_t getKeycode(wchar_t character) {
        // 统一转换为小写进行基础映射
        wchar_t lowerChar = towlower(character);

        switch (lowerChar) {
        case L'a': return 0x04;
        case L'b': return 0x05;
        case L'c': return 0x06;
        case L'd': return 0x07;
        case L'e': return 0x08;
        case L'f': return 0x09;
        case L'g': return 0x0A;
        case L'h': return 0x0B;
        case L'i': return 0x0C;
        case L'j': return 0x0D;
        case L'k': return 0x0E;
        case L'l': return 0x0F;
        case L'm': return 0x10;
        case L'n': return 0x11;
        case L'o': return 0x12;
        case L'p': return 0x13;
        case L'q': return 0x14;
        case L'r': return 0x15;
        case L's': return 0x16;
        case L't': return 0x17;
        case L'u': return 0x18;
        case L'v': return 0x19;
        case L'w': return 0x1A;
        case L'x': return 0x1B;
        case L'y': return 0x1C;
        case L'z': return 0x1D;
        default:   return 0x00; // 未找到
        }
    }

    // 验证字符是否为字母
    inline bool isAlphaWide(wchar_t character) {
        return (character >= L'A' && character <= L'Z') ||
            (character >= L'a' && character <= L'z');
    }

    inline bool isWideDigit(wchar_t ch) {
        return ch >= L'0' && ch <= L'9';
    }

    inline bool isPositiveInteger(const wchar_t* str, int strlen) {
        if (strlen == 0) {
            return false;
        }

        for (int i = 0; i < strlen; i++) {
            if (!isWideDigit( *(str + i) ) ) {
                return false;
            }
        }
        return true;
    }
}
#endif // !_HID_MAP_H_