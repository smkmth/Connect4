#pragma once



//////////////////////////////////////////////////////////////////////////
// function 'declarations' also known as 'prototypes'
// as we know, these must appear before they are first called
void	Reset(void);
//int		GetPlayerInput(int iArgLastInput);
void	UpdateGame(int iArgColumn);
//void	RenderBoard						( void );

bool	ConfirmQuitGame(void);
bool	DisplayWinMessageQueryPlayAgain(void);
bool	DisplayDrawMessageQueryPlayAgain(void);

void	Update_CheckForWin(void);
bool	Update_CheckForRow_Vertical(void);
bool	Update_CheckForRow_Horizontal(void);
bool	Update_CheckForRow_LU2RD(void);
bool	Update_CheckForRow_LD2RU(void);