#ifndef PLAYER_SPLASH_H
#define PLAYER_SPLASH_H

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

class CSplash
{
public:
    CSplash();
    CSplash(HINSTANCE hInstance);
    ~CSplash();

    void Show();
    bool LoadBMP(LPCSTR lpFileName);
    DWORD GetWidth() const;
    DWORD GetHeight() const;
    DWORD GetBPP() const;
    DWORD GetDIBHeaderSize() const;
    DWORD GetPaletteNumColors() const;
    WORD GetPaletteNumEntries() const;
    DWORD GetPaletteSize() const;
    BYTE *GetPaletteData() const;
    BYTE *GetPaletteEntry(DWORD index) const;
    BITMAPINFO *GetBitmapInfo() const;
    BYTE *GetBitmapData() const;
    HPALETTE GetPalette() const;

private:
    CSplash(const CSplash &);
    CSplash &operator=(const CSplash &);

    BYTE *m_Data;
    HWND m_hWnd;
    HINSTANCE m_hInstance;

    static LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif /* PLAYER_SPLASH_H */