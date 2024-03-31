#include "stdafx.h"
#include <windows.h>

#include "GL/glew.h"

#include "TextureBlitter.h"

static const char vsh[] = R"(
#version 120
attribute vec4 a_VertexCoords;
attribute vec2 a_TexCoords;
uniform mat4 u_VertexMatrix;
varying vec2 v_TexCoords;
void main()
{
    gl_Position = u_VertexMatrix * vec4(a_VertexCoords.x, a_VertexCoords.y, a_VertexCoords.z, 1.0);
    v_TexCoords = a_TexCoords;
}
)";

static const char fsh[] = R"(
#version 120
uniform sampler2D u_Texture;
uniform ivec4 u_Channel;
uniform float u_opacity;
varying vec2 v_TexCoords;
void main()
{
    vec4 c = texture2D(u_Texture, v_TexCoords);
    gl_FragColor = vec4(c[u_Channel[0]], c[u_Channel[1]], c[u_Channel[2]], c[u_Channel[3]]*u_opacity);
}
)";

static uint32_t LoadShaderPairSrc(const char *szVertexSrc, const char *szFragmentSrc) {
	// Temporary Shader objects
	GLuint hVertexShader;
	GLuint hFragmentShader;
	GLuint hReturn = 0;
	GLint testVal;
	// Create shader objects
	hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load them. 
	GLchar *fsStringPtr[1];

	fsStringPtr[0] = (GLchar *)szVertexSrc;
	glShaderSource(hVertexShader, 1, (const GLchar **)fsStringPtr, NULL);
	fsStringPtr[0] = (GLchar *)szFragmentSrc;
	glShaderSource(hFragmentShader, 1, (const GLchar **)fsStringPtr, NULL);

	//gltLoadShaderSrc(szVertexSrc, hVertexShader);
	//gltLoadShaderSrc(szFragmentSrc, hFragmentShader);

	// Compile them
	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for errors
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetShaderInfoLog(hVertexShader, 1024, NULL, infoLog);

		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)NULL;
	}

	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetShaderInfoLog(hFragmentShader, 1024, NULL, infoLog);

		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)NULL;
	}

	// Link them - assuming it works...
	hReturn = glCreateProgram();
	glAttachShader(hReturn, hVertexShader);
	glAttachShader(hReturn, hFragmentShader);
	glLinkProgram(hReturn);

	// These are no longer needed
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);

	// Make sure link worked too
	glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(hReturn, 1024, NULL, infoLog);

		glDeleteProgram(hReturn);
		return (GLuint)NULL;
	}

	return hReturn;
}

TextureBlitter::~TextureBlitter() {
  if (program_)
    glDeleteProgram(program_);

  if (vbo_)
    glDeleteBuffers(1, &vbo_);
}

bool TextureBlitter::blit(unsigned texture, const float* targetMat4) {
	if (!program_) {
		// TODO: 此处的program_要重新编写创建代码 2022/10/29 17:28:00
		program_ = LoadShaderPairSrc(vsh, fsh);
		if (!program_)
			return true;

		sampler_ = glGetUniformLocation(program_, "u_Texture");
		mat_loc_ = glGetUniformLocation(program_, "u_VertexMatrix");
		opacity_loc_ = glGetUniformLocation(program_, "u_opacity");
		channel_loc_ = glGetUniformLocation(program_, "u_Channel");
		a_loc_[0] = glGetAttribLocation(program_, "a_VertexCoords"); // fixed location after link
		a_loc_[1] = glGetAttribLocation(program_, "a_TexCoords");
	}

    glUseProgram(program_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    static const float I4x4[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f };
    if (!targetMat4) {
        targetMat4 = I4x4;
    }

    glUniformMatrix4fv(mat_loc_, 1, GL_FALSE, targetMat4);
    glUniform4iv(channel_loc_, 1, channel_);
    float opacity = (float)(src_opacity_ ) / 100;
    glUniform1f(opacity_loc_, opacity);
 
    //const float vbo_data[] = {
    //  view_vert_[0], view_vert_[1], tex_vert_[0], tex_vert_[1], // TL
    //  view_vert_[0], view_vert_[3], tex_vert_[0], tex_vert_[3], // BL
    //  view_vert_[2], view_vert_[1], tex_vert_[2], tex_vert_[1], // TR
    //  view_vert_[2], view_vert_[3], tex_vert_[2], tex_vert_[3], // BR
    //};

    const float vbo_data[] = {
        view_vert_[2], view_vert_[1], 0.0f, tex_vert_[2], tex_vert_[1], //top right
		view_vert_[2], view_vert_[3], 0.0f, tex_vert_[2], tex_vert_[3], //bottom right
		view_vert_[0], view_vert_[1], 0.0f, tex_vert_[0], tex_vert_[1], //top left

		view_vert_[2], view_vert_[3], 0.0f, tex_vert_[2], tex_vert_[3], //bottom right
		view_vert_[0], view_vert_[3], 0.0f, tex_vert_[0], tex_vert_[3], //bottom left
		view_vert_[0], view_vert_[1], 0.0f, tex_vert_[0], tex_vert_[1], //top left
    };

    if (!vbo_) {
        glGenBuffers(1, &vbo_);
        update_vbo_ = true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    if (update_vbo_) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);
    }

    struct Vert {
        float pos[3];
        float texCoord[2];
    };

    glVertexAttribPointer(a_loc_[0], 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0);
    glEnableVertexAttribArray(a_loc_[0]);
    glVertexAttribPointer(a_loc_[1], 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, texCoord));
    glEnableVertexAttribArray(a_loc_[1]);

    glUniform1i(sampler_, 0);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vbo_data) / sizeof(Vert));

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(a_loc_[0]);
    glDisableVertexAttribArray(a_loc_[1]);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}
