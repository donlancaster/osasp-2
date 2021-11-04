#include <Windows.h>
#include <vector>
#include <algorithm>
#include "TableReader.h"

char filePath[256] = R"(G:\BSUIR\5 sem\OSaSP\Lab2 Main\file.txt)";
using StringTable = std::vector<std::vector<std::string>>;
StringTable text;
std::vector<size_t> maxRowHeights;
size_t windowWidth;
size_t windowHeight;

size_t sum(const std::vector<size_t> &vect);

void DrawTableText(HDC hDC);

void DrawLines(HDC hDC);

void RedrawDrawAreaBackground(HDC hDC);

void DrawTable(HDC hdc);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hwnd;// дескриптор окна
    MSG msg; //сообщение (ввод пользователем чего-то)
    WNDCLASSEX wc; //информация о классе окна
    wc.style =// стиль класса окна
            CS_HREDRAW //перерисовывает все окно, если перемещение или регулировка размера изменяют ширину рабочей области.
            | CS_VREDRAW;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hIcon = LoadIcon(nullptr, IDI_HAND);
    wc.hIconSm = LoadIcon(nullptr, IDI_HAND);
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.hInstance = hinstance;//дескриптор окна
    wc.cbClsExtra = 0;//доп байты
    wc.cbWndExtra = 0;
    wc.lpfnWndProc = WndProc;//оконная процедура
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "WinMainClass";

    RegisterClassEx(&wc);//регистрирует класс окна для последующего использования в CreateWindow()
    hwnd = CreateWindow(
            wc.lpszClassName,//зарегистрированное имя класса
            "tabli4k1n",//название окна
            WS_OVERLAPPEDWINDOW,//какой-то стиль окна
            0, 0, 800, 600, 0, 0, hinstance, nullptr);
    ShowWindow(hwnd, nCmdShow); //устанавливает состояние показа окна
    UpdateWindow(hwnd);//обновляет окно, вызывает WM_PAINT

    // цикл обработки сообщений
    while (GetMessage(&msg, nullptr, 0,
                      0)) {//getMessage ловит сообщение из очереди и блокирует поток, если сообщений большне нет
        TranslateMessage(&msg); //переводит это сообщение для системы ?
        DispatchMessage(&msg); //отправляет сообщение в оконную процедуру wndProc
    }
    return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            TableReader rd;
            rd.setFilePath(filePath);
            try {
                text = move(rd.getText());
                InvalidateRect(hWnd, NULL, TRUE);
            }
            catch (...) {
                MessageBox(hWnd, "error reading file", "error", MB_ICONERROR);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawTable(hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_SIZE: {
            windowWidth = LOWORD(lParam);
            windowHeight = HIWORD(lParam);
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}




void DrawTableText(HDC hDC) {
    const size_t rows = text.size();
    const size_t columns = text[0].size();
    const LONG columnWidth = windowWidth / columns;
    maxRowHeights.resize(rows);

    size_t fontSize = 1;
    RECT rect = {0, 0, columnWidth, columnWidth};
    HFONT font;
    static LOGFONT logFont;
    logFont.lfHeight = fontSize;
    font = CreateFontIndirect(&logFont);
    HFONT oldFont = (HFONT) SelectObject(hDC, font);

    bool biggestFontNotFound = true;

    while (biggestFontNotFound) {

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                rect = {0, 0, columnWidth, columnWidth};
                DrawText(hDC, text[i][j].c_str(), -1, &rect, DT_WORDBREAK | DT_LEFT | DT_CALCRECT);
                size_t calculatedWidth = rect.right - rect.left;
                if (calculatedWidth > columnWidth) {
                    biggestFontNotFound = false;
                    break;
                }
            }
            if (!biggestFontNotFound) break;
        }
        if (!biggestFontNotFound) break;

        fontSize += 1;
        logFont.lfHeight = fontSize;
        DeleteObject(font);
        font = CreateFontIndirect(&logFont);
        SelectObject(hDC, font);
    }

    fontSize -= 1;
    font = (HFONT) SelectObject(hDC, oldFont);
    if (font) DeleteObject(font);
    logFont.lfHeight = fontSize;
    font = CreateFontIndirect(&logFont);
    oldFont = (HFONT) SelectObject(hDC, font);

    std::vector<size_t> heights(columns);

    while (true) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                rect = {0, 0, columnWidth, columnWidth};
                DrawText(hDC, text[i][j].c_str(), -1, &rect, DT_WORDBREAK | DT_LEFT | DT_CALCRECT);
                size_t calculated_height = rect.bottom - rect.top;
                heights[j] = calculated_height;
            }
            sort(heights.begin(), heights.end());
            maxRowHeights[i] = heights[heights.size() - 1];
        }
        size_t sum = 0;
        for (auto &it : maxRowHeights) {
            sum += it;
        }
        if (sum < windowHeight) break;

        fontSize -= 1;
        logFont.lfHeight = fontSize;
        DeleteObject(font);
        font = CreateFontIndirect(&logFont);
        SelectObject(hDC, font);
    }

    long additionalHeight = (windowHeight - sum(maxRowHeights)) / maxRowHeights.size();

    RECT cell = {0, 0, columnWidth, 0};
    for (size_t i = 0; i < rows; ++i) {
        cell.bottom += maxRowHeights[i];
        cell.bottom += additionalHeight;
        for (size_t j = 0; j < columns; ++j) {
            DrawText(hDC, text[i][j].c_str(), -1, &cell, DT_WORDBREAK | DT_BOTTOM);
            cell.left += columnWidth;
            cell.right += columnWidth;
        }
        cell.left = 0;
        cell.right = columnWidth;
        cell.top += maxRowHeights[i];
        cell.top += additionalHeight;
    }
    font = (HFONT) SelectObject(hDC, oldFont);
    if (font) DeleteObject(font);
}

size_t sum(const std::vector<size_t> &vect) {
    size_t sum = 0;
    for (auto it : vect) sum += it;
    return sum;
}

void DrawLines(HDC hDC) {
//    if (windowWidth < 20) return;
//    if (windowHeight < 20) return;
    size_t wholeHeight = 0;
    for (auto &it : maxRowHeights) {
        wholeHeight += it;
    }
    size_t columns = text[0].size();
    size_t rows = text.size();
    size_t columnWidth = windowWidth / columns;
    size_t changingX = columnWidth;
    for (size_t i = 0; i < columns; ++i) {
        MoveToEx(hDC, changingX, 0, NULL);
        LineTo(hDC, changingX, windowHeight);
        changingX += columnWidth;
    }

    size_t changingY = 0;
    long additional_height = (windowHeight - sum(maxRowHeights)) / maxRowHeights.size();
    for (size_t i = 0; i < rows; ++i) {
        changingY += maxRowHeights[i];
        changingY += additional_height;
        MoveToEx(hDC, 0, changingY, NULL);
        LineTo(hDC, windowWidth, changingY);
    }
}

void RedrawDrawAreaBackground(HDC hDC) {
    RECT rect;
    SetRect(&rect, 0, 0, windowWidth, windowHeight);
    FillRect(hDC, &rect, (HBRUSH) (WHITE_BRUSH));
}

void DrawTable(HDC hdc) {
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP memBM = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
    SelectObject(hMemDC, memBM);

    RedrawDrawAreaBackground(hMemDC);
    DrawTableText(hMemDC);
    DrawLines(hMemDC);


    BitBlt(hdc, 0, 0, windowWidth, windowHeight, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
    DeleteObject(memBM);
}
