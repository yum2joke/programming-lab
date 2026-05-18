#include <windows.h>
#include <tchar.h>
#include <stdbool.h>

// 게임 관련 전역변수들
static float g_playerX, g_playerY;
static RECT g_clientRect;
const int PLAYER_SIZE = 50;
const float PLAYER_SPEED = 200.0f; // 초당 픽셀 단위 속도

// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_CREATE:
        GetClientRect(hwnd, &g_clientRect);
        // 플레이어 시작 위치를 창 중앙으로 초기화
        g_playerX = (float)(g_clientRect.right - PLAYER_SIZE) / 2.0f;
        g_playerY = (float)(g_clientRect.bottom - PLAYER_SIZE) / 2.0f;
        break;

    case WM_SIZE: // 창 크기가 변경될 때마다 클라이언트 영역 업데이트
        GetClientRect(hwnd, &g_clientRect);
        break;

    case WM_KEYDOWN:
        // 키보드 입력은 이제 실시간 게임 루프에서 직접 처리
        break;

    case WM_PAINT:
        // 렌더링은 게임 루프에서 직접 처리하므로, WM_PAINT는 비워둠
        // BeginPaint/EndPaint는 WM_PAINT 메시지를 처리 완료 상태로 만들기 위해 필요
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
        }
        break;
    case WM_DESTROY:
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
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
            lastTime = currentTime;

            // --- 입력 처리 (Update) ---
            if (GetAsyncKeyState('A') & 0x8000) g_playerX -= PLAYER_SPEED * deltaTime;
            if (GetAsyncKeyState('D') & 0x8000) g_playerX += PLAYER_SPEED * deltaTime;
            if (GetAsyncKeyState('W') & 0x8000) g_playerY -= PLAYER_SPEED * deltaTime;
            if (GetAsyncKeyState('S') & 0x8000) g_playerY += PLAYER_SPEED * deltaTime;

            // --- 상태 업데이트 (Update) ---
            // 경계 처리
            if (g_playerX < g_clientRect.left) g_playerX = g_clientRect.left;
            if (g_playerX + PLAYER_SIZE > g_clientRect.right) g_playerX = (float)g_clientRect.right - PLAYER_SIZE;
            if (g_playerY < g_clientRect.top) g_playerY = g_clientRect.top;
            if (g_playerY + PLAYER_SIZE > g_clientRect.bottom) g_playerY = (float)g_clientRect.bottom - PLAYER_SIZE;

            // --- 더블 버퍼링 렌더링 ---
            HDC hdc = GetDC(hwnd);

            // 1) 백버퍼(메모리 DC) 생성
            HDC hMemDC = CreateCompatibleDC(hdc);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, g_clientRect.right, g_clientRect.bottom);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

            // 2) 백버퍼에 그리기
            FillRect(hMemDC, &g_clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH)); // 배경 지우기
            
            HBRUSH hPlayerBrush = CreateSolidBrush(RGB(150, 150, 255));
            RECT playerRect = { (int)g_playerX, (int)g_playerY, (int)g_playerX + PLAYER_SIZE, (int)g_playerY + PLAYER_SIZE };
            FillRect(hMemDC, &playerRect, hPlayerBrush);
            DeleteObject(hPlayerBrush);

            // 3) 백버퍼를 화면으로 한번에 복사
            BitBlt(hdc, 0, 0, g_clientRect.right, g_clientRect.bottom, hMemDC, 0, 0, SRCCOPY);

            // 4) 리소스 정리
            SelectObject(hMemDC, hOldBitmap);
            DeleteObject(hBitmap);
            DeleteDC(hMemDC);
            ReleaseDC(hwnd, hdc);
        }
    }

    return (int)msg.wParam;
}