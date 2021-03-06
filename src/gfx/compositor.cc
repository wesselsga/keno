#include "gfx.h"
#include "shader.h"

#include <clock.h>

#include "effect.h"

#ifdef _RASPI
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif


using namespace gfx;


const char* _vertBasic = R"GLSL(

uniform mat4 u_mvp;
uniform float u_alpha;
attribute vec2 a_tex;
attribute vec4 a_color;
attribute vec4 a_pos;
varying vec4 v_color;
varying vec2 v_tex;

void main()
{
	v_tex = a_tex;
	v_color = vec4(a_color.r, a_color.g, a_color.b, u_alpha);
	gl_Position = a_pos * u_mvp;
}

)GLSL";

const char* _fragBasic = R"GLSL(

precision mediump float;
varying vec2 v_tex;
varying vec4 v_color;
uniform sampler2D s_tex;

void main()
{
   //gl_FragColor = vec4(1.0,1.0,1.0,1.0);
	gl_FragColor = texture2D(s_tex, v_tex) * v_color;
}

)GLSL";


const char* _fragBicubic = R"GLSL(

precision mediump float;
varying vec2      v_tex;
varying vec4      v_color;
uniform sampler2D s_tex;
uniform vec2      e_x;
uniform vec2      e_y;
uniform vec2      size_source;

vec4 bicubic_2d(vec2, vec2);
vec4 cubic(float);

void main()
{
  vec2 inv = vec2(1.0 / size_source.x, 1.0 / size_source.y);

  gl_FragColor = bicubic_2d(v_tex * size_source, inv) * v_color;
}

vec4 bicubic_2d(vec2 st, vec2 scale)
{
    float fx = fract(st.x);
    float fy = fract(st.y);
    st.x -= fx;
    st.y -= fy;

    vec4 xcubic = cubic(fx);
    vec4 ycubic = cubic(fy);

    vec4 c = vec4(st.x - 0.5, 
					st.x + 1.5, 
					st.y - 0.5, 
					st.y + 1.5);
					
    vec4 s = vec4(xcubic.x + xcubic.y, 
					xcubic.z + xcubic.w, 
					ycubic.x + ycubic.y, 
					ycubic.z + ycubic.w);
    
	vec4 offset = c + vec4(xcubic.y, xcubic.w, ycubic.y, ycubic.w) / s;

    vec4 sample0 = texture2D(s_tex, vec2(offset.x, offset.z) * scale);
    vec4 sample1 = texture2D(s_tex, vec2(offset.y, offset.z) * scale);
    vec4 sample2 = texture2D(s_tex, vec2(offset.x, offset.w) * scale);
    vec4 sample3 = texture2D(s_tex, vec2(offset.y, offset.w) * scale);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
       mix(sample3, sample2, sx),
       mix(sample1, sample0, sx), sy);
}

vec4 cubic(float x)
{
   float x2 = x * x;
   float x3 = x2 * x;
   vec4 w;
      
   w.x = -x3 + (3.0 * x2) - (3.0 * x) + 1.0;
   w.y = (3.0 * x3) - (6.0 * x2) + 4.0;
   w.z = (-3.0 * x3) + (3.0 * x2) + (3.0 * x) + 1.0;
   w.w = x3;
   
   /*w.x = (x * (x *(-x + 3.0) - 3.0) + 1.0);
   w.y = (x * x * (3.0 * x - 6.0) + 4.0);
   w.z = (x * (x * (-3.0 * x + 3.0) + 3.0) + 1.0);
   w.w = x3;*/
   
   return w / 6.0;
}

)GLSL";


struct vertex
{
	GLfloat tex[2];
	GLfloat color[4];
	GLfloat pos[4];
};



Compositor::Compositor(std::shared_ptr<Clock> const& clock)
   : _clock(clock)
{
   // load our screen shader
	//std::cout << "mixer: loading shaders...";
   //std::ifstream fs("c:\\temp\\default.shdr");
	//std::ifstream fs("c:\\temp\\bilinear.shdr");
	//std::ifstream fs("c:\\temp\\bicubic.shdr");
	
	_program = Program::build(_vertBasic, _fragBasic);
   //_program = Program::build(_vertBasic, _fragBicubic);

	//glBindAttribLocation(_program->id(), 0, "a_tex");
	//glBindAttribLocation(_program->id(), 1, "a_color");
   //glBindAttribLocation(_program->id(), 2, "a_pos");

	// activate our shaders
	glUseProgram(_program->id());

   //glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

Compositor::~Compositor()
{
}

void Compositor::process()
{
   glDisable(GL_DEPTH_TEST);

   // get current viewport
	GLint vp[4];
   glGetIntegerv(GL_VIEWPORT, vp);

   mat4<GLfloat> model, proj;

	GLfloat width = vp[2]*1.0f;
	GLfloat height = vp[3]*1.0f;

	// orthogonal projection (2D screen coordinates, top-left origin)
	proj = mat4<GLfloat>::ortho(0, width, height, 0.0f, -1.0f, 1.0f);

   GLuint uloc;
   GLenum err;
   
	uloc = glGetUniformLocation(_program->id(), "s_tex");
   glUniform1i(uloc, 0);
   
	float tex_size[2] = { 1920.0f, 1080.0f };
   float e_x[2] = { 1.0f / 1920.0f, 0.0f };
   float e_y[2] = { 0.0f, 1.0f / 1080.0f };
   
   uloc = glGetUniformLocation(_program->id(), "e_x");
   glUniform2fv(uloc, 1, e_x);
   err = glGetError();
   LOG_IF(WARN, err!=GL_NO_ERROR) << "e_x";


   uloc = glGetUniformLocation(_program->id(), "e_y");
   glUniform2fv(uloc, 1, e_y);
   err = glGetError();
   LOG_IF(WARN, err!=GL_NO_ERROR) << "e_y";

   uloc = glGetUniformLocation(_program->id(), "size_source");
   glUniform2fv(uloc, 1, tex_size);
   err = glGetError();
   LOG_IF(WARN, err!=GL_NO_ERROR) << "size_source";

   GLuint uloc_mvp = glGetUniformLocation(_program->id(), "u_mvp");
   GLuint uloc_alpha = glGetUniformLocation(_program->id(), "u_alpha");

   float opacity;

   // do all effects -> remove any effects that are complete
   for (auto fx = _effects.begin(); fx != _effects.end();)
   {
      if ((*fx)->animate()) {
         fx++;
      }
      else
      {
         LOG(INFO) << "effect is done.";
         fx = _effects.erase(fx);
      }
   }
   
   for (auto const layer : _layers)
   {
      // skip hidden layers
      if (!layer->visible()){
         continue;
      }

      // center in viewport
		//model *= mat4<GLfloat>::translate((width-xextent)/2.0f, (height-yextent)/2.0f, 0.0f);

      glActiveTexture(GL_TEXTURE0);
	   glBindTexture(GL_TEXTURE_2D, layer->texture());
	   err = glGetError();
      LOG_IF(WARN, err!=GL_NO_ERROR) << "bind texture";

      glBindBuffer(GL_ARRAY_BUFFER, layer->model());
      err = glGetError();
      LOG_IF(WARN, err!=GL_NO_ERROR) << "bind model";

      mat4<GLfloat> mvp;      
      //mvp *= mat4<GLfloat>::scale(1.0f, 1.0f, 0.0f);
		mvp *= proj * model;

	   // vertex shader needs model-view-projection
	   glUniformMatrix4fv(uloc_mvp, 1, false, mvp.get());
     
      opacity = layer->opacity();
      glUniform1fv(uloc_alpha, 1, &opacity);

	   glEnableVertexAttribArray(0);
	   glEnableVertexAttribArray(1);
	   glEnableVertexAttribArray(2);
      err = glGetError();	   

	   // bind the tex coordinates to the shader (unused here)
	   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (const GLvoid*)(0));
      err = glGetError();
      LOG_IF(WARN, err!=GL_NO_ERROR) << "shader 0";

	   // bind the colors to the shader
	   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (const GLvoid*)(2 * sizeof(GLfloat)));
      err = glGetError();
      LOG_IF(WARN, err!=GL_NO_ERROR) << "shader 1";

	   // bind the position to the shader
	   glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (const GLvoid*)(6 * sizeof(GLfloat)));
      err = glGetError();
      LOG_IF(WARN, err!=GL_NO_ERROR) << "shader 2";

	   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      err = glGetError();
      LOG_IF(WARN, err!=GL_NO_ERROR) << "draw arrays";

      //LOG(VERBOSE) << "drawing array";

	   glBindBuffer(GL_ARRAY_BUFFER, 0);
      err = glGetError();

      glBindTexture(GL_TEXTURE_2D, 0);
   }

}

std::shared_ptr<Layer> Compositor::layer(size_t const n) const
{
   if (n < _layers.size()){
      return _layers[n];
   }
   return nullptr;
}

std::shared_ptr<Layer> Compositor::addLayer(const std::shared_ptr<Stream>& stream)
{
   if (!stream){
      return nullptr;
   }

   float z = -1.0f;

   if (_layers.size() > 0)
      z = 1.0f;

   auto layer = Layer::create("1", stream, z);

   _layers.push_back(layer);

   return layer;
}

//
// adds an effect targeting the given layer
//
std::shared_ptr<Effect> Compositor::addEffect(
      std::string const& name, 
      std::string const& duration, 
      size_t layer)
{
   uint32_t d = 400;
   if (duration == "slow"){
      d = 600;
   }
   else if (duration == "fast"){
      d = 200;
   }
   
   return addEffect(name, d, layer);
}

//
// adds an effect targeting the given layer
//
std::shared_ptr<Effect> Compositor::addEffect(
      std::string const& name, 
      uint32_t duration, 
      size_t layer)
{
   auto target = this->layer(layer);
   if (!target) {
      return nullptr;
   }

   auto fx = Effect::create(name, duration, _clock);
   if (fx)
   {
      fx->select(target);
      fx->reset();      

      _effects.push_back(fx);
   }

   return fx;
}