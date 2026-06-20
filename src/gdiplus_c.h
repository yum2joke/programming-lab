#pragma once

#include <windows.h>

#define WINGDIPAPI __stdcall


// gdipluscolor.h 에서 사용되는 MakeARGB() 구현
#define MAKE_ARGB(a, r, g, b) ((DWORD)((((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff)))


// --- 각종 자료형 연결 ---

// gdipluspixelformats.h
typedef DWORD ARGB;

// gdiplustypes.h
typedef float REAL;

// gdiplusgpstubs.h
typedef int GpStatus;
typedef struct { REAL X; REAL Y; } GpPointF;


// --- class 연결 ---

// gdiplusgpstubs.h
typedef struct GpGraphics GpGraphics;
typedef struct GpPen GpPen;
typedef struct GpBrush GpBrush;
typedef struct GpSolidFill GpSolidFill;
typedef struct GpImage GpImage;
typedef struct GpBitmap GpBitmap;
typedef struct GpLineGradient GpLineGradient;
typedef struct GpPath GpPath;
typedef struct GpPathGradient GpPathGradient;    
typedef struct GpImageAttributes GpImageAttributes;


// --- enum 연결 ---

// gdiplusenums.h
typedef enum 
{
    UnitWorld,      // 0 -- World coordinate (non-physical unit)
    UnitDisplay,    // 1 -- Variable -- for PageTransform only
    UnitPixel,      // 2 -- Each unit is one device pixel.
    UnitPoint,      // 3 -- Each unit is a printer's point, or 1/72 inch.
    UnitInch,       // 4 -- Each unit is 1 inch.
    UnitDocument,   // 5 -- Each unit is 1/300 inch.
    UnitMillimeter  // 6 -- Each unit is 1 millimeter.
} GpUnit;

typedef enum {
    SmoothingModeInvalid     = -1,
    SmoothingModeDefault     = 0,
    SmoothingModeHighSpeed   = 1,
    SmoothingModeHighQuality = 2,
    SmoothingModeNone        = 3,
    SmoothingModeAntiAlias   = 4
} SmoothingMode;

typedef enum
{
    MatrixOrderPrepend    = 0,
    MatrixOrderAppend     = 1
} GpMatrixOrder;

// gdiplusinit.h
typedef void* DebugEventProc;
typedef struct
{
    UINT32 GdiplusVersion;             // Must be 1  (or 2 for the Ex version)
    DebugEventProc DebugEventCallback; // Ignored on free builds
    BOOL SuppressBackgroundThread;     // FALSE unless you're prepared to call 
                                       // the hook/unhook functions properly
    BOOL SuppressExternalCodecs;       // FALSE unless you want GDI+ only to use
                                       // its internal image codecs.
} GdiplusStartupInput;

// gdipluscolormatrix.h
typedef struct {
    REAL m[5][5];
} ColorMatrix;


// --- 함수 연결 (__declspec(dllimport) 이용) ---

// 초기화
// gdiplusinit.h
__declspec(dllimport) GpStatus WINGDIPAPI GdiplusStartup(ULONG_PTR* token, const GdiplusStartupInput* input, void* output);
__declspec(dllimport) void WINGDIPAPI GdiplusShutdown(ULONG_PTR token);

// 그래픽스 생성 / 설정
// gdiplusflat.h
__declspec(dllimport) GpStatus WINGDIPAPI GdipCreateFromHDC(HDC hdc, GpGraphics **graphics);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDeleteGraphics(GpGraphics *graphics);
__declspec(dllimport) GpStatus WINGDIPAPI GdipSetSmoothingMode(GpGraphics *graphics, SmoothingMode smoothingMode);

// Pen
__declspec(dllimport) GpStatus WINGDIPAPI GdipCreatePen1(DWORD color, float width, GpUnit unit, GpPen **pen);
__declspec(dllimport) GpStatus WINGDIPAPI GdipCreatePen2(GpBrush *brush, REAL width, GpUnit unit, GpPen **pen);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDeletePen(GpPen *pen);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDeleteBrush(GpBrush *brush);

// 그리기(선, 타원, 채우기, 이미지)
__declspec(dllimport) GpStatus WINGDIPAPI GdipDrawLine(GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDrawEllipse(GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height);
__declspec(dllimport) GpStatus WINGDIPAPI GdipFillEllipse(GpGraphics *graphics, GpBrush *brush, REAL x, REAL y, REAL width, REAL height);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDrawImageRect(GpGraphics *graphics, GpImage *image, REAL x, REAL y, REAL width, REAL height);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDrawImageRectRect(GpGraphics *graphics, GpImage *image, REAL dstx, REAL dsty, REAL dstwidth, REAL dstheight, REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight, GpUnit srcUnit, const GpImageAttributes* imageAttributes, void* callback, void* callbackData);

// 브러시 / 그라데이션 / 경로
__declspec(dllimport) GpStatus WINGDIPAPI GdipCreateLineBrush(const GpPointF* point1, const GpPointF* point2, ARGB color1, ARGB color2, int wrapMode, GpLineGradient **lineGradient);
__declspec(dllimport) GpStatus WINGDIPAPI GdipSetLinePresetBlend(GpLineGradient *brush, const ARGB *blend, const REAL *positions, int count);
__declspec(dllimport) GpStatus WINGDIPAPI GdipCreatePath(int brushMode, GpPath **path);
__declspec(dllimport) GpStatus WINGDIPAPI GdipAddPathEllipse(GpPath *path, REAL x, REAL y, REAL width, REAL height);
__declspec(dllimport) GpStatus WINGDIPAPI GdipCreatePathGradientFromPath(const GpPath *path, GpPathGradient **polyGradient);
__declspec(dllimport) GpStatus WINGDIPAPI GdipSetPathGradientCenterColor(GpPathGradient *brush, ARGB colors);
__declspec(dllimport) GpStatus WINGDIPAPI GdipSetPathGradientSurroundColorsWithCount(GpPathGradient *brush, ARGB *color, int *count);
__declspec(dllimport) GpStatus WINGDIPAPI GdipSetPathGradientPresetBlend(GpPathGradient *brush, const ARGB *blend, const REAL *positions, int count);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDeletePath(GpPath *path);

// 이미지 로드 및 관리
__declspec(dllimport) GpStatus WINGDIPAPI GdipLoadImageFromFile(const WCHAR* filename, GpImage **image);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDisposeImage(GpImage *image);
__declspec(dllimport) GpStatus WINGDIPAPI GdipGetImageWidth(GpImage *image, UINT *width);
__declspec(dllimport) GpStatus WINGDIPAPI GdipGetImageHeight(GpImage *image, UINT *height);

// 이미지 속성 (컬러 매트릭스)
__declspec(dllimport) GpStatus WINGDIPAPI GdipCreateImageAttributes(GpImageAttributes **imageattr);
__declspec(dllimport) GpStatus WINGDIPAPI GdipDisposeImageAttributes(GpImageAttributes *imageattr);
__declspec(dllimport) GpStatus WINGDIPAPI GdipSetImageAttributesColorMatrix(GpImageAttributes *imageattr, int type, BOOL enableFlag, const ColorMatrix* colorMatrix, const ColorMatrix* grayMatrix, int flags);

// 변환 (월드 트랜스폼)
__declspec(dllimport) GpStatus WINGDIPAPI GdipTranslateWorldTransform(GpGraphics *graphics, REAL dx, REAL dy, GpMatrixOrder order);
__declspec(dllimport) GpStatus WINGDIPAPI GdipRotateWorldTransform(GpGraphics *graphics, REAL angle, GpMatrixOrder order);