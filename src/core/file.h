#pragma once

class Path
{
public:
	Path();
	Path(const std::string&);
	Path(const Path& rhs){ assign(rhs); }
	~Path();

	Path& operator=(const Path& rhs)
	{
		if (this != &rhs){
			assign(rhs);
		}
		return *this;
	}

	const std::string& str() const { return _str; }
	const char* c_str() const { return _str.c_str(); }

private:

	void assign(const Path&);

	std::string _str;
};