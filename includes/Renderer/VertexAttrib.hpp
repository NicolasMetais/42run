#pragma once
#include <../loader/include/glad/glad.h>

/** @brief Fixed OpenGL vertex attribute locations shared between the CPU and all shaders. */
namespace VertexAttrib
{
	constexpr GLuint Position  = 0;
	constexpr GLuint Normal    = 1;
	constexpr GLuint Tangent   = 2;
	constexpr GLuint TexCoord0 = 3;
	constexpr GLuint TexCoord1 = 4;
	constexpr GLuint Color0    = 5;
	constexpr GLuint Color1    = 6;
}
