#include "gfx.h"
#include "shader.h"

#ifdef _RASPI
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

using namespace gfx;

Shader::Shader(const _priv&, const GLuint id) : _id(id)
{
}

Shader::~Shader()
{
	glDeleteShader(_id);
}

std::shared_ptr<Shader> Shader::loadVertex(const std::string& code)
{
	GLuint shader = load(code.c_str(), GL_VERTEX_SHADER);
   if (shader){
		return std::make_shared<Shader>(_priv{}, shader);
	}
	return nullptr;
}

std::shared_ptr<Shader> Shader::loadFragment(const std::string& code)
{
	GLuint shader = load(code.c_str(), GL_FRAGMENT_SHADER);
   if (shader){
		return std::make_shared<Shader>(_priv{}, shader);
	}
	return nullptr;
}

GLuint Shader::load(const char* code, GLenum type)
{
	GLint compiled;
   
   // create the shader object
   GLuint shader = glCreateShader(type);
   if (!shader){
      return 0;
	}

   // Load the shader source
   glShaderSource(shader, 1, &code, NULL);
   
   // Compile the shader
   glCompileShader(shader);

   // Check the compile status
   glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) 
   {
      GLint msglen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &msglen);
      
      if (msglen > 1)
      {
			char* msg = (char*)malloc(sizeof(char) * msglen);
         glGetShaderInfoLog(shader, msglen, NULL, msg);
			LOG(ERR) << "gfx: error compiling shader: " << std::endl << msg;
         free(msg);
      }

      glDeleteShader(shader);
      return 0;
   }

   return shader;
}


Program::Program(const _priv&, const GLuint id) : _id(id)
{
}

Program::~Program()
{
	glDeleteProgram(_id);
}

std::shared_ptr<Program> Program::build(std::istream& in)
{   
   std::string line;
   std::string sep("========");

   std::string vs;
   while (std::getline(in, line))
   {
      if (!line.compare(0, sep.size(), sep)){
         break;
      }

      vs.append(line);
      vs.append("\n");
   }

   std::string fs;
   while (std::getline(in, line))
   {
      fs.append(line);
      fs.append("\n");        
   }

   return build(vs, fs);
}

std::shared_ptr<Program> Program::build(
               const std::string& vs,
					const std::string& fs)
{
   auto vshader = Shader::loadVertex(vs);
   if (!vshader){
      return nullptr;
   }

   auto fshader = Shader::loadFragment(fs);
   if (!fshader){
      return nullptr;
   }

   return build(vshader, fshader);
}

std::shared_ptr<Program> Program::build(
					const std::shared_ptr<Shader>& vertex,
					const std::shared_ptr<Shader>& frag)
{
	if (!vertex || !frag){
		return nullptr;
	}

	// create the program object
   GLuint program = glCreateProgram();
   if (!program){
      return nullptr;
	}

   glAttachShader(program, vertex->id());
   glAttachShader(program, frag->id());

   glBindAttribLocation(program, 0, "a_tex");
	glBindAttribLocation(program, 1, "a_color");
   glBindAttribLocation(program, 2, "a_pos");
	
	// Link the program
   glLinkProgram(program);
   
	// Check the link status
	GLint  linked;
   glGetProgramiv(program, GL_LINK_STATUS, &linked);
   
	if (!linked) 
   {
      GLint msglen = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msglen);      
      if (msglen > 1)
      {
			char* msg = (char*)malloc(sizeof(char) * msglen);
         glGetProgramInfoLog(program, msglen, NULL, msg);
			LOG(ERR) << "gfx: error linking program: " << std::endl << msg;
         free(msg);
      }
      
		glDeleteProgram(program);
      return nullptr;
   }
   
	return std::make_shared<Program>(_priv{}, program);
}