#include "framework.h"
#include "Projekt_4_Szymon_203618_Maciek_203894.h"
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <commctrl.h> 
#pragma comment(lib, "comctl32.lib")

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100
#define ID_FILTER_ALL 1001
#define ID_FILTER_SQUARES 1002
#define ID_FILTER_CIRCLES 1003
#define ID_FILTER_TRIANGLES 1004
#define ID_GEN_ALL 2001
#define ID_GEN_CIRCLE_ONLY 2002
#define ID_GEN_TRIANGLE_ONLY 2003
#define ID_SLIDER_SPEED     3001
#define ID_SLIDER_FREQUENCY 3002
#define ID_GEN_CIRCLE_SQUARE 2004
#define ID_GEN_TRIANGLE_SQUARE 2005
#define ID_GEN_SQUARE_ONLY 2006
#define ID_OPTION_1 4001
#define ID_OPTION_2 4002
#define ID_OPTION_3 4003
#define ID_OPTION_4 4004

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

int generationCounter = 0;
int movementSpeed = 3;  
int generationRate = 15;    
int currentCombinedOption = 1;
int correctProducts = 0;
int incorrectProducts = 0;
int trianglesCompleted = 0;
int squaresCompleted = 0;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

enum State { MovingTop, Falling, PulledUp, SecondFall, SlidingDown, Filtering, Filtered };
enum Mode { GENERATE_ALL, GENERATE_CIRCLES, GENERATE_TRIANGLES, GENERATE_CIRCLE_SQUARE, GENERATE_TRIANGLE_SQUARE, GENERATE_SQUARES };
enum FilterType { FILTER_NONE, FILTER_SQUARES, FILTER_CIRCLES, FILTER_TRIANGLES };

Mode currentMode = GENERATE_ALL;
FilterType currentFilter = FILTER_NONE;

struct Figure {
    enum Type { Circle, Square, Triangle } type;
    float x, y;
    State state;
    float progress;
    float filterTime;

    Figure()
        : type(Circle),
        x(0.0f),
        y(0.0f),
        state(MovingTop),
        progress(0.0f),
        filterTime(0.0f)
    {
    }
};

const PointF linePullStart(1000, 275);
const PointF linePullEnd(450, 125);
const PointF slideStart(500, 200);
const PointF slideEnd(0, 300);
const float pullSpeed = 0.005f;
const float slideSpeed = 0.004f;
const float fallSpeed = 5.0f;
const float offsetY = -15.0f;

std::vector<Figure> figures;

Figure CreateFigure() {
    Figure f;

    switch (currentMode) {
    case GENERATE_ALL:
        switch (rand() % 3) {
        case 0: f.type = Figure::Circle; break;
        case 1: f.type = Figure::Square; break;
        case 2: f.type = Figure::Triangle; break;
        }
        break;
    case GENERATE_SQUARES:
        f.type = Figure::Square;
        break;
    case GENERATE_TRIANGLE_SQUARE:
        f.type = (rand() % 2 == 0) ? Figure::Triangle : Figure::Square;
        break;
    case GENERATE_CIRCLES:
        f.type = Figure::Circle;
        break;
    case GENERATE_TRIANGLES:
        f.type = Figure::Triangle;
        break;
    case GENERATE_CIRCLE_SQUARE:
        f.type = (rand() % 2 == 0) ? Figure::Circle : Figure::Square;
        break;
    }

    f.x = 1325;
    f.y = 70;
    f.state = MovingTop;
    f.progress = 0.0f;
    f.filterTime = 0.0f;
    return f;
}

void DrawFigure(Graphics& g, const Figure& f) {
    SolidBrush brush(
        f.type == Figure::Circle ? Color(255, 255, 255, 0) :
        f.type == Figure::Square ? Color(255, 0, 0, 255) :
        Color(255, 0, 255, 0));

    if (f.type == Figure::Circle)
        g.FillEllipse(&brush, (int)f.x, (int)f.y, 30, 30);
    else if (f.type == Figure::Square)
        g.FillRectangle(&brush, (int)f.x, (int)f.y, 30, 30);
    else if (f.type == Figure::Triangle) {
        Point points[3] = {
            Point((int)f.x + 15, (int)f.y),
            Point((int)f.x, (int)f.y + 30),
            Point((int)f.x + 30, (int)f.y + 30)
        };
        g.FillPolygon(&brush, points, 3);
    }
}

VOID OnPaint(HDC hdc) {
    RECT clientRect;
    GetClientRect(WindowFromDC(hdc), &clientRect);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    Graphics graphics(memDC);
    graphics.Clear(Color(255, 250, 235, 215));


    SolidBrush fillBrush(Color(255, 255, 105, 180));
    Point t1[4] = { Point(0, 300), Point(0, 325), Point(500, 225), Point(500, 200) };
    graphics.FillPolygon(&fillBrush, t1, 4);
    Point t2[4] = { Point(450, 150), Point(450, 125), Point(1000, 275), Point(1000, 300) };
    graphics.FillPolygon(&fillBrush, t2, 4);
    graphics.FillRectangle(&fillBrush, 950, 100, 400, 25);

    for (const auto& f : figures)
        DrawFigure(graphics, f);

    SolidBrush filterBrush(Color(255, 192, 192, 192));
    graphics.FillRectangle(&filterBrush, 850, 100, 100, 100);

    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
    SolidBrush textBrush(Color(255, 0, 0, 0));
    graphics.DrawString(L"FILTR", -1, &font, PointF(880, 140), &textBrush);
    WCHAR stats[100];
    swprintf(stats, 100, L"Poprawne: %d   Wadliwe: %d", correctProducts, incorrectProducts);
    graphics.DrawString(stats, -1, &font, PointF(20, 60), &textBrush);
    int completedProducts = min(trianglesCompleted, squaresCompleted);
    int faultyProducts = abs(trianglesCompleted - squaresCompleted);

    WCHAR productStats[100];
    swprintf(productStats, 100, L"Poprawne produkty: %d   Wadliwe: %d", completedProducts, faultyProducts);
    graphics.DrawString(productStats, -1, &font, PointF(20, 80), &textBrush);

    BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

float CalculateProgress(float x, PointF start, PointF end) {
    if (fabs(end.X - start.X) < 1.0f) return 0.0f;
    return (x - start.X) / (end.X - start.X);
}

VOID OnTimer(HWND hWnd) {
    std::vector<Figure> updated;

    const int filterX1 = 850;
    const int filterX2 = 950;
    const int filterY1 = 100;
    const int filterY2 = 200;

    const float timerIntervalMs = 30.0f;
    const float requiredFilterTimeMs = 1000.0f;

    for (auto& f : figures) {
        if (f.state == Filtered)
            continue;

        float centerX = f.x + 15;
        float centerY = f.y + 15;

        bool inFilterZone = (
            centerX >= filterX1 && centerX <= filterX2 &&
            centerY >= filterY1 && centerY <= filterY2
            );

        bool matchesFilter = false;
        switch (currentFilter) {
        case FILTER_SQUARES:   matchesFilter = (f.type == Figure::Square); break;
        case FILTER_CIRCLES:   matchesFilter = (f.type == Figure::Circle); break;
        case FILTER_TRIANGLES: matchesFilter = (f.type == Figure::Triangle); break;
        default: break;
        }

        if (currentFilter != FILTER_NONE && inFilterZone && matchesFilter) {
            if (f.state != Filtering) {
                f.state = Filtering;
                f.y += 25;
            }

            f.filterTime += timerIntervalMs;

            if (f.filterTime >= requiredFilterTimeMs) {
                if (movementSpeed == 4 && generationRate == 15)
                    correctProducts++;
                else
                    incorrectProducts++;

                f.state = Filtered;
                continue;
            }
        }
        else {
            if (f.state == Filtering)
                f.state = Falling;
            f.filterTime = 0.0f;
        }

        // Ruch
        switch (f.state) {
        case MovingTop:
            f.x -= movementSpeed;
            if (f.x <= 920)
                f.state = Falling;
            break;

        case Falling:
            f.y += fallSpeed * (movementSpeed / 3.0f);
            if (f.y + 30 >= 275) {
                f.state = PulledUp;
                f.progress = CalculateProgress(f.x, linePullStart, linePullEnd);
            }
            break;

        case PulledUp:
            f.progress += pullSpeed * (movementSpeed / 3.0f);
            f.x = linePullStart.X + (linePullEnd.X - linePullStart.X) * f.progress;
            f.y = linePullStart.Y + (linePullEnd.Y - linePullStart.Y) * f.progress + offsetY;
            if (f.progress >= 1.05f)
                f.state = SecondFall;
            break;

        case SecondFall:
            f.y += fallSpeed * (movementSpeed / 3.0f);
            if (f.y + 30 >= 200) {
                f.state = SlidingDown;
                f.progress = CalculateProgress(f.x, slideStart, slideEnd);
            }
            break;

        case SlidingDown:
            f.progress += slideSpeed * (movementSpeed / 3.0f);
            f.x = slideStart.X + (slideEnd.X - slideStart.X) * f.progress;
            f.y = slideStart.Y + (slideEnd.Y - slideStart.Y) * f.progress + offsetY;
            break;

        case Filtering:
            break;

        case Filtered:
            break;
        }

        if (f.state == SlidingDown && f.progress >= 1.0f) {
            if (f.type == Figure::Triangle)
                trianglesCompleted++;
            else if (f.type == Figure::Square)
                squaresCompleted++;
            continue;
        }
        else {
            updated.push_back(f);
        }
    }

    figures = updated;

    generationCounter++;
    if (generationCounter >= generationRate) {
        figures.push_back(CreateFigure());
        generationCounter = 0;
    }

    InvalidateRect(hWnd, NULL, TRUE);
}


void AddMenus(HWND hWnd) {
    HMENU hMenubar = CreateMenu();
    HMENU hFilterMenu = CreateMenu();
    HMENU hGenerateMenu = CreateMenu();

    AppendMenuW(hFilterMenu, MF_STRING, ID_FILTER_ALL, L"Brak filtra");
    AppendMenuW(hFilterMenu, MF_STRING, ID_FILTER_SQUARES, L"Filtruj kwadraty");
    AppendMenuW(hFilterMenu, MF_STRING, ID_FILTER_CIRCLES, L"Filtruj koła");
    AppendMenuW(hFilterMenu, MF_STRING, ID_FILTER_TRIANGLES, L"Filtruj trójkąty");

    AppendMenuW(hGenerateMenu, MF_STRING, ID_GEN_CIRCLE_SQUARE, L"Tylko koło i kwadrat");
    AppendMenuW(hGenerateMenu, MF_STRING, ID_GEN_TRIANGLE_SQUARE, L"Tylko trójkąt i kwadrat");
    AppendMenuW(hGenerateMenu, MF_STRING, ID_GEN_ALL, L"Wszystko");
    AppendMenuW(hGenerateMenu, MF_STRING, ID_GEN_SQUARE_ONLY, L"Tylko kwadraty");
    AppendMenuW(hGenerateMenu, MF_STRING, ID_GEN_CIRCLE_ONLY, L"Tylko koła");
    AppendMenuW(hGenerateMenu, MF_STRING, ID_GEN_TRIANGLE_ONLY, L"Tylko trójkąty");

    AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hFilterMenu, L"Typ filtra");
    AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hGenerateMenu, L"Generowanie");

    SetMenu(hWnd, hMenubar);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJEKT4SZYMON203618MACIEK203894, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJEKT4SZYMON203618MACIEK203894));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJEKT4SZYMON203618MACIEK203894));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJEKT4SZYMON203618MACIEK203894);
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    AddMenus(hWnd);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
    {
        srand((unsigned)time(NULL));
        SetTimer(hWnd, 1, 30, NULL);

        InitCommonControls();

        HWND hSliderSpeed = CreateWindowW(TRACKBAR_CLASS, L"Speed",
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            20, 20, 200, 30, hWnd, (HMENU)ID_SLIDER_SPEED,
            hInst, NULL);
        SendMessage(hSliderSpeed, TBM_SETRANGE, TRUE, MAKELPARAM(1, 10));
        SendMessage(hSliderSpeed, TBM_SETPOS, TRUE, movementSpeed);

        HWND hSliderFreq = CreateWindowW(TRACKBAR_CLASS, L"Freq",
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            250, 20, 200, 30, hWnd, (HMENU)ID_SLIDER_FREQUENCY,
            hInst, NULL);
        SendMessage(hSliderFreq, TBM_SETRANGE, TRUE, MAKELPARAM(1, 30));
        SendMessage(hSliderFreq, TBM_SETPOS, TRUE, generationRate);
        CreateWindowW(L"STATIC", L"Szybkość ruchu",
            WS_CHILD | WS_VISIBLE,
            20, 0, 150, 20,
            hWnd, NULL, hInst, NULL);

        CreateWindowW(L"STATIC", L"Częstotliwość generowania",
            WS_CHILD | WS_VISIBLE,
            250, 0, 200, 20,
            hWnd, NULL, hInst, NULL);
        CreateWindowW(L"BUTTON", L"Koło + Kwadrat | filtr: Kwadrat",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            500, 20, 300, 20, hWnd, (HMENU)ID_OPTION_1, hInst, NULL);

        CreateWindowW(L"BUTTON", L"Koło + Kwadrat | filtr: Koło",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            500, 40, 300, 20, hWnd, (HMENU)ID_OPTION_2, hInst, NULL);

        CreateWindowW(L"BUTTON", L"Trójkąt + Kwadrat | filtr: Trójkąt",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            500, 60, 300, 20, hWnd, (HMENU)ID_OPTION_3, hInst, NULL);

        CreateWindowW(L"BUTTON", L"Trójkąt + Kwadrat | filtr: Kwadrat",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            500, 80, 300, 20, hWnd, (HMENU)ID_OPTION_4, hInst, NULL);

        
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_GEN_SQUARE_ONLY:
            currentMode = GENERATE_SQUARES;
            break;
        case ID_GEN_CIRCLE_SQUARE:
            currentMode = GENERATE_CIRCLE_SQUARE;
            break;
        case ID_FILTER_ALL:
            currentFilter = FILTER_NONE;
            break;
        case ID_FILTER_SQUARES:
            currentFilter = FILTER_SQUARES;
            break;
        case ID_FILTER_CIRCLES:
            currentFilter = FILTER_CIRCLES;
            break;
        case ID_FILTER_TRIANGLES:
            currentFilter = FILTER_TRIANGLES;
            break;
        case ID_GEN_ALL:
            currentMode = GENERATE_ALL;
            break;
        case ID_GEN_CIRCLE_ONLY:
            currentMode = GENERATE_CIRCLES;
            break;
        case ID_GEN_TRIANGLE_ONLY:
            currentMode = GENERATE_TRIANGLES;
            break;
        case ID_GEN_TRIANGLE_SQUARE:
            currentMode = GENERATE_TRIANGLE_SQUARE;
            break;
        case ID_OPTION_1:
            currentCombinedOption = 1;
            currentMode = GENERATE_CIRCLE_SQUARE;
            currentFilter = FILTER_SQUARES;
            break;
        case ID_OPTION_2:
            currentCombinedOption = 2;
            currentMode = GENERATE_CIRCLE_SQUARE;
            currentFilter = FILTER_CIRCLES;
            break;
        case ID_OPTION_3:
            currentCombinedOption = 3;
            currentMode = GENERATE_TRIANGLE_SQUARE;
            currentFilter = FILTER_TRIANGLES;
            break;
        case ID_OPTION_4:
            currentCombinedOption = 4;
            currentMode = GENERATE_TRIANGLE_SQUARE;
            currentFilter = FILTER_SQUARES;
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_TIMER:
        OnTimer(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_HSCROLL:
    {
        HWND hwndScroll = (HWND)lParam;
        int pos = (int)SendMessage(hwndScroll, TBM_GETPOS, 0, 0);

        int id = GetDlgCtrlID(hwndScroll);
        if (id == ID_SLIDER_SPEED) {
            movementSpeed = pos;
        }
        else if (id == ID_SLIDER_FREQUENCY) {
            generationRate = pos;
        }
        correctProducts = 0;
        incorrectProducts = 0;
    }
    break;
    case WM_ERASEBKGND:
        return 1;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
