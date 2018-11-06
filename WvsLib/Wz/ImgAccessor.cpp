#include "ImgAccessor.h"

using namespace WZ;

ImgAccessor::ImgAccessor(const std::string& sImgName)
{
	MapFile file;
	file.Open(sImgName + ".img");
	uint32_t fsize = file.ReadCInt();
	pNode = AllocObj( Node );
	pNode->InitTop(sImgName);
	pNode->Reserve(1);

	//they overloaded operator new, our AllocObjCtor can't support it LOL
	pImg = new Img(file, *pNode, fsize, 0);
}

ImgAccessor::~ImgAccessor()
{
	//Don't delete Img, it would cause unknown errors, WTF
	//delete (pImg);

	//I think releasing Node is enough
	FreeObj(pNode);
	//
	stWzResMan->ReleaseMemory();
}

WZ::Node WZ::ImgAccessor::begin()
{
	return pNode->begin();
}

WZ::Node WZ::ImgAccessor::end()
{
	return pNode->end();
}

WZ::Node & WZ::ImgAccessor::operator++()
{
	return pNode->operator++();
}

WZ::Node & ImgAccessor::operator[](const std::string & sName)
{
	return pNode->operator[](sName.c_str());
}
