#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class ShaderManager
{
public:
    // Program ID
    unsigned int ID;
    ShaderManager(const char* vertexPath, const char* fragmentPath);
    ~ShaderManager();
    // Activate Shader program
    void use();
    // uniform tool function
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
};

#endif
