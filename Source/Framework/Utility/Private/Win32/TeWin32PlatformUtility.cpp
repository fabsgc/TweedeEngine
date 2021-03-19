#include "Prerequisites/TePrerequisitesUtility.h"
#include "Private/Win32/TeWin32PlatformUtility.h"
#include "Utility/TePlatformUtility.h"
#include "Image/TeColor.h"
#include <windows.h>
#include <iphlpapi.h>
#include <intrin.h>

#if defined(TE_WIN_SDK_10) && defined(_MSC_VER)
#   include <Rpc.h>
#   pragma comment(lib, "Rpcrt4.lib")
#endif

namespace te
{
    void PlatformUtility::Terminate(bool force)
    {
        if (!force)
            PostQuitMessage(0);
        else
            TerminateProcess(GetCurrentProcess(), 0);
    }

    UUID PlatformUtility::GenerateUUID()
    {
        ::UUID uuid;
        RPC_STATUS status = UuidCreate(&uuid);

        if (status == RPC_S_OK)
        {
            // Endianess might not be correct, but it shouldn't matter
            UINT32 data1 = uuid.Data1;
            UINT32 data2 = uuid.Data2 | (uuid.Data3 << 16);
            UINT32 data3 = uuid.Data3 | (uuid.Data4[0] << 16) | (uuid.Data4[1] << 24);
            UINT32 data4 = uuid.Data4[2] | (uuid.Data4[3] << 8) | (uuid.Data4[4] << 16) | (uuid.Data4[5] << 24);

            return UUID(data1, data2, data3, data4);
        }

        return UUID::EMPTY;
    }

    HBITMAP Win32PlatformUtility::CreateBitmap(const Color* pixels, UINT32 width, UINT32 height, bool premultiplyAlpha)
    {
        BITMAPINFO bi;

        ZeroMemory(&bi, sizeof(BITMAPINFO));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        HDC hDC = GetDC(nullptr);

        void* data = nullptr;
        HBITMAP hBitmap = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, (void**)&data, nullptr, 0);

        HDC hBitmapDC = CreateCompatibleDC(hDC);
        ReleaseDC(nullptr, hDC);

        //Select the bitmaps to DC
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, hBitmap);

        //Scan each pixel of the source bitmap and create the masks
        Color pixel;
        DWORD *dst = (DWORD*)data;
        for (UINT32 y = 0; y < height; ++y)
        {
            for (UINT32 x = 0; x < width; ++x)
            {
                UINT32 revY = height - y - 1;
                pixel = pixels[revY * width + x];

                if (premultiplyAlpha)
                {
                    pixel.r *= pixel.a;
                    pixel.g *= pixel.a;
                    pixel.b *= pixel.a;
                }

                *dst = pixel.GetAsBGRA();

                dst++;
            }
        }

        SelectObject(hBitmapDC, hOldBitmap);
        DeleteDC(hBitmapDC);

        return hBitmap;
    }
}
