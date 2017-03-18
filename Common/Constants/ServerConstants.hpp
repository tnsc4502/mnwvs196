#include <string>

namespace ServerConstants
{
	enum class GameLocale { TW = 6 };

	constexpr int kGameVersion = 196;
	constexpr int kGameLocale = (int)GameLocale::TW;

	const std::string strGameSubVersion = "3";
	const std::string kGameFilePath = "D:/Program Files (x86)/Gamania/MapleStory196/";
}