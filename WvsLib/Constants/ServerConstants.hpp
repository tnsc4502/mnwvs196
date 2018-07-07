#include <string>

namespace ServerConstants
{
	enum ServerType
	{
		SVR_LOGIN,
		SVR_CENTER,
		SVR_GAME,
		SVR_SHOP,
		SVR_CLAIM
	};

	enum class GameLocale { TW = 6 };

	constexpr int kMaxChannelCount = 30;
	constexpr int kGameVersion = 196;
	constexpr int kGameLocale = (int)GameLocale::TW;

	const std::string strGameSubVersion = "3";
}