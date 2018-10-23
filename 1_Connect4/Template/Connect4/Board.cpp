#include "stdafx.h"

#include "stdlib.h"
#include "time.h"
#include <Windows.h>
#include <iostream>

using namespace std;

class CameBoard
{
private:
	int m_iBoardWidth	=7;
	int m_iBoardHight	=6;


	// 2d array to store the board
	int m_aaiBoard[m_iBoardWidth][m_iBoardHeight];


public:

	void RenderBoard(void)
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
		for (int iColumnNumber = 0; iColumnNumber < k_iBoardWidth; ++iColumnNumber)
		{
			cout << " " << (iColumnNumber + 1);
		}
		cout << endl;

		// render the board row by row
		for (int iRow = 0; iRow < k_iBoardHeight; ++iRow)
		{
			//////////////////////////////////////////////////////////////////////////
			// separator row - board frame colour
			SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[k_iBoardSlot_Frame]);

			cout << k_chSeparatorJoin;

			// we print 2 chars per column
			for (int iSeparator = 0; iSeparator < k_iBoardWidth; ++iSeparator)
			{
				cout << k_chSeparatorHorizontal << k_chSeparatorJoin;
			}
			cout << endl;

			//////////////////////////////////////////////////////////////////////////
			// row with pieces
			cout << k_chSeparatorVertical;

			// again, 2 chars per column
			for (int iColumn = 0; iColumn < k_iBoardWidth; ++iColumn)
			{
				// cache board element index
				char i8BoardElementIndex = g_aaiBoard[iColumn][iRow];

				// render board element in appropriate colour
				SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[i8BoardElementIndex]);
				cout << k_achBoardPieces[i8BoardElementIndex];

				// render frame in frame colour
				SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[k_iBoardSlot_Frame]);
				cout << k_chSeparatorVertical;
			}
			cout << endl;
		}

		// final row at bottom of board
		SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[k_iBoardSlot_Frame]);
		cout << k_chSeparatorJoin;
		// we print 2 chars per column
		for (int iSeparator = 0; iSeparator < k_iBoardWidth; ++iSeparator)
		{
			cout << k_chSeparatorHorizontal << k_chSeparatorJoin;
		}
		cout << endl;

		// render column numbers again
		SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[k_iBoardSlot_Empty]);
		for (int iColumnNumber = 0; iColumnNumber < k_iBoardWidth; ++iColumnNumber)
		{
			cout << " " << (iColumnNumber + 1);
		}
		cout << endl;
	}




};