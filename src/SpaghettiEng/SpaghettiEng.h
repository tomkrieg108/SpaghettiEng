#pragma once

#include "CoreLib/Core.h"
#include <glad/gl.h>    //need to include before <GLFW/glfw3.h>

#include "SpaghettiEng/Core/Application.h"
#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/KeyCodes.h"
#include "SpaghettiEng/Core/MouseCodes.h"
#include "SpaghettiEng/Core/InputState.h"
#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/Layer.h"

#include "SpaghettiEng/Events/EventManager.h"
#include "SpaghettiEng/ImGuiUtils/ImGuiUtils.h"

#include "SpaghettiEng/Render/Camera/Camera2D.h"
#include "SpaghettiEng/Render/Camera/CameraController2D.h"

#include "SpaghettiEng/Render/Backends/OpenGL/GLBuffer.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLVertexArray.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLTextRenderer.h"