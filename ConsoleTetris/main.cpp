#include <iostream>
using namespace std;

#include <Windows.h>

#include <thread>


wstring tetromino[7]; // Ont cr�er un tableau de texte de taille 7 pour les pi�ces de tetris

// On d�finit la taille du terrain de jeu
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr; // Ont d�clare le pointeur qui permettra de ranger les �l�ments du terrain

// On d�finit la taille de la Console
int nScreenWidth = 80;		// Nombre de colones de la Console
int nScreenHeight = 30;		// Nombre de lignes de la Console


int Rotate(int pieceX, int pieceY, int rotation)
{
	// retourne la bonne equation en fonction de la roation
	switch (rotation % 4) // modulo 4 pour avoir le nombre de rotation de 90�
	{
	case 0: return pieceY * 4 + pieceX;			// 0 degr�s
	case 1: return 12 + pieceY - (pieceX * 4);	// 90 degr�s
	case 2: return 15 - (pieceY * 4) - pieceX;	// 180 degr�s
	case 3: return 3 - pieceY + (pieceX * 4);	// 270 degr�s
	}
	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int pieceX = 0; pieceX < 4; pieceX++)
		for (int pieceY = 0; pieceY < 4; pieceY++)
		{
			// On r�cup�re l'index de l'�l�ment dans la pi�ce en appliquant la rotation
			int pieceIndex = Rotate(pieceX, pieceY, nRotation);

			// On r�cup�re l'index de l'�l�ment dans le terrain de jeu
			int fieldIndex = (nPosY + pieceY) * nFieldWidth + (nPosX + pieceX);


			if (nPosX + pieceX >= 0 && nPosX + pieceX < nFieldWidth) // Si la position l'�l�ment est compris dans la largeur du terrain
			{
				if (nPosY + pieceY >= 0 && nPosY + pieceY < nFieldHeight) // Si la position l'�l�ment est compris dans la hauteur du terrain
				{
					if (tetromino[nTetromino][pieceIndex] == L'X' && pField[fieldIndex] != 0) // On test si l'�l�ment � l'index de la pi�ce est = � X (donc � une bloc) et que l'�l�ment � l'index dans le terrain est diff�rent de 0 (cad qu'il n'es pas vide)
						return false; // On retourne faux, la pi�ce ne rentre pas dans la case car elle est d�j� occup�e
				}
			}
		}

	return true;
}

int main()
{

	// Creation des assets
#pragma region Assets
	
	// ..X...X...X...X.
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	// ..X..XX..X......
	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	// .X...XX...X.....
	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	// .....XX..XX.....
	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	// ..X..XX...X.....
	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	// .....XX...X...X.
	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	// .....XX..X...X..
	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

#pragma endregion

	pField = new unsigned char[nFieldWidth * nFieldHeight]; // On assigne au pointeur la valeur d'un tableau de char, de taille LargeurDuTerrain * HauteurDuTerrain

	// Cr�ation des bordures du terrain de jeu
#pragma region SetFieldBondaries

for (int x = 0; x < nFieldWidth; x++) // Pour chaque �l�ments de chaque colone du tableau
	for (int y = 0; y < nFieldHeight; y++) // Pour chaque �l�ments de chaque ligne du tableau
		pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0; // Pour chaque �l�ment du tableau on assigne la valeur 9 si l'�l�ment est �gal � la bordure de gauche ou � celle de doite ou celle d'en bas, sinon on assigne la valeur 0
		/* Equilavent de
		{
		if (x == 0)							// si la valeur de x = 0 alors cela veut dire qu'on est sur la bordure de gauche
			pField[y*nFieldWidth + x] = 9;
		else if (x == nFieldWidth - 1)		// si la valeur de x = LargeurDuTerrain - 1 alors cela veut dire qu'on est sur la bordure de droite
			pField[y*nFieldWidth + x] = 9;
		else if (y == nFieldHeight - 1)		// si la valeur de y = HauteurDuTerrain - 1 alors cela veut dire qu'on est sur la bordure d'en bas
			pField[y*nFieldWidth + x] = 9;
		else pField[y*nFieldWidth + x] = 0;

		}
		*/


#pragma endregion

	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight]; // On d�clare ET assigne au pointeur la valeur d'un tableau de char, de taille LargeurFenetre * HauteurFenetre	
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' '; // Pour chaque �l�ments du tableau, remplit avec un blanc

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // On cr�er notre Console de type Texte
	SetConsoleActiveScreenBuffer(hConsole); // On Active la Console
	DWORD dwBytesWritten = 0;

	// Game Logic Stuff
	bool bGameOver = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4]; // D�finition d'un Tableau de bool pour set les valeurs des touches
	bool bRotateHold = false; // D�finition d'un bool pour set si la touche de rotation est maintenue enfoc�e

	while (!bGameOver)
	{
		// GAME TIMING =======================

		this_thread::sleep_for(50ms);

		// INPUT =============================

		for (int k = 0; k < 4; k++)								// R	L	D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0; // Check si chaque touche est press�e (FlecheGauche, FlecheDroite, FLecheBas, Z)

		// GAME LOGIC ========================

		nCurrentX += (bKey[0] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0);
		nCurrentX -= (bKey[1] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0);
		nCurrentY += (bKey[2] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0);
		/* Equivalent de 
		if (bKey[0]) // FlecheDroite
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
			{
				nCurrentX = nCurrentX + 1;
			}
		}
		
		if (bKey[1]) // FlecheGauche
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
			{
				nCurrentX = nCurrentX - 1;
			}
		}

		if (bKey[2]) // FlecheBas
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				nCurrentY = nCurrentY + 1;
			}
		}
		*/

		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && (DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0);
			bRotateHold = true;
		}
		else
			bRotateHold = false;


		// RENDER OUTPUT =====================
		
		
		// On "affiche" le terrain -> comprenant les bordures + les pieces de tetris + les espaces vides
		for (int x = 0; x < nFieldWidth; x++) // Pour chaque �l�ments de chaque colone du tableau
			for (int y = 0; y < nFieldHeight; y++) // Pour chaque �l�ments de chaque ligne du tableau
				screen[(y + 2)*nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]]; // Pour chaque �l�ment de la console (offset de 2), on assigne un char en fonction de la valeur de l'�l�ment au m�me index dans le tableau du terrain de jeu

		// On "affiche" la pi�ce control�e par le joueur
		for (int pieceX = 0; pieceX < 4; pieceX++) // Pour chaque �l�ments de chaque colone du tableau
			for (int pieceY = 0; pieceY < 4; pieceY++) // Pour chaque �l�ments de chaque ligne du tableau
				if (tetromino[nCurrentPiece][Rotate(pieceX, pieceY, nCurrentRotation)] == L'X') // Si l'�l�ment est un bloc
					screen[(nCurrentY + pieceY + 2)*nScreenWidth + (nCurrentX + pieceX + 2)] = nCurrentPiece + 65; // Pour chaque �l�ment de la console (offset de 2), on assigne un char correspondant � l'index de la pi�ce + 65 (code ascii de la lettre A)


		// Affichage d'une frame
		WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten); // On affiche dans la console, notre tableau "screen", de taille LargeurFenetre * HauteurFenetre, en d�butant � la position 0,0
	}

	return 0;
}