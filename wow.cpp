#ifndef UNICODE
#define UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <cmath>
#include <iostream>

using namespace Gdiplus;

double time_passed = 0.0;
double speed = 2.0;
double min_scale = 0.2;
double max_scale = 2.0;
Bitmap* pBitmap = nullptr;
int orig_width = 0;
int orig_height = 0;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);


            RECT rect;
            GetClientRect(hwnd, &rect);
            int clientWidth = rect.right - rect.left;
            int clientHeight = rect.bottom - rect.top;


            HDC memHdc = CreateCompatibleDC(hdc);
            HBITMAP memBmp = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
            HBITMAP oldBmp = (HBITMAP)SelectObject(memHdc, memBmp);


            HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(memHdc, &rect, whiteBrush);
            DeleteObject(whiteBrush);


            time_passed += 0.03 * speed;
            double sine_val = (std::sin(time_passed) + 1.0) / 2.0;
            double current_scale = min_scale + (max_scale - min_scale) * sine_val;

            int new_width = static_cast<int>(orig_width * current_scale);
            int new_height = static_cast<int>(orig_height * current_scale);

            int posX = (clientWidth - new_width) / 2;
            int posY = (clientHeight - new_height) / 2;


            Graphics graphics(memHdc);
            graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

            if (pBitmap && pBitmap->GetLastStatus() == Ok) {
                graphics.DrawImage(pBitmap, posX, posY, new_width, new_height);
            } else {

                SolidBrush redBrush(Color(220, 20, 60));
                graphics.FillEllipse(&redBrush, clientWidth / 2 - 60, clientHeight / 2 - 60, 120, 120);
            }


            BitBlt(hdc, 0, 0, clientWidth, clientHeight, memHdc, 0, 0, SRCCOPY);


            SelectObject(memHdc, oldBmp);
            DeleteObject(memBmp);
            DeleteDC(memHdc);

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    pBitmap = Bitmap::FromFile(L"apple.jpg");
    if (pBitmap->GetLastStatus() != Ok) {
        std::cout << "apple.jpg not found! Creating a placeholder red circle for testing.\n";
        delete pBitmap;
        pBitmap = nullptr;
    } else {
        orig_width = pBitmap->GetWidth();
        orig_height = pBitmap->GetHeight();
        std::cout << "Successfully loaded apple.jpg!\n";
    }


    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hInstance = hInstance;
    wc.lpszClassName = L"AppleAnimationClass";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClass(&wc)) {
        GdiplusShutdown(gdiplusToken);
        return -1;
    }


    HWND hwnd = CreateWindow(wc.lpszClassName, L"Jablko rozszerz",
                             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             100, 100, 800, 600, NULL, NULL, hInstance, NULL);


    MSG msg = {0};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {

            InvalidateRect(hwnd, NULL, FALSE);
            Sleep(16); //
        }
    }

    if (pBitmap) {
        delete pBitmap;
    }
    GdiplusShutdown(gdiplusToken);
    return 0;
}
