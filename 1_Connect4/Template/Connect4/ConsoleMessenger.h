#pragma once
class ConsoleMessenger
{
private:

	//pointer to the handle thingy
	HANDLE* phwndConsole;
	CGameBoard* currentGameBoard;

public:
	ConsoleMessenger(HANDLE* pConsoleRef, CGameBoard* cGameBoard);
	~ConsoleMessenger();
	void WriteMessage();
};

