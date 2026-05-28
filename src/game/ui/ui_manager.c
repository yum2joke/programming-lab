#include "ui_manager.h"

#include "config.h"

typedef enum { UI_MODE_RENDER, UI_MODE_HIT_TEST } UIMode;

typedef struct {
    UIMode mode;
    RECT clientRect;
    int mouseX;
    int mouseY;

    union {
        struct { HDC hdc; } render;             // RENDER 모드용
        struct { UIAction hitAction; } hitTest; // HIT_TEST 모드용
    } modeData;
} UIContext;

static void DrawButton(HDC hdc, RECT btnRect, const TCHAR* text)
{
    HBRUSH hBtnBrush = CreateSolidBrush(RESTART_BUTTON_COLOR);
    FillRect(hdc, &btnRect, hBtnBrush);
    DeleteObject(hBtnBrush);

    SetTextColor(hdc, RESTART_TEXT_COLOR);
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFont = CreateFont(RESTART_FONT_SIZE, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, PAUSE_FONT_NAME);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    DrawText(hdc, text, -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

static void DrawTitle(HDC hdc, RECT clientRect, const TCHAR* text, COLORREF color, int fontSize)
{
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, PAUSE_FONT_NAME);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    RECT textRect = clientRect;
    textRect.bottom = clientRect.bottom / 2; // 화면 중앙의 위쪽 영역으로 배치
    DrawText(hdc, text, -1, &textRect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

// 버튼 프로세스 (렌더, 클릭검사 등)
static void ProcessButton(UIContext* ctx, int x, int y, int width, int height, const TCHAR* text, UIAction action)
{
    RECT rect = { x, y, x + width, y + height };
    
    if (ctx->mode == UI_MODE_RENDER)
    {
        DrawButton(ctx->modeData.render.hdc, rect, text);
    }
    else if (ctx->mode == UI_MODE_HIT_TEST)
    {
        if (ctx->mouseX >= rect.left && ctx->mouseX <= rect.right && 
            ctx->mouseY >= rect.top && ctx->mouseY <= rect.bottom)
        {
            ctx->modeData.hitTest.hitAction = action;
        }
    }
}

// 타이틀 프로세스
static void ProcessTitle(UIContext* ctx, const TCHAR* text, COLORREF color, int fontSize)
{
    if (ctx->mode == UI_MODE_RENDER)
    {
        DrawTitle(ctx->modeData.render.hdc, ctx->clientRect, text, color, fontSize);
    }
}

// UI 통합 프로세스 (버튼, 타이틀 etc...)
static void ProcessUI(UIContext* ctx, GameState gameState)
{
    int centerX = ctx->clientRect.right / 2;
    int centerY = ctx->clientRect.bottom / 2;
    int startX = centerX - (RESTART_BUTTON_WIDTH / 2);
    int y1 = centerY + 50;
    int y2 = y1 + RESTART_BUTTON_HEIGHT + BUTTON_SPACING;

    switch (gameState)
    {
        case GAME_STATE_MAIN_MENU:
            ProcessTitle(ctx, TITLE_TEXT, TITLE_TEXT_COLOR, TITLE_FONT_SIZE);
            ProcessButton(ctx, startX, y1, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, START_BUTTON_TEXT, UI_ACTION_START);
            break;

        case GAME_STATE_GAME_OVER:
            ProcessTitle(ctx, GAMEOVER_TEXT, GAMEOVER_TEXT_COLOR, GAMEOVER_FONT_SIZE);
            ProcessButton(ctx, startX, y1, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, RESTART_TEXT, UI_ACTION_RESTART);
            ProcessButton(ctx, startX, y2, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, MAINMENU_BUTTON_TEXT, UI_ACTION_MAIN_MENU);
            break;

        case GAME_STATE_GAME_CLEAR:
            ProcessTitle(ctx, GAMECLEAR_TEXT, GAMECLEAR_TEXT_COLOR, GAMECLEAR_FONT_SIZE);
            ProcessButton(ctx, startX, y1, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, RESTART_TEXT, UI_ACTION_RESTART);
            ProcessButton(ctx, startX, y2, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, MAINMENU_BUTTON_TEXT, UI_ACTION_MAIN_MENU);
            break;

        case GAME_STATE_PAUSED:
            ProcessTitle(ctx, PAUSE_TEXT, PAUSE_TEXT_COLOR, PAUSE_FONT_SIZE);
            ProcessButton(ctx, startX, y1, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, RESTART_TEXT, UI_ACTION_RESTART);
            ProcessButton(ctx, startX, y2, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, MAINMENU_BUTTON_TEXT, UI_ACTION_MAIN_MENU);
            break;
            
        default:
            break;
    }
}

void UIManager_Render(HDC hdc, RECT clientRect, GameState gameState)
{
    UIContext ctx = { 
        .mode = UI_MODE_RENDER, 
        .clientRect = clientRect 
    };
    ctx.modeData.render.hdc = hdc;
    ProcessUI(&ctx, gameState);
}

UIAction UIManager_CheckButtonClick(int mouseX, int mouseY, RECT clientRect, GameState gameState)
{
    UIContext ctx = { 
        .mode = UI_MODE_HIT_TEST, 
        .clientRect = clientRect, 
        .mouseX = mouseX, 
        .mouseY = mouseY 
    };
    ctx.modeData.hitTest.hitAction = UI_ACTION_NONE;
    ProcessUI(&ctx, gameState);
    return ctx.modeData.hitTest.hitAction;
}