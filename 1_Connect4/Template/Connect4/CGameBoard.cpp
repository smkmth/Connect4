#include "stdafx.h"
#include "CGameBoard.h"
#include "stdafx.h"
#include "stdlib.h"
#include "time.h"
#include <Windows.h>
#include <iostream>
using namespace std;

//constructer
CGameBoard::CGameBoard(HANDLE* pConsoleRef)
{
	phwndConsole = pConsoleRef;
	g_iActivePlayer = k_eBoardSlot_Player1;

}

//getters and setters 
int CGameBoard::GetBoardHeight() { return sm_iBoardHeight; }
int CGameBoard::GetBoardWidth() { return sm_iBoardWidth; }
int CGameBoard::GetGameState() { return g_iGameState; }
int CGameBoard::GetActivePlayer() {return g_iActivePlayer; }
int CGameBoard::GetNumberOfTurns() { return m_iSpacesRemaining;}

void CGameBoard::SetNumberOfTurns(int val) { m_iSpacesRemaining = val;}
void CGameBoard::SetGameState(int val) {g_iGameState = val;}
void CGameBoard::SetActivePlayer(int val){ g_iActivePlayer = val;}



void CGameBoard::RenderBoard(void)
{
	// first, clear the win32 console
	// n.b. have to use a windows command for this. Stinky!
	system("cls");

	// consts used to render the board
	const char	k_chSeparatorVertical = '|';
	const char	k_chSeparatorHorizontal = '-';
	const char	k_chSeparatorJoin = '+';

	// it will look like this when rendered (the player's pieces go in the gaps)
	// 1 2 3 4 5 6 7
	//+-+-+-+-+-+-+-+
	//| | | | | | | |
	//+-+-+-+-+-+-+-+
	//| | | | | | | |
	//+-+-+-+-+-+-+-+
	//| | | | | | | |
	//+-+-+-+-+-+-+-+
	//| | | | | | | |
	//+-+-+-+-+-+-+-+
	//| | | | | | | |
	//+-+-+-+-+-+-+-+
	//| | | | | | | |
	//+-+-+-+-+-+-+-+
	// 1 2 3 4 5 6 7

	// render column numbers
	for (int iColumnNumber = 0; iColumnNumber < sm_iBoardWidth; ++iColumnNumber)
	{
		cout << " " << (iColumnNumber + 1);
	}
	cout << endl;

	// render the board row by row
	for (int iRow = 0; iRow < sm_iBoardHeight; ++iRow)
	{
		//////////////////////////////////////////////////////////////////////////
		// separator row - board frame colour
		SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[k_eBoardSlot_Frame]);

		cout << k_chSeparatorJoin;

		// we print 2 chars per column
		for (int iSeparator = 0; iSeparator < sm_iBoardWidth; ++iSeparator)
		{
			cout << k_chSeparatorHorizontal << k_chSeparatorJoin;
		}
		cout << endl;

		//////////////////////////////////////////////////////////////////////////
		// row with pieces
		cout << k_chSeparatorVertical;

		// again, 2 chars per column
		for (int iColumn = 0; iColumn < sm_iBoardWidth; ++iColumn)
		{
			// cache board element index
			char i8BoardElementIndex = g_aaiBoard[iColumn][iRow];

			// render board element in appropriate colour
			SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[i8BoardElementIndex]);
			cout << k_achBoardPieces[i8BoardElementIndex];

			// render frame in frame colour
			SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[k_eBoardSlot_Frame]);
			cout << k_chSeparatorVertical;
		}
		cout << endl;
	}

	// final row at bottom of board
	SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[k_eBoardSlot_Frame]);
	cout << k_chSeparatorJoin;
	// we print 2 chars per column
	for (int iSeparator = 0; iSeparator < sm_iBoardWidth; ++iSeparator)
	{
		cout << k_chSeparatorHorizontal << k_chSeparatorJoin;
	}
	cout << endl;

	// render column numbers again
	SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[k_eBoardSlot_Empty]);
	for (int iColumnNumber = 0; iColumnNumber < sm_iBoardWidth; ++iColumnNumber)
	{
		cout << " " << (iColumnNumber + 1);
	}
	cout << endl;
}


void CGameBoard::UpdateGame(int iArgColumn)
{
	// find the 'lowest' available position in iArgColumn
	// N.B. 0,0 is top left of board, so 'lowest' row is highest index
	int iLowestEmptyRow = 0;
	for (int iRowToCheck = 0; iRowToCheck < sm_iBoardHeight; ++iRowToCheck)
	{
		if (k_eBoardSlot_Empty == g_aaiBoard[iArgColumn][iRowToCheck])
		{
			iLowestEmptyRow = iRowToCheck;
		}
		else
		{
			break;
		}
	}

	// insert current player's piece there
	// N.B. g_iActivePlayer is always either k_iBoardSlot_Player1 or k_iBoardSlot_Player2
	g_aaiBoard[iArgColumn][iLowestEmptyRow] = g_iActivePlayer;

	// if this finds a winning row it sets g_iGameState and g_aaiWinningRow
	Update_CheckForWin();
}


void CGameBoard::Update_CheckForWin()
{
	// uber basic optimisations on the searches: 
	// * we only check for lines of the active player's pieces
	// * we check the 4 directions separately, and only check a subset of the board for each
	// * for vertical and diagonal we always search DOWN the board (gravity drop prevents need for upward)
	//
	// there are lots more, but I'll leave them to you to work out
	if (Update_CheckForRow_Vertical()
		|| Update_CheckForRow_Horizontal()
		|| Update_CheckForRow_LU2RD()
		|| Update_CheckForRow_LD2RU())
	{
		g_iGameState = g_iActivePlayer;
	}
}


bool CGameBoard::Update_CheckForRow_Vertical()
{
	// check for vertical line:
	// * start at [0][0]
	// * end at [ k_iBoardWidth ][ iMaxRowToCheck ]
	// * always check for possible end of line before the middle pieces
	int iMaxRowToCheck = (sm_iBoardHeight - (sm_iWinLineLength - 1));

	for (int iColumn = 0; iColumn < sm_iBoardWidth; ++iColumn)
	{
		for (int iRow = 0; iRow < iMaxRowToCheck; ++iRow)
		{
			// N.B. set the winning row as we go
			if (g_aaiBoard[iColumn][iRow] == g_iActivePlayer)
			{
				g_aaiWinningRow[0][0] = iColumn;
				g_aaiWinningRow[0][1] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 becuase the 0th was the one we've already found at the start of the line
				int iLinePosIndex = (sm_iWinLineLength - 1);

				for (; iLinePosIndex > 0; --iLinePosIndex)
				{
					int iCheckRow = iRow + iLinePosIndex;

					if (g_aaiBoard[iColumn][iCheckRow] == g_iActivePlayer)
					{
						g_aaiWinningRow[iLinePosIndex][0] = iColumn;
						g_aaiWinningRow[iLinePosIndex][1] = iCheckRow;
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if (0 == iLinePosIndex)
				{
					return true;
				}
			}
		}
	}

	// didn't find a vertical line
	return false;
}


bool CGameBoard::Update_CheckForRow_Horizontal()
{
	// check for horizontal line:
	// * start at [0][0]
	// * end at [ iMaxColumnToCheck ][ k_iBoardHeight ]
	// * always check for possible end of line before the middle pieces
	int iMaxColumnToCheck = (sm_iBoardWidth - (sm_iWinLineLength - 1));

	for (int iColumn = 0; iColumn < iMaxColumnToCheck; ++iColumn)
	{
		for (int iRow = 0; iRow < sm_iBoardHeight; ++iRow)
		{
			// N.B. set the winning row as we go
			if (g_aaiBoard[iColumn][iRow] == g_iActivePlayer)
			{
				g_aaiWinningRow[0][0] = iColumn;
				g_aaiWinningRow[0][1] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 becuase the 0th was the one we've already found at the start of the line
				int iLinePosIndex = (sm_iWinLineLength - 1);

				for (; iLinePosIndex > 0; --iLinePosIndex)
				{
					int iCheckColumn = iColumn + iLinePosIndex;

					if (g_aaiBoard[iCheckColumn][iRow] == g_iActivePlayer)
					{
						g_aaiWinningRow[iLinePosIndex][0] = iCheckColumn;
						g_aaiWinningRow[iLinePosIndex][1] = iRow;
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if (0 == iLinePosIndex)
				{
					return true;
				}
			}
		}
	}

	// didn't find a line
	return false;
}


bool CGameBoard::Update_CheckForRow_LU2RD()
{
	// check for left,up to right,down diagonal:
	// * start at [0][0]
	// * end at [ iMaxColumnToCheck ][ iMaxRowToCheck ]
	// * always check for possible end of line before the middle pieces
	int iMaxColumnToCheck = (sm_iBoardWidth - (sm_iWinLineLength - 1));
	int iMaxRowToCheck = (sm_iBoardHeight - (sm_iWinLineLength - 1));

	for (int iColumn = 0; iColumn < iMaxColumnToCheck; ++iColumn)
	{
		for (int iRow = 0; iRow < iMaxRowToCheck; ++iRow)
		{
			// N.B. set the winning row as we go
			if (g_aaiBoard[iColumn][iRow] == g_iActivePlayer)
			{
				g_aaiWinningRow[0][0] = iColumn;
				g_aaiWinningRow[0][1] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 becuase the 0th was the one we've already found at the start of the line
				int iLinePosIndex = (sm_iWinLineLength - 1);

				for (; iLinePosIndex > 0; --iLinePosIndex)
				{
					int iCheckColumn = iColumn + iLinePosIndex;
					int iCheckRow = iRow + iLinePosIndex;

					if (g_aaiBoard[iCheckColumn][iCheckRow] == g_iActivePlayer)
					{
						g_aaiWinningRow[iLinePosIndex][0] = iCheckColumn;
						g_aaiWinningRow[iLinePosIndex][1] = iCheckRow;
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if (0 == iLinePosIndex)
				{
					return true;
				}
			}
		}
	}

	// didn't find a line
	return false;
}


bool CGameBoard::Update_CheckForRow_LD2RU()
{
	// check for left,up to right,down diagonal:
	// * start at [ iMinColumnToCheck ][ iMinRowToCheck ]
	// * end at [ k_iBoardWidth ][ k_iBoardHeight ]
	// * always check for possible end of line before the middle pieces
	int iMaxColumnToCheck = (sm_iBoardWidth - (sm_iWinLineLength - 1));
	int iMinRowToCheck = (sm_iWinLineLength - 1);

	for (int iColumn = 0; iColumn < iMaxColumnToCheck; ++iColumn)
	{
		for (int iRow = iMinRowToCheck; iRow < sm_iBoardHeight; ++iRow)
		{
			// N.B. set the winning row as we go
			if (g_aaiBoard[iColumn][iRow] == g_iActivePlayer)
			{
				g_aaiWinningRow[0][0] = iColumn;
				g_aaiWinningRow[0][1] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 because the 0th was the one we've already found at the start of the line
				int iLinePosIndex = (sm_iWinLineLength - 1);

				for (; iLinePosIndex > 0; --iLinePosIndex)
				{
					int iCheckColumn = iColumn + iLinePosIndex;
					int iCheckRow = iRow - iLinePosIndex;

					if (g_aaiBoard[iCheckColumn][iCheckRow] == g_iActivePlayer)
					{
						g_aaiWinningRow[iLinePosIndex][0] = iCheckColumn;
						g_aaiWinningRow[iLinePosIndex][1] = iCheckRow;
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if (0 == iLinePosIndex)
				{
					return true;
				}
			}
		}
	}

	// didn't find a line
	return false;
}


bool CGameBoard::ConfirmQuitGame()
{
	system("cls");
	cout << "######################################\n"
		<< "### are you sure you want to quit? ###\n"
		<< "######################################\n"
		<< "'y' followed by return to confirm, any other input will cancel.\n";

	// get 1st input char and flush the input buffer to prevent unwanted immediate returns from cin.get() later
	cin.clear();
	char chInput = cin.get();
	cin.sync();

	return (('y' == chInput) || ('Y' == chInput)) ? true : false;
}


bool CGameBoard::DisplayWinMessageQueryPlayAgain()
{
	RenderBoard();

	cout << endl << "########################\n"
		<< "### Congratulations! ###\n"
		<< "########################\n\n";

	// change to winning player colour for winner description 
	// n.b. g_iActivePlayer == either k_iBoardSlot_Player1 or 
	SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[g_iActivePlayer]);
	cout << "'" << k_achBoardPieces[g_iGameState] << "' player has won!\n\n";
	SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[k_eBoardSlot_Empty]);

	cout << "Winning line from ["
		<< (g_aaiWinningRow[0][0] + 1) << "," << (g_aaiWinningRow[0][1] + 1) << "] to ["
		<< (g_aaiWinningRow[sm_iWinLineLength - 1][0] + 1) << "," << (g_aaiWinningRow[sm_iWinLineLength - 1][1] + 1) << "]\n"
		<< "Press 'y' followed by return to play again, any other input to quit\n";

	// get 1st input char and flush the input buffer to prevent unwanted immediate returns from cin.get() later
	char chInput = cin.get();
	cin.sync();

	return (('y' == chInput) || ('Y' == chInput)) ? false : true;
}


bool CGameBoard::DisplayDrawMessageQueryPlayAgain()
{
	RenderBoard();

	cout << endl << "########################\n"
		<< "### Game is a draw!! ###\n"
		<< "########################\n\n";

	// change to winning player colour for winner description 
	// n.b. g_iActivePlayer == either k_iBoardSlot_Player1 or 
	SetConsoleTextAttribute(phwndConsole, k_ai8BoardElementColours[k_eBoardSlot_Empty]);

	cout << "Press 'y' followed by return to play again, any other input to quit\n";

	// get 1st input char and flush the input buffer to prevent unwanted immediate returns from cin.get() later
	char chInput = cin.get();
	cin.sync();

	return (('y' == chInput) || ('Y' == chInput)) ? false : true;
}


void CGameBoard::Reset()
{
	// reset board
	memset(g_aaiBoard, k_eBoardSlot_Empty, sizeof(g_aaiBoard));
	// randomise 1st player
	// N.B. ( rand() & 1) will give 0 or 1, so add 1 to get 1 or 2
	g_iActivePlayer = (rand() & 1) + 1;

	// neither player has won yet
	g_iGameState = k_iState_Playing;

	// reset winning position array
	memset(g_aaiWinningRow, 0, sizeof(g_aaiWinningRow));
}







