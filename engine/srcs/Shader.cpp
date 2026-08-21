#include <Shader.hpp>

std::string Shader::readShaderFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open shader file " + path);
    std::stringstream buff;
    buff << file.rdbuf();
    return buff.str();
};

GLuint Shader::compileShader(GLenum type, const std::string& path)
{
    std::string code = readShaderFile(path);
    const char* src = code.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Vérifier compilation
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error in shader compilation: " << infoLog << std::endl;
    }
    return shader;
}

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    this->id = glCreateProgram();
    glAttachShader(this->id, vertexShader);
    glAttachShader(this->id, fragmentShader);
    glLinkProgram(this->id);

    GLint success;
    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(this->id, 512, nullptr, infoLog);
        std::cerr << "Error in the linking program: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::bind() const {
    glUseProgram(this->id);
}

void Shader::unbind() const {
    glUseProgram(0);
}

GLint Shader::cachedLocation(const std::string& name) const {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end())
        return it->second;
    GLint loc = glGetUniformLocation(this->id, name.c_str());
    uniformCache.emplace(name, loc);
    return loc;
}

void Shader::setMatrix4_false(const std::string& name, const float* value) const {
    glUniformMatrix4fv(cachedLocation(name), 1, GL_FALSE, value);
};

void Shader::setMatrix4_true(const std::string& name, const float* value) const {
    glUniformMatrix4fv(cachedLocation(name), 1, GL_TRUE, value);
};

void Shader::setVec2(const std::string& name, const float x, const float y) const {
      glUniform2f(cachedLocation(name), x, y);
};


void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const {
    glUniform3f(cachedLocation(name), x, y ,z);
};

void Shader::setVec4(const std::string& name, const float x, const float y, const float z, const float w) const {
    glUniform4f(cachedLocation(name), x, y ,z, w);
};

void Shader::setInt(const std::string& name, const int value) const {
    glUniform1i(cachedLocation(name), value);
};

void Shader::setFloat(const std::string& name, const float value) const {
    glUniform1f(cachedLocation(name), value);
};

GLint Shader::getUniformLocation(const std::string& name) const {
    return cachedLocation(name);
};


