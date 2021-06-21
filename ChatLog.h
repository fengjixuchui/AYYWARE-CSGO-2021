#pragma once

class CBaseHudChat;

namespace Interfaces {

	extern CBaseHudChat* gpChat;

}

template <typename ...T>
void ChatLog(char* fmt, T... args);

template <typename ...T>
void ChatLog(char* fmt, T... args)
{
	char buffer[256] = { 0 };
	//brown color
	memcpy(buffer, u8" \x10", 3);

	sprintf(buffer, fmt, args...);

	if (Interfaces::gpChat)
		Interfaces::gpChat->ChatPrintf(0, 0, buffer);
}
