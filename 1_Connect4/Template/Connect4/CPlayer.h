#pragma once
#include "CGameBoard.h";
class CPlayer
{
private:
	// special values returned by (and passed to) fn GetPlayerInput()
	// n.b. all valid rows are in range 0 to ( k_iBoardWidth - 1 )
	int iPlayerInput_Quit;

	// n.b. -ve values from -1 to -k_iBoardWidth are used to return input if the chosen column is full
	int iPlayerInput_None;
	int iPlayerInput_Invalid;
	
	CGameBoard* cBoard;

public:
	CPlayer( CGameBoard* cboard );
	~CPlayer();
	int	GetPlayerInput(int iArgLastInput);


};

