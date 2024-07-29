#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

#ifndef IMGUI_API
#define IMGUI_API TE_PLUGIN_EXPORT
#endif

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include <d3d11.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>

namespace te
{ }
