#pragma once

#include <Common/Common.h>
#include <glad/gl.h>    //need to include before <GLFW/glfw3.h>

#include "SpaghettiEng/Core/Application.h"
#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/KeyCodes.h"
#include "SpaghettiEng/Core/MouseCodes.h"
#include "SpaghettiEng/Core/Input.h"
#include "SpaghettiEng/Core/Layer.h"
#include "SpaghettiEng/Events/EventManager.h"
#include "SpaghettiEng/ImGuiUtils/ImGuiUtils.h"

#include "SpaghettiEng/Camera/Camera2D.h"
#include "SpaghettiEng/Camera/CameraController2D.h"
#include "SpaghettiEng/OpenGL32/GL32BufferLayout.h"
#include "SpaghettiEng/OpenGL32/GL32Buffer.h"
#include "SpaghettiEng/OpenGL32/GL32VertexArray.h"
#include "SpaghettiEng/OpenGL32/GL32Shader.h"
#include <SpaghettiEng/OpenGL32/GL32Renderer.h>
#include <SpaghettiEng/OpenGL32/GL32TextRenderer.h>