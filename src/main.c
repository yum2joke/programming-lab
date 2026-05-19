#include <windows.h>
#include <tchar.h>
#include <stdbool.h>
#include <math.h>

// 게임 관련 전역변수들
static float g_playerX, g_playerY;
static RECT g_clientRect;
static int g_mouseX, g_mouseY;
static bool g_isPaused = false;

// 게임 설정 상수
const int PLAYER_SIZE = 20;
const float PLAYER_SPEED = 500.0f; // 초당 픽셀 단위 속도
const COLORREF PLAYER_COLOR = RGB(150, 150, 255);

// 조준선
const int CROSSHAIR_CIRCLE_RADIUS = 5;
const COLORREF CROSSHAIR_LINE_COLOR = RGB(255, 200, 0);
const int CROSSHAIR_LINE_THICKNESS = 3;

// 일시정지 UI
const COLORREF PAUSE_TEXT_COLOR = RGB(255, 255, 255);
const int PAUSE_FONT_SIZE = 48;
const TCHAR* PAUSE_FONT_NAME = _T("Arial");
const TCHAR* PAUSE_TEXT = _T("일시정지");

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
        ShowCursor(FALSE); // 시스템 커서 숨기기
        break;

    case WM_SIZE: // 창 크기가 변경될 때마다 클라이언트 영역 업데이트
        GetClientRect(hwnd, &g_clientRect);
        if (!g_isPaused)
        {
            // 게임이 진행 중일 때 창 크기가 바뀌면 커서 제한 영역도 다시 설정
            RECT clipRect;
            GetClientRect(hwnd, &clipRect);
            ClientToScreen(hwnd, (LPPOINT)&clipRect.left);
            ClientToScreen(hwnd, (LPPOINT)&clipRect.right);
            ClipCursor(&clipRect);
        }
        break;

    case WM_ACTIVATE: // 창 활성화 상태 변경 감지
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            // 창이 비활성화되면 강제로 일시정지
            if (!g_isPaused)
            {
                g_isPaused = true;
                ShowCursor(TRUE);
                ClipCursor(NULL);
            }
        }
        break;

    case WM_KEYDOWN:
        // 일시정지 / 활성화
        if (wParam == VK_ESCAPE)
        {
            g_isPaused = !g_isPaused; // 일시정지 상태 토글
            if (g_isPaused)
            {
                ShowCursor(TRUE); // 커서 보이기
                ClipCursor(NULL); // 커서 제한 해제
            }
            else
            {
                ShowCursor(FALSE); // 커서 숨기기
                // 커서를 다시 창 안으로 제한
                RECT clipRect;
                GetClientRect(hwnd, &clipRect);
                ClientToScreen(hwnd, (LPPOINT)&clipRect.left);
                ClientToScreen(hwnd, (LPPOINT)&clipRect.right);
                ClipCursor(&clipRect);
            }
        }
        break;

    case WM_MOUSEMOVE:
        // 마우스 좌표 저장
        g_mouseX = LOWORD(lParam);
        g_mouseY = HIWORD(lParam);
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
        ClipCursor(NULL); // 프로그램 종료 전 커서 제한 해제
        ShowCursor(TRUE); // 커서 다시 보이기
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

            // --- 게임 로직 업데이트 (일시정지 아닐 때만) ---
            if (!g_isPaused)
            {
                // --- 입력 처리 및 상태 업데이트 ---
                float deltaX = 0.0f;
                float deltaY = 0.0f;

                // 1. 기본 자동 스크롤 (위로 밀려남)
                deltaY -= PLAYER_SPEED * 0.1f;

                // 2. 키 입력에 따른 추가 이동
                if (GetAsyncKeyState('A') & 0x8000) deltaX -= PLAYER_SPEED;
                if (GetAsyncKeyState('D') & 0x8000) deltaX += PLAYER_SPEED;
                if (GetAsyncKeyState('W') & 0x8000) deltaY -= PLAYER_SPEED;
                if (GetAsyncKeyState('S') & 0x8000)
                {
                    float lowerLimitY = (float)g_clientRect.bottom - PLAYER_SIZE;
                    if (g_playerY < lowerLimitY)
                    {
                        float ratio = (lowerLimitY - g_playerY) / lowerLimitY;
                        deltaY += PLAYER_SPEED * ratio;
                    }
                }

                // 3. 최종 위치 업데이트 (deltaTime 적용)
                g_playerX += deltaX * deltaTime;
                g_playerY += deltaY * deltaTime;

                // 경계 처리
                if (g_playerX < g_clientRect.left) g_playerX = g_clientRect.left;
                if (g_playerX + PLAYER_SIZE > g_clientRect.right) g_playerX = (float)g_clientRect.right - PLAYER_SIZE;
                if (g_playerY < g_clientRect.top) g_playerY = g_clientRect.top;
                if (g_playerY + PLAYER_SIZE > g_clientRect.bottom) g_playerY = (float)g_clientRect.bottom - PLAYER_SIZE;
            }

            // 항상 lastTime을 업데이트하여 일시정지 후 점프 현상 방지
            lastTime = currentTime;

            // --- 렌더링 (일시정지 여부와 관계없이 항상 실행) ---
            HDC hdc = GetDC(hwnd);
            HDC hMemDC = CreateCompatibleDC(hdc);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, g_clientRect.right, g_clientRect.bottom);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

            FillRect(hMemDC, &g_clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            
            HBRUSH hPlayerBrush = CreateSolidBrush(PLAYER_COLOR);
            RECT playerRect = { (int)g_playerX, (int)g_playerY, (int)g_playerX + PLAYER_SIZE, (int)g_playerY + PLAYER_SIZE };
            FillRect(hMemDC, &playerRect, hPlayerBrush);
            DeleteObject(hPlayerBrush);

            // 게임 진행 중에만 조준선 그리기
            if (!g_isPaused)
            {
                HPEN hLinePen = CreatePen(PS_SOLID, CROSSHAIR_LINE_THICKNESS, CROSSHAIR_LINE_COLOR);
                HPEN hOldPen = (HPEN)SelectObject(hMemDC, hLinePen);

                float playerCenterX = g_playerX + PLAYER_SIZE / 2.0f;
                float playerCenterY = g_playerY + PLAYER_SIZE / 2.0f;
                MoveToEx(hMemDC, (int)playerCenterX, (int)playerCenterY, NULL);

                // 플레이어 -> 마우스 방향 벡터 계산
                float dirX = g_mouseX - playerCenterX;
                float dirY = g_mouseY - playerCenterY;
                float length = sqrtf(dirX * dirX + dirY * dirY);

                if (length > 0.001f) // 0으로 나누는 것 방지
                {
                    // 방향 벡터를 정규화하고 길게 늘여서 화면 밖까지 라인 그리기
                    float endX = playerCenterX + (dirX / length) * 2000.0f;
                    float endY = playerCenterY + (dirY / length) * 2000.0f;
                    LineTo(hMemDC, (int)endX, (int)endY);
                }

                Ellipse(hMemDC, g_mouseX - CROSSHAIR_CIRCLE_RADIUS, g_mouseY - CROSSHAIR_CIRCLE_RADIUS, g_mouseX + CROSSHAIR_CIRCLE_RADIUS, g_mouseY + CROSSHAIR_CIRCLE_RADIUS);

                SelectObject(hMemDC, hOldPen);
                DeleteObject(hLinePen);
            }

            // 일시정지 텍스트 그리기
            if (g_isPaused)
            {
                SetTextColor(hMemDC, PAUSE_TEXT_COLOR);
                SetBkMode(hMemDC, TRANSPARENT);
                HFONT hFont = CreateFont(PAUSE_FONT_SIZE, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, PAUSE_FONT_NAME);
                HFONT hOldFont = (HFONT)SelectObject(hMemDC, hFont);

                DrawText(hMemDC, PAUSE_TEXT, -1, &g_clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectObject(hMemDC, hOldFont);
                DeleteObject(hFont);
            }

            BitBlt(hdc, 0, 0, g_clientRect.right, g_clientRect.bottom, hMemDC, 0, 0, SRCCOPY);

            SelectObject(hMemDC, hOldBitmap);
            DeleteObject(hBitmap);
            DeleteDC(hMemDC);
            ReleaseDC(hwnd, hdc);
        }
    }

    return (int)msg.wParam;
}