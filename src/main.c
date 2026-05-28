#include "game/game.h"

#include <stdbool.h>
#include <tchar.h>
#include <windows.h>

// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_CREATE:
        Game_Init(hwnd);
        break;

    case WM_SIZE:
        Game_HandleSize(hwnd);
        break;

    case WM_ACTIVATE:
        Game_HandleActivate(wParam);
        break;

    case WM_KEYDOWN:
        Game_HandleKeyDown(hwnd, wParam);
        break;

    case WM_MOUSEMOVE:
        Game_HandleMouseMove(lParam);
        break;

    case WM_LBUTTONDOWN:
        Game_HandleMouseDown(lParam);
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
        }
        break;
    case WM_DESTROY:
        Game_Cleanup();
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

// Windows GUI 애플리케이션 진입점
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszCmdLine,
    int nCmdShow
)
{
    HWND hwnd;
    MSG msg;
    WNDCLASS WndClass;

    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // 배경색: 검정
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = _T("LastResistanceWindowClass");
    RegisterClass(&WndClass);

    hwnd = CreateWindow(
        _T("LastResistanceWindowClass"), _T("최후의 저항"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 720, 1080,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // --- 실시간 게임 루프 ---
    // 프레임 속도 제어를 위한 타이머 변수
    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    bool bQuit = false;
    while (!bQuit)
    {
        // 1. 윈도우 메시지 처리
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = true;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else // 2. 메시지가 없으면 게임 로직 실행
        {
            // Delta Time 계산 (프레임 간 시간 간격)
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);
            float deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

            // 게임 로직 업데이트
            Game_Update(deltaTime);
            
            // 프레임 시간 갱신
            lastTime = currentTime;

            // 화면 렌더링
            Game_Render(hwnd);
        }
    }

    return (int)msg.wParam;
}