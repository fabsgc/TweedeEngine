#include "TeD3D11HLSLInclude.h"
#include "Utility/TeFileStream.h"

namespace te
{
    D3D11HLSLInclude::D3D11HLSLInclude(const String& directory)
        : _directory(directory)
    { }

    HRESULT __stdcall D3D11HLSLInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
    {
        String includePath;
        switch (IncludeType)
        {
        case D3D_INCLUDE_LOCAL: // #include "FILE"
            includePath = GetFullPath(_directory) + "\\" + String(pFileName);
            break;
        case D3D_INCLUDE_SYSTEM: // #include "<FILE>"
            includePath = GetFullPath(_directory) + "\\" + String(pFileName);
            break;
        default:
            TE_ASSERT_ERROR(false, "Only local and system directory is currently supported for HLSL includes", __FILE__, __LINE__);
            break;
        }

        FileStream includeFile(includePath.c_str());
        TE_ASSERT_ERROR(!includeFile.Fail(), "Can't open include file {" + includePath + "}", __FILE__, __LINE__);

        _data = includeFile.GetAsString();
        *ppData = _data.c_str();
        *pBytes = (UINT)includeFile.Size();

        return S_OK;
    }

    HRESULT __stdcall D3D11HLSLInclude::Close(LPCVOID pData)
    {
        return S_OK;
    }

    String D3D11HLSLInclude::GetFullPath(String relativePath)
    {
        DWORD  retval = 0;
        TCHAR  buffer[512] = TEXT("");
        TCHAR  buf[512]    = TEXT("");
        TCHAR** lppPart    = { NULL };

        retval = GetFullPathName(relativePath.c_str(), 512, buffer, lppPart);
        TE_ASSERT_ERROR((retval != 0), "GetFullPathName failed", __FILE__, __LINE__);

        return buffer;
    }
}
