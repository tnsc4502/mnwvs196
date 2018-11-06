#pragma once
#include "WzResMan.hpp"
#include <string>

namespace WZ 
{
	class ImgAccessor
	{
		Node *pNode;
		Img *pImg;
	public:

		ImgAccessor(const std::string& sImgName);
		~ImgAccessor();

		Node begin();
		Node end();
		Node& operator++();
		Node& operator[](const std::string& sName);
	};
}
