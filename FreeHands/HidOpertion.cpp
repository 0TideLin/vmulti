#include "FreeHands.h"
#include <random>

static int getRandom1to10() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    return dis(gen);
}

constexpr BYTE RELESE_BUTTON = 0x00;
constexpr BYTE LEFI_BUTTON = 0x01;
constexpr BYTE RIGTH_BUTTON = 0x02;
constexpr BYTE MID_BUTTON = 0x04;
const wchar_t* KeyboardItems[] = {
    ITEMSTRING_LEFT_MOUSE, ITEMSTRING_RIGHT_MOUSE, ITEMSTRING_MID_MOUSE, ITEMSTRING_RELEASE_MOUSE,\
L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z"
};
const int KeyboardItemSize = sizeof(KeyboardItems) / sizeof(KeyboardItems[0]);

VOID SendHidRequests()
{
    BYTE shiftKeys = 0;
    BYTE keyCodes[KBD_KEY_CODES] = { 0, 0, 0, 0, 0, 0 };
    DWORD DelayMs;

    for (size_t i = 0; i < gControlListHWNDs.size(); i++)
    {
        if (gKeycodeArray[i] >= 0 && gKeycodeArray[i] <= 3) {
            printf("gKeycodeArray[i]: %d\n", gKeycodeArray[i]);
            switch (gKeycodeArray[i]) {
            case 0:
                vmulti_update_relative_mouse(vmulti, RELESE_BUTTON, 0, 0, 0);
                break;
            case 1:
                vmulti_update_relative_mouse(vmulti, LEFI_BUTTON, 0, 0, 0);
                break;
            case 2:
                vmulti_update_relative_mouse(vmulti, RIGTH_BUTTON, 0, 0, 0);
                break;
            case 3:
                vmulti_update_relative_mouse(vmulti, MID_BUTTON, 0, 0, 0);
                break;
            default:
                break;
            }
        }
        else if (gKeycodeArray[i] > 3 && gKeycodeArray[i] <= 0x1D) {
            printf("gKeycodeArray[i]: %d\n", gKeycodeArray[i]);
            keyCodes[0] = gKeycodeArray[0];
            vmulti_update_keyboard(vmulti, shiftKeys, keyCodes);
            keyCodes[0] = 0;
            vmulti_update_keyboard(vmulti, shiftKeys, keyCodes);
        }
        else if (gKeycodeArray[i] > KEYCODE_DELAY_SIG) {
            DelayMs = (gKeycodeArray[i] - KEYCODE_DELAY_SIG) + getRandom1to10() * 10; //Add random 10~100ms
            printf("Delay %d ms\n", DelayMs);
            std::this_thread::sleep_for(std::chrono::milliseconds(DelayMs));
        }
    }
    return;
}