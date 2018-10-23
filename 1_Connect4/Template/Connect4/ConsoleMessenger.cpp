#include "stdafx.h"
#include "CGameBoard.h"
#include "ConsoleMessenger.h"
#include <iostream>;
#include <Windows.h>





ConsoleMessenger::ConsoleMessenger(HANDLE* pConsoleRef, CGameBoard* cGameBoard)
{
	phwndConsole = pConsoleRef;
	currentGameBoard = GameBoard;

}


ConsoleMessenger::~ConsoleMessenger()
{
}

void ConsoleMessenger::WriteMessage() {

	SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[currentGameBoard->GetActivePlayer()]);
	std::cout << "## '" << k_achBoardPieces[currentGameBoard->GetActivePlayer()] << "' player's turn ##";
	SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[k_iBoardSlot_Empty]);
}
