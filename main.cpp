// LIBRARIES

#include <iostream>  // <- Input and output
#include <locale>    // <- Change language
#include <windows.h> // <- Redimention window
#include <conio2.h>  // <- Use gotoxy and colors
#include <cstdlib>   // <- Use random
#include <list>		 // <- Use list
#include <vector>	 // <- Use vector
#include <iterator>  // <- Manipulate list
#include <ctime>	 // <- Use time in random numbers
#include <windows.h>
#include <fstream> 	 // <- Write and read files

// FILES

#include "classes.cpp"

// CONSTANTS

#define WINDOW_WIDTH 80

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_RIGHT 77
#define KEY_LEFT 75
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_BACKSPACE 8

#define GAME_WIDTH 25
#define GAME_HEIGHT 15

using namespace std;





// VARI�VEIS GLOBAIS

// Marcadores
char const
	CROSSHAIR 		  = 'x', // <- Mira
	OPEN_SHOT_MARKER  = '>', // <- 
	CLOSE_SHOT_MARKER = '<'; // <- 

// �gua
char const AGUA_CHAR = '#';
COLORS const AGUA_COLOR = LIGHTBLUE;

// Tipos pr�-definidos
Tipo *tipos[] = {
	new Tipo("Submarino"   , 3, 1, '1', LIGHTCYAN),
	new Tipo("Encoura�ado" , 2, 2, '2', GREEN    ),
	new Tipo("Cruzador"    , 3, 3, '3', DARKGRAY ),
	new Tipo("Hidroavi�o"  , 4, 1, '4', LIGHTRED ),
	new Tipo("Porta-avi�es", 1, 4, '5', YELLOW   ),
};





// ENUMS

enum GameStatus
{
	VICTORY,
	DEFEAT,
	EXIT
};





// FUNCTIONS

/*
void gotoxy2(int x, int y)
{
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		(COORD) {x - 1, y - 1}
	);
}

int wherex2()
{
    CONSOLE_SCREEN_BUFFER_INFO con;
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if(hcon != INVALID_HANDLE_VALUE &&
        GetConsoleScreenBufferInfo(hcon,&con)) 
    {
        return con.dwCursorPosition.X;
    }
    
    return 0;
}

int wherey2()
{
	int pos[2];
    CONSOLE_SCREEN_BUFFER_INFO con;
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if(hcon != INVALID_HANDLE_VALUE &&
    	GetConsoleScreenBufferInfo(hcon,&con)) 
    {
        return con.dwCursorPosition.Y;
    }
    
    return 0;
}
*/

// Coloca o cursor em [1; 1]
void hideCursor()
{
	gotoxy(1, 1);
}

// Interrompe o sistema at� que a key passada seja pressionada
void waitKey(int key)
{
	char input;
	
	do
	{
		input = getch();
	}
	while(input != key);
}

int getPosToCenter(string str)
{
	return WINDOW_WIDTH / 2 - str.size() / 2;
}

// Gerar embarca��o
template <size_t rows, size_t cols>
list<Embarcacao*> gerarEmbarcacoes(Peca (&field)[rows][cols], Tipo *tipoAtual)
{
	list<Embarcacao*> ships;
	
	for(int qtd = 0; qtd < tipoAtual->getQtd(); qtd++)
	{
		// Posicionar embarca��o
		bool sortingCoord = true;
		while(sortingCoord)
		{
			// Sortear coordenada inicial
			int x, y;
			while(true)
			{
				x = rand() % GAME_WIDTH;
				y = rand() % GAME_HEIGHT;
				
				if(field[x][y].isWater())
					break;
			}
			
			// Coordenadas
			Coordenada coords[tipoAtual->getSize()];
			coords[0].setX(x);
			coords[0].setY(y);
			
			// Dire��es dispon�veis
			list<int> directions = { 0, 1, 2, 3 };
			
			// Direcionar embarca��o
			bool directing = true;
			while(directing)
			{
				// Se n�o existirem mais dire��es dispon�veis, volta e pega novas coordenadas
				if(directions.empty())
					break;
				
				// Sorteia uma dire��o
				int iDir = rand() % directions.size();
				
				// Pega a dire��o sorteada
				list<int>::iterator dir = directions.begin();
				advance(dir, iDir);
				
				// Apaga a dire��o
				directions.erase(dir);
				
				// Salvar coordenadas (caso v�lidas)
				bool freeWay = true;
				switch(*dir)
				{
					// Up
					case 0:
						for(int d = 1; d < tipoAtual->getSize(); d++)
							if(field[x][y - d].isWater() && y - d >= 0)
							{
								coords[d].setX(x);
								coords[d].setY(y - d);
							}
							else
							{
								freeWay = false;
								break;
							}
						break;
					// Down
					case 1:
						for(int d = 1; d < tipoAtual->getSize(); d++)
							if(field[x][y + d].isWater() && y + d <= GAME_HEIGHT - 1)
							{
								coords[d].setX(x);
								coords[d].setY(y + d);
							}
							else
							{
								freeWay = false;
								break;
							}
						break;
					// Right
					case 2:
						for(int d = 1; d < tipoAtual->getSize(); d++)
							if(field[x + d][y].isWater() && x + d <= GAME_WIDTH - 1)
							{
								coords[d].setX(x + d);
								coords[d].setY(y);
							}
							else
							{
								freeWay = false;
								break;
							}
						break;
					// Left
					case 3:
						for(int d = 1; d < tipoAtual->getSize(); d++)
							if(field[x - d][y].isWater() && x - d >= 0)
							{
								coords[d].setX(x - d);
								coords[d].setY(y);
							}
							else
							{
								freeWay = false;
								break;
							}
						break;
				}
				
				// Sair dos loops e passar para pr�xima embarca��o
				if(freeWay)
				{
					Embarcacao* e = new Embarcacao(tipoAtual);
					//Peca* pecas[tipoAtual->getSize()];
					list<Peca*> pecas;
					
					for(int iC = 0; iC < sizeof(coords) / sizeof(*coords); iC++)
					{
						int i = coords[iC].getX();
						int j = coords[iC].getY();
						
						field[i][j].setEmbarcacao(e);
						
						//pecas[iC] = &field[i][j];
						pecas.push_back(&field[i][j]);
					}
					
					e->setPecas(pecas);
					ships.push_back(e);
					
					directing = false;
					sortingCoord = false;
				}
			}
		}
	}
	
	return ships;
}

// Return the positon in the ranking
int saveScore(string name, int score)
{
	// Write score file
	ofstream inFile;
	inFile.open("score.txt");
	inFile << name << " - " << score;
	inFile.close();
	
	return 0;
}





// INTERFACE FUNCTIONS

int menu(string title, vector<string> options, int opt)
{
	// CONSTRUIR MEN�
	int biggerTxt = title.size();
	
	for(string option: options)
		if(option.size() + 4 > biggerTxt)
			biggerTxt = option.size() + 4;
			
	string border = "----";
	for(int i = 0; i < biggerTxt; i++)
		border += "-";
	border += "\n";
	
	int x = getPosToCenter(border);
	
	// Top border
	gotoxy(x, wherey());
	textcolor(WHITE);
	cout << border;
	
	// Title
	gotoxy(x, wherey());
	cout << "|";
	const int initX = wherex() + 1;
	const int initY = wherey() + 2;
	
	gotoxy(x + border.size() / 2 - title.size() / 2, wherey()); // <- Centralizar t�tulo
	textcolor(LIGHTMAGENTA);
	cout << title;
	gotoxy(x + biggerTxt + 3, wherey());
	textcolor(WHITE);
	cout << "|\n";
	
	gotoxy(x, wherey());
	cout << border;
		
	for(string option: options)
	{
		gotoxy(x, wherey());
		cout << "|   " << option;
		gotoxy(x + biggerTxt + 3, wherey());
		cout << "|\n";
	}
	
	gotoxy(x, wherey());
	cout << border;
	
	// MOVIMENTAR CURSOR
	textcolor(LIGHTMAGENTA);
	bool choosing = true;
	do
	{
		gotoxy(initX, initY + opt);
		cout << ">";
		hideCursor();
		
		int arrowDir = 0;
		
		switch(getch())
		{
			// Up
			case KEY_UP:
			    if(opt > 0)
			    	arrowDir = -1;
			    break;
			// Down
			case KEY_DOWN:
			    if(opt < 3)
			    	arrowDir = 1;
			    break;
			// Enter
			case KEY_ENTER:
			    choosing = false;
			    break;
		}
		
		gotoxy(initX, initY + opt);
		cout << " ";
		
		opt += arrowDir;
	}
	while(choosing);
	
	int opt2 = opt;
	return opt2;
}

// Jogo
GameStatus game()
{
	//return GameStatus::VICTORY;
	//return GameStatus::DEFEAT;
	
	// Matriz do jogo
	Peca field[GAME_WIDTH][GAME_HEIGHT];
	
	// Embarca��es n�o afundadas
	list<Embarcacao*> shipsNotSunk;

	// Tiros
	int tiros = 500;
	int inWater = 0;
	
	// GERAR EMBARCA��ES
	for(Tipo *tipo: tipos)
		shipsNotSunk.merge(
			gerarEmbarcacoes(field, tipo)
		);
	
	// Vari�veis para movimento do cursor
	int x = 0;
	int y = 0;
	bool revealing = false;
	bool clearRelealed = false;
	
	// Loop: permite reprintar os elementos visuais
	while(true)
	{
		system("cls");
		
		/*
		// PRINTAR CAMPO (Revelar tudo)
		for(int j = 0; j < GAME_HEIGHT; j++)
		{
			cout << "\n";
			
			for(int i = 0; i < GAME_WIDTH; i++)
			{
				Peca *peca = &field[i][j];
				
				if(peca->isWater())
				{
					textcolor(AGUA_COLOR);
					cout << " " << AGUA_CHAR;
				}
				else
				{
					textcolor(peca->getEmbarcacao()->getTipo()->getColor());
					cout << " " << peca->getEmbarcacao()->getTipo()->getCharacter();
				}
			}
		}
		cout << "\n\n";
		*/
		
		// TIROS RESTANTES
		textcolor(WHITE);
		cout << "\n ------------------------";
		cout << "\n | Tiros restantes: ";
		
		const int
			xTiros = wherex(),
			yTiros = wherey();
		
		cout << tiros;
		gotoxy(xTiros + 3, yTiros);
		cout << " |\n ------------------------";
		
		// COORDENADAS DO EIXO X
		
		// Espa�o destinado a cada coordenada
		int charSpace = to_string(GAME_WIDTH).size();
		
		// Espa�o a ser colocado a cada linha antes da primeira pe�a
		string initRowSpace = "";
		for(int i2 = 0; i2 < charSpace / 2; i2++)
			initRowSpace += " ";
			
		// Espa�o entre as pe�as
		string betweenPieces = "";
		for(int i2 = 0; i2 < charSpace; i2++)
			betweenPieces += " ";
		
		// Linha (borda) horiontal do campo
		string line = "";
		for(int i = 0; i < GAME_WIDTH * (charSpace + 1) + initRowSpace.size(); i++)
			line += "-";
		
		cout << "\n -------" << line;
		cout << "\n |   | ";
		int xInitNumbers = wherex();
		for(int i = 1; i <= GAME_WIDTH; i++)
		{
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! x inicial + index * espa�o por n�mero + espa�o por n�mero - 
			gotoxy(
				xInitNumbers + (i - 1) * (charSpace + 1) + (charSpace - 1) - ((charSpace - to_string(i).size()) / 2),
				wherey()
			);
			cout << i << " ";
		}
		cout << "|";
		cout << "\n ----+--" << line << " \n";
		
		const int
			absoluteX = xInitNumbers + initRowSpace.size(),
			absoluteY = wherey();
		
		// CAMPO
		for(int j = 0; j < GAME_HEIGHT; j++)
		{
			textcolor(WHITE);
			cout << " | " << char(j % 26 + 65) << " | " << initRowSpace; // <- 26 = n�mero de letras no alfabeto; 65 = c�digo ASCII do caractere 'A'
			
			for(int i = 0; i < GAME_WIDTH; i++)
			{
				//cout << "  ";
				//cout << initRowSpace;
				
				/*
				gotoxy(
					xInitNumbers + i * (charSpace + 1),
					wherey()
				);
				*/
				
				if(!field[i][j].isRevealed())
				{
					cout << "-";
				}
				else if(field[i][j].isWater())
				{
					textcolor(AGUA_COLOR);
					cout << AGUA_CHAR;
					
					textcolor(WHITE);
				}
				else
				{
					Tipo *t = field[i][j].getEmbarcacao()->getTipo();
					textcolor(t->getColor());
					cout << t->getCharacter();
					
					textcolor(WHITE);
				}
				
				cout << betweenPieces;
			}
			
			cout << "|\n";
		}
		
		cout << " -------" << line;
		
		// STATUS DE TIRO
		string left = "\n\t\t   ";
		cout << left;
		int xLeftStatusTiro = wherex() + 1;
		cout << "-----------------------------";
		int xMeioStatusTiro = (xLeftStatusTiro + (wherex() - 1)) / 2;
		string cleanStatusTiro = "                           ";
		cout << left << "|" << cleanStatusTiro << "|";
		int yStatusTiro = wherey();
		cout << left << "----------------------------- \n";
		
		// TABELA DE STATUS
		// Barra do topo
		left = "\n    ";
		cout << left << "=============================================================";
		int xRightStatusTable = wherex() - 1;
		cout << left << "| ";
		
		int xTamanho = wherex() + 22;
		
		// �gua
		textcolor(AGUA_COLOR);
		cout << AGUA_CHAR << " - �gua";
		
		// Calcular posi��o de "?/? afundados"
		string txtAfundados = " afundados ";
		int xAfundados = xRightStatusTable;
		int sizeMaiorQtd = 0;
		for(Tipo *tipo: tipos)
		{
			int sizeQtd = to_string(tipo->getQtd()).size();
			if(sizeQtd > sizeMaiorQtd)
				sizeMaiorQtd = sizeQtd;
				
			int newXAfudados = xRightStatusTable - (sizeQtd * 2 + 1 + txtAfundados.size());
			if(newXAfudados < xAfundados)
				xAfundados = newXAfudados;
		}
		
		int xInWater = xRightStatusTable - txtAfundados.size() - 1;
		int yInWater = wherey();
		
		gotoxy(
			xInWater - to_string(inWater).size() + 1,
			yInWater
		);
		cout << inWater << " tiros";
		
		// Fim da �gua
		textcolor(WHITE);
		gotoxy(xRightStatusTable, wherey());
		cout << "|";
		
		cout << left << "|-----------------------------------------------------------|";
		int yPrimeiroAfundados = wherey() + 1;
		
		// Tipos
		for(Tipo *tipo: tipos)
		{
			textcolor(WHITE);
			cout << left << "| ";
			
			textcolor(tipo->getColor());
			cout << tipo->getCharacter() << " - " << tipo->getName();
			
			gotoxy(xTamanho, wherey());
			cout << " (tamanho: " << tipo->getSize() << ")";
			
			int qtd = tipo->getQtdAfundados();
			gotoxy(
				xAfundados + (sizeMaiorQtd - to_string(qtd).size()),
				wherey()
			);
			cout << qtd << "/" << tipo->getQtd();
			gotoxy(xRightStatusTable - txtAfundados.size(), wherey());
			cout << txtAfundados;
			
			gotoxy(xRightStatusTable, wherey());
			textcolor(WHITE);
			cout << "|";
		}
		
		// Barra de baixo
		cout << left << "=============================================================";
		
		textcolor(DARKGRAY);
		cout << "\n\n\t\t\tAperte ESC para sair";
		
		// MOVIMENTO DO CURSOR
		
		bool movingCursor = true;
		while(movingCursor)
		{
			// Printar posi��o atual
			if(!revealing)
			{
				textcolor(LIGHTMAGENTA);
				gotoxy(
					absoluteX + x * (betweenPieces.size() + 1),
					absoluteY + y
				);
				cout << CROSSHAIR;
			}
			else
			{
				revealing = false;
				clearRelealed = true;
			}
			
			hideCursor();
			
			// Deslocamento (-1 || 0 || 1)
			int dX = 0;
			int dY = 0;
			
			int opt;
			switch(getch())
			{
				// Up
				case KEY_UP:
					dY = -1;
					break;
				// Down
				case KEY_DOWN:
					dY = 1;
					break;
				// Right
				case KEY_RIGHT:
					dX = 1;
					break;
				// Left
				case KEY_LEFT:
					dX = -1;
					break;
				case KEY_ESC:
					system("cls");
					cout << "\n\n\n\n\n\n";
					
					opt = menu(
						"Deseja mesmo sair?",
						{
							"N�o",
							"Sim"
						}, 0
					);
					
					movingCursor = false; // <- Exit loop
					
					if(opt == 1)
						return GameStatus::EXIT;
					
					break;
				// Enter
				case KEY_ENTER:
					if(!field[x][y].isRevealed())
					{
						revealing = true;
						
						// Atualizar n�mero de tiros
						tiros--;
						
						if(tiros == 0)
							return GameStatus::DEFEAT;
						
						textcolor(WHITE);
						gotoxy(xTiros, yTiros);
						cout << "   "; // <- Limpar
						gotoxy(xTiros, yTiros);
						cout << tiros; // <- Atualizar
						
						// Printar ponteiro de tiro (esquerdo)
						gotoxy(
							absoluteX + x * (betweenPieces.size() + 1) - 1,
							absoluteY + y
						);
						textcolor(LIGHTMAGENTA);
						cout << OPEN_SHOT_MARKER;
						
						// Revelar pe�a
						field[x][y].reveal();
						
						string status;
						COLORS statusColor;
						if(field[x][y].isWater())
						{
							inWater++;
							
							// Printar pe�a revelada
							textcolor(AGUA_COLOR);
							cout << AGUA_CHAR;
							
							// Atualiza status de tiro
							statusColor = AGUA_COLOR;
							status = "Tiro ao mar";
							
							// Atualizar tabela de status
							gotoxy(
								xInWater - to_string(inWater).size() + 1,
								yInWater
							);
							cout << inWater;
						}
						else
						{
							// Printar pe�a revelada
							Tipo *t = field[x][y].getEmbarcacao()->getTipo();
							textcolor(t->getColor());
							cout << t->getCharacter();
							
							// Atualiza status de tiro
							if(!field[x][y].getEmbarcacao()->isSunk())
							{
								statusColor = WHITE;
								status += "Embarca��o atingida";
							}
							else
							{
								// Remove ship from list
								for (list<Embarcacao*>::iterator it = shipsNotSunk.begin(); it != shipsNotSunk.end(); ++it)
									if(*it == field[x][y].getEmbarcacao())
									{
										shipsNotSunk.erase(it);
										break;
									}
									
								// Check if the game is over
								if(shipsNotSunk.empty())
									return GameStatus::VICTORY;
								
								// Shot status
								Tipo* tipo = field[x][y].getEmbarcacao()->getTipo();
								
								statusColor = tipo->getColor();
								status += tipo->getName() + " afundado";
								
								// Atualiza n�mero de atingidos
								tipo->incrementarQtdAfundados();
								
								// Atualizar tabela de status
								for(int iTipo = 0; iTipo < sizeof(tipos) / sizeof(*tipos); iTipo++)
									if(tipos[iTipo] == tipo)
									{
										int qtd = tipo->getQtdAfundados();
										gotoxy(
											xAfundados + (sizeMaiorQtd - to_string(qtd).size()),
											yPrimeiroAfundados + iTipo
										);
										cout << qtd;
										break;
									}
							}
						}
						
						// Printar ponteiro de tiro (direito)
						gotoxy(
							absoluteX + x * (betweenPieces.size() + 1) + 1,
							absoluteY + y
						);
						textcolor(LIGHTMAGENTA);
						cout << CLOSE_SHOT_MARKER;
						
						/*
						gotoxy(1, 1);
						textcolor(statusColor);
						cout << status;
						*/
						
						// Atualiza status de tiro
						textcolor(statusColor);
						gotoxy(xLeftStatusTiro, yStatusTiro);
						cout << cleanStatusTiro; // <- Limpar
						gotoxy(xMeioStatusTiro - status.size() / 2, yStatusTiro);
						cout << status; // <- Exibir
					}
					break;
			}
			
			// X em movimento
			if(!revealing)
			{
				// Limpar X da posi��o anterior
				if(!clearRelealed)
				{
					// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					gotoxy(
						absoluteX + x * (betweenPieces.size() + 1),
						absoluteY + y
					);
					
					if(!field[x][y].isRevealed())
					{
						textcolor(WHITE);
						cout << "-";
					}
					else
						if(field[x][y].isWater())
						{
							textcolor(AGUA_COLOR);
							cout << AGUA_CHAR;
						}
						else
						{
							Tipo *t = field[x][y].getEmbarcacao()->getTipo();
							textcolor(t->getColor());
							cout << t->getCharacter();
						}
				}
				// Limpas marcador de tiro ">" e "<" (caso estejam aparecendo (clearRelealed))
				else
				{
					clearRelealed = false;
					
					gotoxy(
						absoluteX + x * (betweenPieces.size() + 1) - 1,
						absoluteY + y
					);
					cout << " ";
					
					gotoxy(
						absoluteX + x * (betweenPieces.size() + 1) + 1,
						absoluteY + y
					);
					cout << " ";
				}
				
				// Atualizar posi��o
				x += dX;
				y += dY;
				
				// Ajustar caso saia da �rea da matriz
				if(x < 0)
					x = GAME_WIDTH - 1;
				else if(x > GAME_WIDTH - 1)
					x = 0;
					
				if(y < 0)
					y = GAME_HEIGHT - 1;
				else if(y > GAME_HEIGHT - 1)
					y = 0;
			}
		}
	}
}

void backToMenuText(int y)
{
	// Back text
	textcolor(LIGHTMAGENTA);
	string strs[] = {
		"Aperte ",
		"ENTER ",
		"para ir ao men� principal"
	};
	
	string str = "";
	for(string s: strs)
		str += s;
	int x = getPosToCenter(str);
	
	gotoxy(x, y);
	for(int i = 0; i < sizeof(strs) / sizeof(*strs); i++)
	{
		if(i % 2 == 0)
			textcolor(WHITE);
		else
			textcolor(LIGHTMAGENTA);
			
		cout << strs[i];
	}
	
	waitKey(KEY_ENTER);
}

// Victory
void victory()
{
	int ranking = 1;
	int score = 100;
	
	//int ranking = saveScore(string name, int score);
	
	cout << "\n\n\n";
	
	// Trophy
	string strs[] = {
		"   _____________     ",
		"  (_____________)    ",
		"___|           |___  ",
		"|  |           |  |  ",
		"|  | PARAB�NS! |  |  ",
		"|  |           |  |  ",
		" \\__\\         /__/ ",
		"     \\_______/      ",
		"       (___)         ",
		"        | |          ",
		"        |_|          ",
		"     __(___)__       ",
		"     |///////|       ",
		"     |_______|       "
	};
	
	textcolor(YELLOW);
	int x = getPosToCenter(strs[0]);
	for(string s: strs)
	{
		gotoxy(x, wherey() + 1);
		cout << s;
	}
	
	// Ranking text
	textcolor(LIGHTMAGENTA);
	string str = "Ranking";
	x = getPosToCenter(str);
	gotoxy(x, wherey() + 2);
	cout << str;
	
	// Ranking value
	textcolor(WHITE);
	str = to_string(ranking);
	x = getPosToCenter(str);
	gotoxy(x, wherey() + 1);
	cout << str;
	
	// Score text
	textcolor(LIGHTMAGENTA);
	str = "Score";
	x = getPosToCenter(str);
	gotoxy(x, wherey() + 2);
	cout << str;
	
	// Score value
	textcolor(WHITE);
	str = to_string(score);
	x = getPosToCenter(str);
	gotoxy(x, wherey() + 1);
	cout << str;
	
	// Player name text
	textcolor(LIGHTMAGENTA);
	str = "Nome de jogador";
	x = getPosToCenter(str);
	gotoxy(x, wherey() + 2);
	cout << str;
	
	// Player name value
	textcolor(WHITE);
	str =  "-----------";
	x = getPosToCenter(str);
	
	gotoxy(x, wherey() + 1);
	cout << str;
	
	gotoxy(x, wherey() + 1);
	cout << "| ";
	int xName = wherex();
	int yName = wherey();
	cout << ". . . . |";
	
	gotoxy(x, wherey() + 1);
	cout << "-----------";
	
	int y = wherey();
	
	// Player name input
	string nome = "";
	while(true)
	{
		hideCursor();
		
		int c = getch();
		
		if(((c >= 48 && c <= 57)   // <- Number
		||  (c >= 65 && c <= 90)   // <- Uppercase
		||  (c >= 97 && c <= 122)) // <- Lowercase
		&& nome.size() < 4) 
		{
			nome += (char) c;
			
			gotoxy(xName + (nome.size() - 1) * 2, yName);
			cout << (char) c;
		}
		// Delete
		else if(c == KEY_BACKSPACE && nome.size() > 0)
		{
			gotoxy(xName + (nome.size() - 1) * 2, yName);
			cout << ".";
			
			nome.pop_back();
		}
		// Continue
		else if(c == KEY_ENTER && nome.size() == 4)
		{
			break;
		}
	}
	
	backToMenuText(y + 4);
}

// Defeat
void defeat()
{
	cout << "\n\n\n";
	
	string strs[] = {
		" ____   _____  ____  ____  _____  ______  _____ ",
		"|  _ \\ |  ___||    \\|    \\|  _  ||__  __||     | ",
		"| | \\ ||  _|  |   _/|   _/| | | |  |  |  |  _  |   ",
		"| |_/ || |___ |   \\ |   \\ | |_| |  |  |  | | | |  ",
		"|____/ |_____||_|\\_\\|_|\\_\\|_____|  |__|  |_| |_|"
	};
	
	textcolor(LIGHTRED);
	int x = getPosToCenter(strs[0]);
	for(string s: strs)
	{
		gotoxy(x, wherey() + 1);
		cout << s;
	}
	
	backToMenuText(wherey() + 2);
}

// Instru��es
void instructions()
{
	textcolor(LIGHTMAGENTA);
	cout << "\n\n\t\t\t\t   INSTRU��ES \n\n\n";
	
	string margin = "\t   ";
	textcolor(WHITE);
	cout << margin << "Utilize as setas do teclado ou as teclas ";
	textcolor(LIGHTMAGENTA);
	cout << "W";
	textcolor(WHITE);
	cout << ", ";
	textcolor(LIGHTMAGENTA);
	cout << "A";
	textcolor(WHITE);
	cout << ", ";
	textcolor(LIGHTMAGENTA);
	cout << "S";
	textcolor(WHITE);
	cout << " e ";
	textcolor(LIGHTMAGENTA);
	cout << "D";
	textcolor(WHITE);
	cout << " para \n";
	cout << margin << "controlar o ";
	textcolor(LIGHTMAGENTA);
	cout << "X";
	textcolor(WHITE);
	cout << " no campo da Batalha Naval. \n\n";
	
	cout << margin << "Aperte ";
	textcolor(LIGHTMAGENTA);
	cout << "Enter";
	textcolor(WHITE);
	cout << " quando quiser atirar no ponto marcado pelo ";
	textcolor(LIGHTMAGENTA);
	cout << "X";
	textcolor(WHITE);
	cout << ". \n\n";
	
	cout << margin << "O jogo come�a com um total de tiros disponiveis definidos \n";
	cout << margin << "pela dificuldade escolhida pelo jogador. A cada tiro dado \n";
	cout << margin << "em uma coordenada diferente, sua quantidade de tiros dis- \n";
	cout << margin << "pon�veis diminui. \n\n";
	
	cout << margin << "O jogador vence quando todos os barcos e navios s�o afun- \n";
	cout << margin << "dados por  completo antes  que seus  tiros acabem e perde \n";
	cout << margin << "quando esta condi��o n�o � atendida. \n\n";
	
	cout << margin << "O ganhador tem sua posi��o no ranking relativa ao modo de \n";
	cout << margin << "jogo escolhido no in�cio e � quantidade gasta de tiros.";
	
	textcolor(LIGHTMAGENTA);
	cout << "\n\n\n\n\t\t\t ...";
	textcolor(WHITE);
	cout << " APERTE ENTER PARA SAIR ";
	textcolor(LIGHTMAGENTA);
	cout << "... \n";
	
	hideCursor();
	waitKey(KEY_ENTER);
}

// Ranking
void ranking()
{
	// Read score file
	string line;
	ifstream outFile("score.txt");
	if(outFile.is_open())
	{
		while(getline(outFile, line))
		{
			cout << line << '\n';
		}
		
		outFile.close();
	}
}





// MAIN
int main()
{
	// CONFIGS
	
	// Set charset
	setlocale(LC_ALL, ""); // <- Habilita caracterer especiais
	
	// Set window width
	const basic_string<char> modeBS = "mode " + to_string(WINDOW_WIDTH) + ", 85";
	const char *mode = modeBS.c_str();
	system(mode);

	// Possibilitar gera��o de n�meros pseudo-rand�micos diferentes a cada execu��o
	srand((unsigned) time(0));
	
	// Variables
	int opt = 0;
	
	/*
	float
		A3  = 220,
		Bb3 = 233.1,
		
		C4  = 261.6,
		D4  = 293.7,
		E4  = 329.6,
		F4  = 349.2,
		A4  = 440,
		Bb4 = 466.2,
		
		C5 = 523.3,
		D5 = 587.3,
		E5 = 659.3,
		F5 = 698.5;
	
	Beep(C4, 300);
	Beep(F4, 1200);
	Beep(E4, 300);
	Beep(F4, 300);
	Beep(E4, 800);
	Beep(C4, 800);
	Beep(A3, 800);
	Beep(D4, 800);
	Beep(A3, 800);
	Beep(Bb3, 800);
	Beep(C4, 800);
	*/
	
	// Main loop
	while(true)
	{
		system("cls");
	
		// MAIN MEN�
		string margin = "\t\t ";
		textcolor(LIGHTGRAY);
		cout << "\n\n\n";
		cout << margin << " ____  ____  ______  ____  _     _    _  ____   \n";
		cout << margin << "|�   ||/   ||__  __||�   ||/|   | |__| ||   �|  \n";
		cout << margin << "| __/ |/__ |  |  |  | __ ||/|   |� __ �|| __ |  \n";
		cout << margin << "|   \\ |/|| |  |  |  | || ||/|__ | |  | || || | \n";
		cout << margin << "|�___||/||_|  |__|  |_||_||/___||_|  |_||_||_|  \n";
		
		textcolor(LIGHTBLUE);
		cout << margin << "//////////////////////////////////////////////  \n";
		
		margin = "\t\t\t";
		textcolor(LIGHTGRAY);
		cout << margin << " ___  _	 ____  _  _  ____  _      \n";
		cout << margin << "|�  \\| ||   �||/|| ||�   || |	  \n";
		cout << margin << "|�|\\   || __ ||/|| || __ || |	  \n";
		cout << margin << "|�| \\  || || ||/\\/ || || || |__  \n";
		cout << margin << "|�|  \\_||_||�| \\__/ |_||_||____| \n";
		
		textcolor(LIGHTBLUE);
		cout << margin << "////////////////////////////////   \n";
		
		margin = "\t\t\t  ";
		textcolor(WHITE);
		cout << "\n\n\n\n";
		
		opt = menu(
			"MENU PRINCIPAL",
			{
				"NOVO JOGO",
				"INSTRU��ES DO JOGO",
				"RANKING",
				"SAIR"
			}, opt
		);
		
		system("cls");
		
		// MENU REDIRECT
		switch(opt)
		{
			// New game
			case 0:
				switch(game())
				{
					case GameStatus::VICTORY:
						victory();
						break;
						
					case GameStatus::DEFEAT:
						defeat();
						break;
				}
				break;
			// Instructions
			case 1:
				instructions();
				break;
			// Ranking
			case 2:
				ranking();
				break;
			// Exit
			case 3:
				return 0;
				break;
		}
	}
	
	return 0;
}
