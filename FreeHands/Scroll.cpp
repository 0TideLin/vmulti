#include "FreeHands.h"


int g_scrollPos = 0; // 当前滚动位置
HWND gScrollContainer;

// 滚动容器窗口过程
LRESULT CALLBACK ScrollContainerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD wmEvent = HIWORD(wParam);
    // Parse the menu selections:
    if (wmEvent == CBN_SELCHANGE || wmEvent == EN_CHANGE) {
        UpdateKeycodeInfo(hWnd, wParam);
    }
    switch (message)
    {
    case WM_VSCROLL:
    {
        int oldScrollPos = g_scrollPos;

        switch (LOWORD(wParam))
        {
        case SB_LINEUP:        // 向上箭头
            g_scrollPos -= 10;
            break;
        case SB_LINEDOWN:      // 向下箭头
            g_scrollPos += 10;
            break;
        case SB_PAGEUP:        // 向上翻页
            g_scrollPos -= SCROLL_HEIGHT;
            break;
        case SB_PAGEDOWN:      // 向下翻页
            g_scrollPos += SCROLL_HEIGHT;
            break;
        case SB_THUMBTRACK:    // 拖动滑块
            g_scrollPos = HIWORD(wParam);
            break;
        case SB_THUMBPOSITION: // 滑块位置确定
            g_scrollPos = HIWORD(wParam);
            break;
        default:
            break;
        }

        // 限制滚动位置在有效范围内
        if (g_scrollPos < 0) g_scrollPos = 0;
        if (g_scrollPos > SCROLL_RANGE) g_scrollPos = SCROLL_RANGE;

        // 如果滚动位置改变，更新滚动条并重定位组合框
        if (g_scrollPos != oldScrollPos)
        {
            SetScrollPos(hWnd, SB_VERT, g_scrollPos, TRUE);

            // 重定位所有组合框
            for (size_t i = 0; i < gControlListHWNDs.size(); i++)
            {
                int yPos = 10 + (i * COMBO_SPACING) - g_scrollPos;
                SetWindowPos(gControlListHWNDs[i], NULL, CONTROL_START_X, yPos, 0, 0,
                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

                SetWindowPos(gScrollInfoHWNDs[i], NULL, START_X, yPos, 0, 0,
                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }

            // 重绘窗口
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
    break;

    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        g_scrollPos -= zDelta / 5; // 调整滚动速度

        // 限制滚动位置
        if (g_scrollPos < 0) g_scrollPos = 0;
        if (g_scrollPos > SCROLL_RANGE) g_scrollPos = SCROLL_RANGE;

        SetScrollPos(hWnd, SB_VERT, g_scrollPos, TRUE);

        // 重定位所有组合框
        for (size_t i = 0; i < gControlListHWNDs.size(); i++)
        {
            int yPos = 10 + (i * COMBO_SPACING) - g_scrollPos;
            SetWindowPos(gControlListHWNDs[i], NULL, CONTROL_START_X, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                
            SetWindowPos(gScrollInfoHWNDs[i], NULL, START_X, yPos, 0, 0,
                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        InvalidateRect(hWnd, NULL, TRUE);
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 绘制边框
        RECT rect;
        GetClientRect(hWnd, &rect);
        FrameRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        // 绘制标题
        SetBkMode(hdc, TRANSPARENT);

        EndPaint(hWnd, &ps);
    }
    break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
