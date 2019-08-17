#include "Private/Linux/TeLinuxInput.h"
#include "Private/Linux/TeLinuxPlatform.h"
#include "Private/Linux/TeLinuxWindow.h"
#include "RenderAPI/TeRenderWindow.h"
#include "TeCoreApplication.h"

namespace te
{
    Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::OnCursorMoved;
	Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::OnCursorButtonPressed;
	Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::OnCursorButtonReleased;
	Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::OnCursorDoubleClick;
	Event<void(float)> Platform::OnMouseWheelScrolled;
	Event<void(UINT32)> Platform::OnCharInput;

    struct Platform::Pimpl
	{
    };

	void Platform::Update()
	{
	}
}