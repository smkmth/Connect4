//////////////////////////////////////////////////////////////////////////
// (C) Gamer Camp 2012
//
// Horribly basic win32 command line version of 2 player "connect 4"
// Original version thrown together by Alex Darby
//
// This is deliberately written in a very 'C' style manner.
//
// Please convert it to nice object oriented C++ code.
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "stdlib.h"
#include "time.h"
#include <Windows.h>
#include <iostream>


//////////////////////////////////////////////////////////////////////////
// using ...
using namespace std;


//////////////////////////////////////////////////////////////////////////
// data needed to track the game

// constants

	// size of the board
	const int k_iBoardWidth			= 7;	// X
	const int k_iBoardHeight		= 6;	// Y

	// board state management
	const int k_iBoardSlot_Empty	= 0;
	const int k_iBoardSlot_Player1	= 1;
	const int k_iBoardSlot_Player2	= 2;
	const int k_iBoardSlot_Frame	= 3;

	// game win state 
	const int k_iState_Playing		= 0;
	const int k_iState_Won_Player1	= k_iBoardSlot_Player1;
	const int k_iState_Won_Player2	= k_iBoardSlot_Player2;
	const int k_iState_Draw			= k_iState_Won_Player2 + 1;


	// length of a winning row
	const int k_iWinLineLength		= 4;

	// characters used to represent pieces on the board, this array is indexed by using k_iBoardSlot_xxx
	const char k_achBoardPieces[] = 
	{ 
		' ', // no piece - g_achBoardPieces[ k_iBoardSlot_Empty ]
		'@', // player 1 - g_achBoardPieces[ k_iBoardSlot_Player1 ]
		'O', // player 2 - g_achBoardPieces[ k_iBoardSlot_Player2 ]
		'!', // not used, but provided since k_iBoardSlot_Frame exists
	};


	// handle to console to allow us to change colour of console rendering
	HANDLE g_hwndConsole = NULL;

	// console window output colour settings
	const char k_ai8BoardElementColours[] =
	{
		7,	// white on black
		12,	// red on black
		14,	// yellow on black
		9,	// blue on black
	};

	// special values returned by (and passed to) fn GetPlayerInput()
		// n.b. all valid rows are in range 0 to ( k_iBoardWidth - 1 )
		const int k_iPlayerInput_Quit		= k_iBoardWidth;
	
		// n.b. -ve values from -1 to -k_iBoardWidth are used to return input if the chosen column is full
		const int k_iPlayerInput_None		= -( k_iBoardWidth + 1 );
		const int k_iPlayerInput_Invalid	= -( k_iBoardWidth + 3 );
	

// state data

	// 2d array to store the board
	// n.b.		0,0 is in the top left corner when the board is rendered
	// n.n.b. 	columns (X axis of the board) from 0 @ left to 7 @ right &
	//			rows (Yaxis of the board) from 0 @ top to 7 @ bottom
	int g_aaiBoard[ k_iBoardWidth ][ k_iBoardHeight ];

	// keep track of which player's turn it is 
	// n.b. uses k_iBoardSlot_xxx values
	int g_iActivePlayer = k_iBoardSlot_Player1;

	// set to player number when a player has won
	// n.b. uses k_iBoardSlot_xxx values
	int g_iGameState = k_iState_Playing;

	// number of turns
	int g_iSpacesRemaining = k_iBoardWidth * k_iBoardHeight;

	// holds the positions of the winning row so it can be displayed
	// g_aaiWinningRow[ i ][ 0 ] for column (X)
	// g_aaiWinningRow[ i ][ 1 ] for row (Y)
	int g_aaiWinningRow[ k_iWinLineLength ][ 2 ]; 
	


//////////////////////////////////////////////////////////////////////////
// function 'declarations' also known as 'prototypes'
// as we know, these must appear before they are first called
void	Reset							( void );	
int		GetPlayerInput					( int iArgLastInput );
void	UpdateGame						( int iArgColumn );
void	RenderBoard						( void );

bool	ConfirmQuitGame					( void );
bool	DisplayWinMessageQueryPlayAgain ( void );
bool	DisplayDrawMessageQueryPlayAgain( void );

void	Update_CheckForWin				( void );
bool	Update_CheckForRow_Vertical		( void );
bool	Update_CheckForRow_Horizontal	( void );
bool	Update_CheckForRow_LU2RD		( void );
bool	Update_CheckForRow_LD2RU		( void );



//////////////////////////////////////////////////////////////////////////
// main()
//
// In this shockingly simple C style program this contains all the game 
// flow logic.
//
//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	bool bQuitGame = false;

	// seed the random number generation used by Reset()
	srand( (unsigned)time( NULL ) );

	// initialise g_hwndConsole so we can set colours
	g_hwndConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	while( !bQuitGame )
	{
		Reset();

		// while( neither player has won yet )
		while( k_iState_Playing == g_iGameState )
		{			
			// swap players			
			g_iActivePlayer		= ( k_iBoardSlot_Player1 == g_iActivePlayer ) ? k_iBoardSlot_Player2 : k_iBoardSlot_Player1;
			
			int iPlayerInput	= k_iPlayerInput_None;

			// loop rendering the screen and asking for input until we get valid input
			// N.B. k_iPlayerInput_None < 0 && all valid inputs are >= 0
			while( iPlayerInput < 0 )
			{
				RenderBoard();		
				// we pass the last input so this can print an appropriate message if it was invalid
				iPlayerInput = GetPlayerInput( iPlayerInput );
			}

			// if the player wants to quit make sure they do
			if( k_iPlayerInput_Quit == iPlayerInput )
			{				
				bQuitGame = ConfirmQuitGame();		
				
				// they definitely want to quit
				// break out of the loop and let the game exit
				if( bQuitGame )
				{
					break;
				}
				// changed mind start next loop without doing any game logic 
				// N.B. mustn't update g_iActivePlayer
				else
				{
					continue;
				}
			}

			// this will set g_iGameState if the active player has won 
			UpdateGame(	iPlayerInput );
		
			// check for a draw
			if( --g_iSpacesRemaining == 0 )
			{
				g_iGameState = k_iState_Draw;
			}
		}


		// game state is no longer k_iState_Playing - is it a draw?
		if( k_iState_Draw == g_iGameState )
		{
			// draw! play again?
			if( DisplayDrawMessageQueryPlayAgain() )
			{
				// no thanks?
				bQuitGame = ConfirmQuitGame();
			}
		}
		else
		{
			// congratulations! play again?
			if( DisplayWinMessageQueryPlayAgain() )
			{
				// no thanks?
				bQuitGame = ConfirmQuitGame();
			}
		}

		// reset state in any state
		g_iSpacesRemaining	= ( k_iBoardWidth * k_iBoardHeight );
		g_iGameState		= k_iState_Playing;

	}//while( !bQuitGame ){...

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//
// this function resets the game to its initial state
// n.b. it randomises which player goes first. rand() is a standard C 
// library function and its initial value is seeded at the top of main() 
// using srand()
//
//////////////////////////////////////////////////////////////////////////
void Reset( void )
{
	// reset board
	memset( g_aaiBoard, k_iBoardSlot_Empty, sizeof( g_aaiBoard) );

	// randomise 1st player
	// N.B. ( rand() & 1) will give 0 or 1, so add 1 to get 1 or 2
	g_iActivePlayer = ( rand() & 1 ) + 1;

	// neither player has won yet
	g_iGameState = k_iState_Playing;

	// reset winning position array
	memset( g_aaiWinningRow, 0, sizeof( g_aaiWinningRow ) );
}


//////////////////////////////////////////////////////////////////////////
//
// Prompts the player for input at the command line and validates it.
// 
// possible return values:
// * integer in range 0 to ( k_iBoardWidth - 1 )
// * -ve integer in range -1 to -k_iBoardWidth corresponding to a full column
// * if invalid input was received: k_iPlayerInput_Invalid	
// * if player wants to quit: k_iPlayerInput_Quit
//
// iArgLastInput should be the previous input read by this function *ON THIS GAME TURN*:
// * on first call of the turn pass k_iPlayerInput_None
// * if this function has already returned a value < 0 this turn, pass
//   that in - will change the input prompt to the player appropriately.
//
//////////////////////////////////////////////////////////////////////////
int	GetPlayerInput( int iArgLastInput )
{
	int iReturnValue = k_iPlayerInput_Invalid;

	SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[ g_iActivePlayer ]);
	cout << "## '" << k_achBoardPieces[ g_iActivePlayer ] << "' player's turn ##";
	SetConsoleTextAttribute(g_hwndConsole, k_ai8BoardElementColours[ k_iBoardSlot_Empty ]);

	// all valid inputs are +ve 
	if(		( k_iPlayerInput_None != iArgLastInput )
		&&	( 0 > iArgLastInput ) )
	{
		if( k_iPlayerInput_Invalid == iArgLastInput )
		{
			cout << "\n### Your last input was invalid! Please try again. ###";
		}	
		// otherwise it was a full column
		else
		{
			cout << "\n### Column " << abs( iArgLastInput ) << " is full. Please choose another. ###";
		}
	}

	cout << "\nPlease input a column number (1 to " << k_iBoardWidth << "), or 'q' to quit, then press return:\n";
		
	// get first character from stdin and throw the rest away.
	// this is necessary otherwise the input requires one cin.getch() 
	// for each character on the line and another for <enter> on the end
	char chInput = cin.get();
	cin.sync();

	// validate input and break from loop when valid
	if( ('q' == chInput ) || ('Q' == chInput ))
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
		if( ( 0 <= iInputColumn ) && ( k_iBoardWidth > iInputColumn ) )
		{
			// check this column isn't full (0,0 is top left)
			if( k_iBoardSlot_Empty == g_aaiBoard[ iInputColumn ][ 0 ] )
			{
	 			// set input value and break from loop
				iReturnValue = iInputColumn;
			}
			else
			{
				// full column - return value in range -1 to -k_iBoardWidth
				iReturnValue = -( iInputColumn + 1 );
			}
		}

	}// if( quit ){ ... }else{...

	return iReturnValue;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void UpdateGame( int iArgColumn )
{
	// find the 'lowest' available position in iArgColumn
	// N.B. 0,0 is top left of board, so 'lowest' row is highest index
	int iLowestEmptyRow = 0;
	for( int iRowToCheck = 0; iRowToCheck < k_iBoardHeight; ++iRowToCheck )
	{
		if( k_iBoardSlot_Empty == g_aaiBoard[ iArgColumn ][ iRowToCheck ] )
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
	g_aaiBoard[ iArgColumn ][ iLowestEmptyRow ] = g_iActivePlayer;
	
	// if this finds a winning row it sets g_iGameState and g_aaiWinningRow
	Update_CheckForWin();
}


//////////////////////////////////////////////////////////////////////////
//
// This function inflates the terrapin. Not really. See if you can guess!
//
//////////////////////////////////////////////////////////////////////////
void RenderBoard( void )
{
	// first, clear the win32 console
	// n.b. have to use a windows command for this. Stinky!
	system( "cls" );

	// consts used to render the board
	const char	k_chSeparatorVertical	= '|';
	const char	k_chSeparatorHorizontal	= '-';
	const char	k_chSeparatorJoin		= '+';

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
	for( int iColumnNumber = 0; iColumnNumber < k_iBoardWidth; ++iColumnNumber )
	{
		cout << " " << (iColumnNumber + 1);
	}
	cout << endl;

	// render the board row by row
	for( int iRow = 0; iRow < k_iBoardHeight; ++iRow )
	{
		//////////////////////////////////////////////////////////////////////////
		// separator row - board frame colour
		SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ k_iBoardSlot_Frame ] );
	
		cout << k_chSeparatorJoin;
		
		// we print 2 chars per column
		for( int iSeparator = 0; iSeparator < k_iBoardWidth; ++iSeparator )
		{
			cout << k_chSeparatorHorizontal << k_chSeparatorJoin;
		}
		cout << endl;

		//////////////////////////////////////////////////////////////////////////
		// row with pieces
		cout << k_chSeparatorVertical;
		
		// again, 2 chars per column
		for( int iColumn = 0; iColumn < k_iBoardWidth; ++iColumn )
		{
			// cache board element index
			char i8BoardElementIndex = g_aaiBoard[ iColumn ][ iRow ];

			// render board element in appropriate colour
			SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ i8BoardElementIndex ] );
			cout << k_achBoardPieces[ i8BoardElementIndex ]; 

			// render frame in frame colour
			SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ k_iBoardSlot_Frame ] );
			cout << k_chSeparatorVertical;
		}
		cout << endl;
	}

	// final row at bottom of board
	SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ k_iBoardSlot_Frame ] );
	cout << k_chSeparatorJoin;
	// we print 2 chars per column
	for( int iSeparator = 0; iSeparator < k_iBoardWidth; ++iSeparator )
	{
		cout << k_chSeparatorHorizontal << k_chSeparatorJoin;
	}
	cout << endl;

	// render column numbers again
	SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ k_iBoardSlot_Empty ] );
	for( int iColumnNumber = 0; iColumnNumber < k_iBoardWidth; ++iColumnNumber )
	{
		cout << " " << ( iColumnNumber + 1 );
	}
	cout << endl;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool ConfirmQuitGame( void )
{
	system( "cls" );
	cout	<< "######################################\n"
			<< "### are you sure you want to quit? ###\n"
			<< "######################################\n"
			<< "'y' followed by return to confirm, any other input will cancel.\n";
	
	// get 1st input char and flush the input buffer to prevent unwanted immediate returns from cin.get() later
	cin.clear();
	char chInput = cin.get();
	cin.sync();

	return (( 'y' == chInput ) || ( 'Y' == chInput )) ? true : false;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool DisplayWinMessageQueryPlayAgain( void )
{
	RenderBoard();

	cout << endl	<< "########################\n"
					<< "### Congratulations! ###\n"
					<< "########################\n\n";

	// change to winning player colour for winner description 
	// n.b. g_iActivePlayer == either k_iBoardSlot_Player1 or 
	SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ g_iActivePlayer ] );
	cout			<< "'" << k_achBoardPieces[ g_iGameState ] << "' player has won!\n\n";
	SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ k_iBoardSlot_Empty ] );

	cout			<<	"Winning line from [" 
					<< ( g_aaiWinningRow[ 0 ][ 0 ] + 1 ) << "," << ( g_aaiWinningRow[ 0 ][ 1 ] + 1 )<< "] to ["
					<< ( g_aaiWinningRow[ k_iWinLineLength - 1 ][ 0 ] + 1 ) << "," << ( g_aaiWinningRow[ k_iWinLineLength - 1 ][ 1 ] + 1 ) <<"]\n"
					<< "Press 'y' followed by return to play again, any other input to quit\n";
	
	// get 1st input char and flush the input buffer to prevent unwanted immediate returns from cin.get() later
	char chInput = cin.get();
	cin.sync();

	return (( 'y' == chInput ) || ( 'Y' == chInput )) ? false : true;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool DisplayDrawMessageQueryPlayAgain( void )
{
	RenderBoard();

	cout << endl	<< "########################\n"
					<< "### Game is a draw!! ###\n"
					<< "########################\n\n";

	// change to winning player colour for winner description 
	// n.b. g_iActivePlayer == either k_iBoardSlot_Player1 or 
	SetConsoleTextAttribute( g_hwndConsole, k_ai8BoardElementColours[ k_iBoardSlot_Empty ] );

	cout			<< "Press 'y' followed by return to play again, any other input to quit\n";
	
	// get 1st input char and flush the input buffer to prevent unwanted immediate returns from cin.get() later
	char chInput = cin.get();
	cin.sync();

	return (( 'y' == chInput ) || ( 'Y' == chInput )) ? false : true;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void Update_CheckForWin( void )
{
	// uber basic optimisations on the searches: 
	// * we only check for lines of the active player's pieces
	// * we check the 4 directions separately, and only check a subset of the board for each
	// * for vertical and diagonal we always search DOWN the board (gravity drop prevents need for upward)
	//
	// there are lots more, but I'll leave them to you to work out
	if(		Update_CheckForRow_Vertical() 
		||	Update_CheckForRow_Horizontal() 
		||	Update_CheckForRow_LU2RD()
		||	Update_CheckForRow_LD2RU() )
	{
		g_iGameState = g_iActivePlayer;
	}
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool Update_CheckForRow_Vertical( void )
{
	// check for vertical line:
	// * start at [0][0]
	// * end at [ k_iBoardWidth ][ iMaxRowToCheck ]
	// * always check for possible end of line before the middle pieces
	int iMaxRowToCheck	= ( k_iBoardHeight - ( k_iWinLineLength - 1 ) );

	for( int iColumn = 0; iColumn < k_iBoardWidth; ++iColumn )
	{
		for( int iRow = 0; iRow < iMaxRowToCheck; ++iRow )
		{
			// N.B. set the winning row as we go
			if( g_aaiBoard[ iColumn ][ iRow ] == g_iActivePlayer )
			{
				g_aaiWinningRow[ 0 ][ 0 ] = iColumn;
				g_aaiWinningRow[ 0 ][ 1 ] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 becuase the 0th was the one we've already found at the start of the line
				int iLinePosIndex = ( k_iWinLineLength - 1 );
				
				for( ; iLinePosIndex > 0; --iLinePosIndex )
				{
					int iCheckRow = iRow + iLinePosIndex;

					if( g_aaiBoard[ iColumn ][ iCheckRow ] == g_iActivePlayer )
					{												
						g_aaiWinningRow[ iLinePosIndex ][ 0 ] = iColumn;
						g_aaiWinningRow[ iLinePosIndex ][ 1 ] = iCheckRow;						
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if(	0 == iLinePosIndex )
				{
					return true;
				}
			}
		}
	}

	// didn't find a vertical line
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool Update_CheckForRow_Horizontal ( void )
{
	// check for horizontal line:
	// * start at [0][0]
	// * end at [ iMaxColumnToCheck ][ k_iBoardHeight ]
	// * always check for possible end of line before the middle pieces
	int iMaxColumnToCheck= ( k_iBoardWidth - ( k_iWinLineLength - 1 ) );

	for( int iColumn = 0; iColumn < iMaxColumnToCheck; ++iColumn )
	{
		for( int iRow = 0; iRow < k_iBoardHeight; ++iRow )
		{
			// N.B. set the winning row as we go
			if( g_aaiBoard[ iColumn ][ iRow ] == g_iActivePlayer )
			{
				g_aaiWinningRow[ 0 ][ 0 ] = iColumn;
				g_aaiWinningRow[ 0 ][ 1 ] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 becuase the 0th was the one we've already found at the start of the line
				int iLinePosIndex = ( k_iWinLineLength - 1 );
				
				for( ; iLinePosIndex > 0; --iLinePosIndex )
				{
					int iCheckColumn = iColumn + iLinePosIndex;

					if( g_aaiBoard[ iCheckColumn ][ iRow ] == g_iActivePlayer )
					{												
						g_aaiWinningRow[ iLinePosIndex ][ 0 ] = iCheckColumn;
						g_aaiWinningRow[ iLinePosIndex ][ 1 ] = iRow;						
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if(	0 == iLinePosIndex )
				{
					return true;
				}
			}
		}
	}

	// didn't find a line
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool Update_CheckForRow_LU2RD( void )
{
	// check for left,up to right,down diagonal:
	// * start at [0][0]
	// * end at [ iMaxColumnToCheck ][ iMaxRowToCheck ]
	// * always check for possible end of line before the middle pieces
	int iMaxColumnToCheck	= ( k_iBoardWidth - ( k_iWinLineLength - 1 ) );
	int iMaxRowToCheck		= ( k_iBoardHeight - ( k_iWinLineLength - 1 ) );

	for( int iColumn = 0; iColumn < iMaxColumnToCheck; ++iColumn )
	{
		for( int iRow = 0; iRow < iMaxRowToCheck; ++iRow )
		{
			// N.B. set the winning row as we go
			if( g_aaiBoard[ iColumn ][ iRow ] == g_iActivePlayer )
			{
				g_aaiWinningRow[ 0 ][ 0 ] = iColumn;
				g_aaiWinningRow[ 0 ][ 1 ] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 becuase the 0th was the one we've already found at the start of the line
				int iLinePosIndex = ( k_iWinLineLength - 1 );
				
				for( ; iLinePosIndex > 0; --iLinePosIndex )
				{
					int iCheckColumn	= iColumn + iLinePosIndex;
					int iCheckRow		= iRow + iLinePosIndex;

					if( g_aaiBoard[ iCheckColumn ][ iCheckRow ] == g_iActivePlayer )
					{												
						g_aaiWinningRow[ iLinePosIndex ][ 0 ] = iCheckColumn;
						g_aaiWinningRow[ iLinePosIndex ][ 1 ] = iCheckRow;						
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if(	0 == iLinePosIndex )
				{
					return true;
				}
			}
		}
	}

	// didn't find a line
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool Update_CheckForRow_LD2RU( void )
{
	// check for left,up to right,down diagonal:
	// * start at [ iMinColumnToCheck ][ iMinRowToCheck ]
	// * end at [ k_iBoardWidth ][ k_iBoardHeight ]
	// * always check for possible end of line before the middle pieces
	int iMaxColumnToCheck	= ( k_iBoardWidth - ( k_iWinLineLength - 1 ) );
	int iMinRowToCheck		= ( k_iWinLineLength - 1 );

	for( int iColumn = 0; iColumn < iMaxColumnToCheck; ++iColumn )
	{
		for( int iRow = iMinRowToCheck; iRow < k_iBoardHeight; ++iRow )
		{
			// N.B. set the winning row as we go
			if( g_aaiBoard[ iColumn ][ iRow ] == g_iActivePlayer )
			{
				g_aaiWinningRow[ 0 ][ 0 ] = iColumn;
				g_aaiWinningRow[ 0 ][ 1 ] = iRow;

				// now come back along the line filling in g_aaiWinningRow
				// N.B. we're checking to > 0 because the 0th was the one we've already found at the start of the line
				int iLinePosIndex = ( k_iWinLineLength - 1 );
				
				for( ; iLinePosIndex > 0; --iLinePosIndex )
				{
					int iCheckColumn	= iColumn + iLinePosIndex;
					int iCheckRow		= iRow - iLinePosIndex;

					if( g_aaiBoard[ iCheckColumn ][ iCheckRow ] == g_iActivePlayer )
					{												
						g_aaiWinningRow[ iLinePosIndex ][ 0 ] = iCheckColumn;
						g_aaiWinningRow[ iLinePosIndex ][ 1 ] = iCheckRow;						
					}
					else
					{
						break;
					}
				}

				// if iLinePosIndex == 0 at this point it means we found a whole line
				if(	0 == iLinePosIndex )
				{
					return true;
				}
			}
		}
	}

	// didn't find a line
	return false;
}

