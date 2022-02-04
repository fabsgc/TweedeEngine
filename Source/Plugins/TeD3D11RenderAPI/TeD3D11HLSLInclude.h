#pragma

#include "TeD3D11RenderAPIPrerequisites.h"

namespace te
{
    class D3D11HLSLInclude : public ID3DInclude
    {
    public:
        D3D11HLSLInclude(const String& directory);
        virtual ~D3D11HLSLInclude() = default;

        HRESULT __stdcall Open(
            D3D_INCLUDE_TYPE IncludeType,
            LPCSTR pFileName,
            LPCVOID pParentData,
            LPCVOID *ppData,
            UINT *pBytes) override;

        HRESULT __stdcall Close(LPCVOID pData)  override;

    protected:
        String GetFullPath(String relativePath);

    protected:
        String _directory;
        String _data;
    };
}
