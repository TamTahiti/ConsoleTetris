#include <iostream>
using namespace std;

#include <Windows.h>

#include <thread>

#include <vector>


wstring tetromino[7]; // Ont cr�er un tableau de texte de taille 7 pour les pi�ces de tetris

// On d�finit la taille du terrain de jeu
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr; // Ont d�clare le pointeur qui permettra de ranger les �l�ments du terrain

// On d�finit la taille de la Console
int nScreenWidth = 80;		// Nombre de colones de la Console
int nScreenHeight = 30;		// Nombre de lignes de la Console


int Rotate(int px, int py, int rotation)
{
	// retourne la bonne equation en fonction de la roation
	switch (rotation % 4) // modulo 4 pour avoir le nombre de rotation de 90�
	{
	case 0: return py * 4 + px;			// 0 degr�s
	case 1: return 12 + py - (px * 4);	// 90 degr�s
	case 2: return 15 - (py * 4) - px;	// 180 degr�s
	case 3: return 3 - py + (px * 4);	// 270 degr�s
	}
	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// On r�cup�re l'index de l'�l�ment dans la pi�ce en appliquant la rotation
			int pieceIndex = Rotate(px, py, nRotation);

			// On r�cup�re l'index de l'�l�ment dans le terrain de jeu
			int fieldIndex = (nPosY + py) * nFieldWidth + (nPosX + px);


			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) // Si la position l'�l�ment est compris dans la largeur du terrain
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) // Si la position l'�l�ment est compris dans la hauteur du terrain
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
#pragma region Screen

	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight]; // On d�clare ET assigne au pointeur la valeur d'un tableau de char, de taille LargeurFenetre * HauteurFenetre	
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' '; // Pour chaque �l�ments du tableau, remplit avec un blanc

#pragma endregion
#pragma region Console

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // On cr�er notre Console de type Texte
	SetConsoleActiveScreenBuffer(hConsole); // On Active la Console
	DWORD dwBytesWritten = 0;

#pragma endregion

	// Game Logic Stuff
#pragma region Variables

	bool bGameOver = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = (nFieldWidth / 2) - 2;
	int nCurrentY = 0;

	bool bKey[4]; // D�finition d'un Tableau de bool pour set les valeurs des touches
	bool bRotateHold = false; // D�finition d'un bool pour set si la touche de rotation est maintenue enfoc�e

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

#pragma endregion

	while (!bGameOver)
	{
#pragma region GameTiming

		// GAME TIMING =======================

		this_thread::sleep_for(50ms); // Game Tick
		nSpeedCounter++; // On compte le nombre de tick
		bForceDown = (nSpeedCounter == nSpeed); // On set ForceDown � true si le nombre de tick est �gal � la vitesse d�finie, sinon on set false

#pragma endregion
#pragma region Input

		// INPUT =============================

		for (int k = 0; k < 4; k++)								// R	L	D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0; // Check si chaque touche est press�e (FlecheGauche, FlecheDroite, FLecheBas, Z)

#pragma endregion
#pragma region GameLogic

		// GAME LOGIC ========================
#pragma region PieceTransform

		nCurrentX += (bKey[0] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0); // On d�place la pi�ce vers la droite si on clique sur la fleche de droite et qu'il y a de la place sinon on ne la d�place pas
		nCurrentX -= (bKey[1] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0); // On d�place la pi�ce vers la gauche si on clique sur la fleche de gauche et qu'il y a de la place sinon on ne la d�place pas
		nCurrentY += (bKey[2] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0); // On d�place la pi�ce vers le bas si on clique sur la fleche du bas et qu'il y a de la place sinon on ne la d�place pas
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

		if (bKey[3]) // Si on clique sur Z
		{
			nCurrentRotation += (!bRotateHold && (DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0); // On tourne la pi�ce si on clique sur Z sans maintenir et qu'il y a de la place sinon on ne la tourne pas
			bRotateHold = true;
		}
		else
			bRotateHold = false;

#pragma endregion

		if (bForceDown)
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) // Check s'il y a de la place en dessous de la pi�ce
				nCurrentY++; // Si Oui, on d�place la pi�ce vers le bas
			else
			{
				// Bloquer la pi�ce actuelle sur le terrain de jeu
				for (int px = 0; px < 4; px++)			// Pour chaque �l�ments de chaque colone du tableau de la Piece
					for (int py = 0; py < 4; py++)		// Pour chaque �l�ments de chaque ligne du tableau de la Piece
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') // Si l'�l�ment est un bloc
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1; // On update la valeur de la case dans le terrain de jeu = Index de la pi�ce +1 (+1 car 0 signifie une case vide dans notre terrain, donc si l'index de la pi�ce �tait 0 �a aurait pos� probl�me)
				
				nPieceCount++;					// On augmente le nombre de pi�ces qui ont �t� plac�e par le joueur
				if (nPieceCount % 10 == 0)		// Si le nombre est un multiple de 10
					if (nSpeed >= 10) nSpeed--;	// Si la vitesse est >= 10 on diminue la vitesse (qui correspond au nombre de tick avant de forcer la pi�ce � dscendre)

				// Check si on a fait une ligne
				for (int py = 0; py < 4; py++) // Pour chaque �l�ments de chaque ligne du tableau de la Piece
					if (nCurrentY + py < nFieldHeight - 1) // Check si on ne d�passe pas la hauteur du terrain de jeu
					{
						bool bLine = true; // On part du principe qu'il y a une ligne
						for (int x = 0; x < nFieldWidth; x++) // Pour chaque �l�ments de chaque colone du terrain de jeu
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + x]) != 0; // On set False si une case du terrain l� ou se trouve la pi�ce est vide

						if (bLine) // Si il y a une ligne
						{
							// Supprimer la ligne, remplacer par =
							for (int x = 1; x < nFieldWidth - 1; x++)// Pour chaque �l�ments de chaque colone du terrain de jeu
								pField[(nCurrentY + py) * nFieldWidth + x] = 8; // On set la valeur de la case � 8
						
							vLines.push_back(nCurrentY + py);
						}

					}

				nScore += 25; // Pour chaque pi�ce plac�e, on gagner 25
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100; // Si on fait une ligne, pour chque ligne on gagne 100 mais c'est exponentiel plus on casse de ligne en m�me temps


				// Choisir la prochaine pi�ce
				nCurrentX = (nFieldWidth / 2) - 2; // On set la position X de la pi�ce � la moitier du terrain de jeu - la moitier de la taille de la pi�ce
				nCurrentY = 0; // On set la position Y de la pi�ce en haut du terrain de jeu
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7; // On Choisi un nombre "al�atoire" entre 0 et 6


				// Si la pi�ce ne rentre pas -> Game Over
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0; // On reset le speedCounter une fois qu'on a tout calcul�
		}

#pragma endregion
#pragma region RenderOutput
		
		// RENDER OUTPUT =====================		
		
		// On "affiche" le terrain -> comprenant les bordures + les pieces de tetris + les espaces vides
		for (int x = 0; x < nFieldWidth; x++)			// Pour chaque �l�ments de chaque colone du tableau du terrain de jeu
			for (int y = 0; y < nFieldHeight; y++)		// Pour chaque �l�ments de chaque ligne du tableau du terrain de jeu
				screen[(y + 2)*nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]]; // Pour chaque �l�ment de la console (offset de 2), on assigne un char en fonction de la valeur de l'�l�ment au m�me index dans le tableau du terrain de jeu

		// On "affiche" la pi�ce control�e par le joueur
		for (int px = 0; px < 4; px++)			// Pour chaque �l�ments de chaque colone du tableau de la Piece
			for (int py = 0; py < 4; py++)		// Pour chaque �l�ments de chaque ligne du tableau de la Piece
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') // Si l'�l�ment est un bloc
					screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65; // Pour chaque �l�ment de la console (offset de 2), on assigne un char correspondant � l'index de la pi�ce + 65 (code ascii de la lettre A)

		// Affichage du Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth +6], 32,L"SCORE : %8d", nScore);


		if (!vLines.empty())
		{
			// Affichage d'une frame ( pour afficher les lignes )
			WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // On alonge un peut la dur�e du tick

			for (auto &v : vLines) // ???
				for (int x = 1; x < nFieldWidth - 1; x++) // Pour chaque colone du terrain de jeu sans prendre en compte les bordures
				{
					for (int y = v; y > 0; y--) // Pour chaque ligne du terrain de jeu en partant du bas vers le haut
						pField[y * nFieldWidth + x] = pField[(y - 1) * nFieldWidth + x]; // Assigne la valeur de la case avec celle de la case au dessus		
					pField[x] = 0; // On set la case du haut � 0
				}

			vLines.clear();
		}

		// Affichage d'une frame
		WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten); // On affiche dans la console, notre tableau "screen", de taille LargeurFenetre * HauteurFenetre, en d�butant � la position 0,0

#pragma endregion
	}

	// Game Over
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << nScore << endl;
	system("pause");

	return 0;
}