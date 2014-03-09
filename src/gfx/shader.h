#pragma once

namespace gfx {

class Shader
{
	struct _priv { };
public:
	explicit Shader(const _priv&, const uint32_t);
	virtual ~Shader();

	static std::shared_ptr<Shader> loadVertex(const std::string&);
	static std::shared_ptr<Shader> loadFragment(const std::string&);

	uint32_t id() const { return _id; }

private:
	Shader()=delete;
	Shader(const Shader&)=delete;
	const Shader& operator=(const Shader&) = delete;

	static uint32_t load(const char*, uint32_t);
	
	const uint32_t _id;
};


class Program
{
	struct _priv { };
public:
	explicit Program(const _priv&, const uint32_t);
	virtual ~Program();
   
   static std::shared_ptr<Program> build(std::istream&);

   static std::shared_ptr<Program> build(
					const std::string&,
					const std::string&);

	static std::shared_ptr<Program> build(
					const std::shared_ptr<Shader>&,
					const std::shared_ptr<Shader>&);

	uint32_t id() const { return _id; }

private:
	Program()=delete;
	Program(const Program&)=delete;
	const Program& operator=(const Program&) = delete;

	const uint32_t _id;
};


};
