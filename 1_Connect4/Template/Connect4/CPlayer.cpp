#include "stdafx.h"
#include "CPlayer.h"
#include <iostream>

using namespace std;




CPlayer::CPlayer(CGameBoard* cboard)
{
	// special values returned by (and passed to) fn GetPlayerInput()
	// n.b. all valid rows are in range 0 to ( k_iBoardWidth - 1 )
	int iPlayerInput_Quit = cBoard->GetBoardWidth();


	// n.b. -ve values from -1 to -k_iBoardWidth are used to return input if the chosen column is full
	int iPlayerInput_None = -(cBoard->GetBoardWidth() + 1);
	int iPlayerInput_Invalid = -(cBoard->GetBoardWidth + 3);

	cboard = cBoard;

}

CPlayer::~CPlayer()
{
}


int	CPlayer::GetPlayerInput(int iArgLastInput)
{
	int iReturnValue = iPlayerInput_Invalid;

	SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[g_iActivePlayer]);
	cout << "## '" << k_achBoardPieces[g_iActivePlayer] << "' player's turn ##";
	SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[k_iBoardSlot_Empty]);

	// all valid inputs are +ve 
	if ((k_iPlayerInput_None != iArgLastInput)
		&& (0 > iArgLastInput))
	{
		if (k_iPlayerInput_Invalid == iArgLastInput)
		{
			cout << "\n### Your last input was invalid! Please try again. ###";
		}
		// otherwise it was a full column
		else
		{
			cout << "\n### Column " << abs(iArgLastInput) << " is full. Please choose another. ###";
		}
	}

	cout << "\nPlease input a column number (1 to " << cBoard->GetBoardWidth() << "), or 'q' to quit, then press return:\n";

	// get first character from stdin and throw the rest away.
	// this is necessary otherwise the input requires one cin.getch() 
	// for each character on the line and another for <enter> on the end
	char chInput = cin.get();
	cin.sync();

	// validate input and break from loop when valid
	if (('q' == chInput) || ('Q' == chInput))
	{
		// set input value and break from loop
		iReturnValue = k_iPlayerInput_Quit;
	}
	else
	{
		// numerals in ascii encoding start at 0 and go up to 9 in sequential order.
		// so, to get the column index we subtract '1' from chInput and check the value's in the range 
		// 0 < value < k_iBoardWidth 
		int iInputColumn = chInput - '1';
		if ((0 <= iInputColumn) && (k_iBoardWidth > iInputColumn))
		{
			// check this column isn't full (0,0 is top left)
			if (k_iBoardSlot_Empty == g_aaiBoard[iInputColumn][0])
			{
				// set input value and break from loop
				iReturnValue = iInputColumn;
			}
			else
			{
				// full column - return value in range -1 to -k_iBoardWidth
				iReturnValue = -(iInputColumn + 1);
			}
		}

	}// if( quit ){ ... }else{...

	return iReturnValue;
}

