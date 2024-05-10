#ifndef __CSJGLPROGRAM_H__
#define __CSJGLPROGRAM_H__

#include "CSJGLFrameworkDefine.h"

#include <memory>
#include <string>

#include "gl/glew.h"

/**
 * This class represenets a OpenGL program including shaders.
 * And provides functions to create, compile and link shaders.
 *
 * Users can custom program by different shader code.
 */
class CSJGLFRAMEWORK_API CSJGLProgram {
public:
    CSJGLProgram();
    ~CSJGLProgram();

    /**
     * @brief Create a program with shader code.
     * 
     * @param[in] vertexShaderString    the vertex shader code.
     * @param[in] fragmentShaderString  the fragment shader code.
     * @param[in] createByFile          indicates vertexShader and fragmentShader are filepath or code.
     */
    CSJGLProgram(std::string& vertexShader, std::string& fragmentShader, bool createByFile = false);

    bool programUseable() const {
        return m_bProgramUsable;
    }
    void useProgram();

protected:
    bool createVertexShader(const std::string& shaderString);

    bool createFragmentShader(const std::string& shaderString);

private:
    GLuint m_vRenderingProgram;
    GLuint m_vVertexShader;
    GLuint m_vFragmentShader;

    bool   m_bProgramUsable = false;     // indicates current program is created ok or not.
};

using CSJSpProgram = std::shared_ptr<CSJGLProgram>;

#endif // __CSJGLPROGRAM_H__