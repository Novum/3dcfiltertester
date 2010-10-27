#ifndef GUI_EXCEPTION_HEADER_GUARD
#define GUI_EXCEPTION_HEADER_GUARD

#pragma once

#include <string>

namespace gui
{
	/* 
	* Simple exception class for use in the editor namespace
	*/
	class Exception
	{
	public:
		Exception(std::wstring what) : what(what) {}	
		operator std::wstring () { return what; }
		operator const wchar_t* () { return what.c_str(); }
	private:
		std::wstring what;
	};
	
	inline void Assert(bool what,std::wstring error) 
	{
		if(!what) throw Exception(error);
	}
}

#endif