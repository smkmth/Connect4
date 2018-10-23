#pragma once
class CGameBoard
{
private:

	//board size / num of pieces needed
	static const int sm_iBoardWidth = 7;
	static const int sm_iBoardHeight = 6;
	static const int sm_iNumOfPieces = 4;
	static const int sm_iWinLineLength = 4;
	//array for the board
	int g_aaiBoard[sm_iBoardWidth][sm_iBoardHeight];
	// number of turns
	int m_iSpacesRemaining = sm_iBoardWidth * sm_iBoardHeight;

	//pointer to the handle thingy
	HANDLE* phwndConsole;


	//// board state management
	enum EBoardState
	{
		k_eBoardSlot_Empty
		, k_eBoardSlot_Player1
		, k_eBoardSlot_Player2
		, k_eBoardSlot_Frame

	};
	//vars
	int g_iActivePlayer;

	// game win state 
	const int k_iState_Playing = 0;
	const int k_iState_Won_Player1 = k_eBoardSlot_Player1;
	const int k_iState_Won_Player2 = k_eBoardSlot_Player2;
	const int k_iState_Draw = k_iState_Won_Player2 + 1;


	// set to player number when a player has won
// n.b. uses k_iBoardSlot_xxx values
	int g_iGameState = k_iState_Playing;


	

	// number of turns
	int g_iSpacesRemaining = sm_iBoardWidth * sm_iBoardHeight;

	int g_aaiWinningRow[sm_iWinLineLength][2];

	// characters used to represent pieces on the board, this array is indexed by using k_eBoardSlot_XXX
	const char k_achBoardPieces[sm_iNumOfPieces] =
	{
		' ', // no piece - g_achBoardPieces[ k_eBoardSlot_Empty ]
		'@', // player 1 - g_achBoardPieces[ k_eBoardSlot_Player1 ]
		'O', // player 2 - g_achBoardPieces[ k_eBoardSlot_Player2 ]
		'!', // not used, but provided since k_eBoardSlot_Frame exists
	};


	const char k_ai8BoardElementColours[] =
	{
		7,	// white on black
		12,	// red on black
		14,	// yellow on black
		9,	// blue on black
	};



	
public:
	//constructer
	CGameBoard(HANDLE* phwndConsole);
	//getters
	int GetBoardWidth();
	int GetBoardHeight();
	int GetNumberOfTurns();
	int GetGameState();
	int GetActivePlayer();
	void SetNumberOfTurns(int val);
	void SetGameState(int val);
	void SetActivePlayer(int val);
	//functions
	void RenderBoard();
	void UpdateGame(int iArgColumn);
	void Update_CheckForWin();
	bool Update_CheckForRow_Vertical();
	bool Update_CheckForRow_Horizontal();
	bool Update_CheckForRow_LU2RD();
	bool Update_CheckForRow_LD2RU();
	bool ConfirmQuitGame();
	bool DisplayWinMessageQueryPlayAgain();
	bool DisplayDrawMessageQueryPlayAgain();
	void Reset();
};

