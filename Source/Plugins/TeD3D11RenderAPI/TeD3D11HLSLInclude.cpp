#include "TeD3D11HLSLInclude.h"
#include "Utility/TeDataStream.h"

#include <filesystem>

namespace te
{
    D3D11HLSLInclude::D3D11HLSLInclude(const String& directory)
        : _directory(directory)
    { }

    HRESULT __stdcall D3D11HLSLInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
    {
        String includePath = std::filesystem::absolute(_directory).generic_string();

        switch (IncludeType)
        {
        case D3D_INCLUDE_LOCAL: // #include "FILE"
            includePath += String(pFileName);
            break;
        case D3D_INCLUDE_SYSTEM: // #include "<FILE>"
            includePath += String(pFileName);
            break;
        default:
            TE_ASSERT_ERROR(false, "Only local and system directory is currently supported for HLSL includes");
            break;
        }

        FileStream includeFile(includePath);
        TE_ASSERT_ERROR(!includeFile.Fail(), "Can't open include file {" + includePath + "}");

        _data = includeFile.GetAsString();
        *ppData = _data.c_str();
        *pBytes = (UINT)includeFile.Size();

        return S_OK;
    }

    HRESULT __stdcall D3D11HLSLInclude::Close(LPCVOID pData)
    {
        return S_OK;
    }
}
