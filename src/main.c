#include "game/game.h"
#include "config.h"
#include "gdiplus_c.h"
#include "asset_manager.h"

#include <stdbool.h>
#include <tchar.h>
#include <windows.h>
#include <stdio.h> // _stprintf_s

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

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
        Game_HandleMouseDown(hwnd, lParam);
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

    // GDI+ 엔진 초기화
    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput = {0};
    gdiplusStartupInput.GdiplusVersion = 1; // GDI+ 버전 1 사용
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 에셋 매니저 초기화 및 이미지 로드
    AssetManager_Init();

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

    // Sleep 해상도 1ms로 향상
    timeBeginPeriod(1);

    // --- 실시간 게임 루프 ---
    // 프레임 제한 및 게임 속도 동기화를 위한 타이머 변수
    LARGE_INTEGER frequency;
    LARGE_INTEGER prevTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&prevTime);

    // 프레임당 duration
    float targetFrameDuration = 1.0f / FPS_LIMIT;

    // FPS 계산용 변수
    float fpsTimer = 0.0f;
    int frameCount = 0;

    bool bQuit = false;
    while (!bQuit)
    {
        // 윈도우 메시지 처리
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
        else // 메시지가 없으면 게임 로직 실행
        {
            // 1. Delta Time 계산 (프레임 간 시간 간격)
            LARGE_INTEGER startTime;
            QueryPerformanceCounter(&startTime);
            float deltaTime = (float)(startTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
            prevTime = startTime;

            // 1-1. 1초 단위로 FPS 계산 및 윈도우 타이틀에 표시
            fpsTimer += deltaTime;
            frameCount++;
            if (fpsTimer >= 1.0f)
            {
                TCHAR title[256];
                snprintf(title, sizeof(title), "최후의 저항 - FPS: %d", frameCount);
                SetWindowText(hwnd, title);
                
                while (fpsTimer >= 1.0f) {
                    fpsTimer -= 1.0f;
                }
                frameCount = 0;
            }

            // 2. 게임 업데이트 및 렌더
            Game_Update(deltaTime);
            Game_Render(hwnd);

            // 3. 프레임 제한

            // work(Update + Render)에 걸린 delta 계산
            LARGE_INTEGER endTime;
            QueryPerformanceCounter(&endTime);
            float workDelta = (float)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;            

            // workDelta가 프레임 duration보다 짧다면-
            if (workDelta < targetFrameDuration)
            {
                // 뺀 값을 밀리초만큼 대기
                DWORD sleepMs = (DWORD)((targetFrameDuration - workDelta) * 1000.0f);
                if (sleepMs > 0) Sleep(sleepMs);
            }

            // 연산이 더 오래 걸렸다면- Sleep하지 않고 바로 넘어감
        }
    }

    // 프로그램 종료 시 Sleep 해상도 원상복구
    timeEndPeriod(1);

    // GDI+ 리소스 정리 및 엔진 종료
    AssetManager_Cleanup();
    GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}