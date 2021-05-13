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

// FILES

#include "classes.cpp"

// CONSTANTS

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_RIGHT 77
#define KEY_LEFT 75
#define KEY_ENTER 13
#define KEY_ESC 27

#define GAME_WIDTH 20
#define GAME_HEIGHT 15

using namespace std;





// VARIÁVEIS GLOBAIS

// Marcadores
char const
	CROSSHAIR 		  = 'x', // <- Mira
	OPEN_SHOT_MARKER  = '>', // <- 
	CLOSE_SHOT_MARKER = '<'; // <- 

// Água
char const AGUA_CHAR = '#';
COLORS const AGUA_COLOR = LIGHTBLUE;

// Tipos pré-definidos
Tipo *tipos[] = {
	new Tipo("Submarino"   , 3, 1, '1', LIGHTCYAN),
	new Tipo("Encouraçado" , 2, 2, '2', GREEN    ),
	new Tipo("Cruzador"    , 3, 3, '3', DARKGRAY ),
	new Tipo("Hidroavião"  , 4, 1, '4', LIGHTRED ),
	new Tipo("Porta-aviões", 1, 4, '5', YELLOW   )
};

// Matriz do jogo
Peca field[GAME_WIDTH][GAME_HEIGHT];

// Embarcações não afundadas
list<Embarcacao*> shipsNotSunk;





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

// Interrompe o sistema até que a key passada seja pressionada
void waitKey(int key)
{
	char input;
	
	do
	{
		input = getch();
	}
	while(input != key);
}

// Gerar embarcação
void gerarEmbarcacoes(Tipo *tipoAtual)
{
	for(int qtd = 0; qtd < tipoAtual->getQtd(); qtd++)
	{
		// Posicionar embarcação
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
			
			// Direções disponíveis
			list<int> directions = { 0, 1, 2, 3 };
			
			// Direcionar embarcação
			bool directing = true;
			while(directing)
			{
				// Se não existirem mais direções disponíveis, volta e pega novas coordenadas
				if(directions.empty())
					break;
				
				// Sorteia uma direção
				int iDir = rand() % directions.size();
				
				// Pega a direção sorteada
				list<int>::iterator dir = directions.begin();
				advance(dir, iDir);
				
				// Apaga a direção
				directions.erase(dir);
				
				// Salvar coordenadas (caso válidas)
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
				
				// Sair dos loops e passar para próxima embarcação
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
					shipsNotSunk.push_back(e);
					
					directing = false;
					sortingCoord = false;
				}
			}
		}
	}
}





// INTERFACE FUNCTIONS

// Jogo
GameStatus game()
{
	// VARIÁVEIS
	int tiros = 500;
	
	// GERAR EMBARCAÇÕES
	for(Tipo *tipo: tipos)
		gerarEmbarcacoes(tipo);
	
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
	
	// CAMPO
	cout << "\n -------------------------------------------------------------------";
	cout << "\n |   |  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 |";
	cout << "\n ----+-------------------------------------------------------------- \n";
	
	const int
		absoluteX = 9,
		absoluteY = wherey();
	
	textcolor(WHITE);
	for(int j = 0; j < GAME_HEIGHT; j++)
	{
		cout << " | " << char(j + 65) << " |";
		
		for(int i = 0; i < GAME_WIDTH; i++)
			cout << "  -";
		
		cout << " |\n";
	}
	
	cout << " -------------------------------------------------------------------";
	
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
	
	// Água
	textcolor(AGUA_COLOR);
	cout << AGUA_CHAR << " - Água";
	
	// Calcular posição de "?/? afundados"
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
	
	// Fim da água
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
		cout << " (tamanho " << tipo->getSize() << ")";
		
		gotoxy(xAfundados + sizeMaiorQtd - 1, wherey());
		cout << "0/" << tipo->getQtd();
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
	int x = 0;
	int y = 0;
	bool revealing = false;
	bool clearRelealed = false;
	
	while(true)
	{
		// Printar posição atual
		if(!revealing)
		{
			//cout << "y: " << absoluteY;
			
			textcolor(LIGHTMAGENTA);
			gotoxy(
				absoluteX + x * 3,
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
				return GameStatus::EXIT;
				break;
			// Enter
			case KEY_ENTER:
				if(!field[x][y].isRevealed())
				{
					revealing = true;
					
					// Atualizar número de tiros
					tiros--;
					
					if(tiros == 0)
						return GameStatus::DEFEAT;
					
					textcolor(WHITE);
					gotoxy(xTiros, yTiros);
					cout << "   "; // <- Limpar
					gotoxy(xTiros, yTiros);
					cout << tiros; // <- Atualizar
					
					// Atualizar campo
					gotoxy(
						absoluteX + x * 3 - 1,
						absoluteY + y
					);
					
					textcolor(LIGHTMAGENTA);
					cout << OPEN_SHOT_MARKER;
					
					// Revelar peça
					field[x][y].reveal();
					
					// Printar peça revelada
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
					
					textcolor(LIGHTMAGENTA);
					cout << CLOSE_SHOT_MARKER;
					
					// Atualiza status de tiro
					string status;
					if(field[x][y].isWater())
					{
						textcolor(AGUA_COLOR);
						status = "Tiro ao mar";
					}
					else
					{
						if(!field[x][y].getEmbarcacao()->isSunk())
						{
							textcolor(WHITE);
							status += "Embarcação atingida";
						}
						else
						{
							// Remove sink from list
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
							
							textcolor(tipo->getColor());
							status += tipo->getName() + " afundado";
							
							// Atualiza número de atingidos
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
			// Limpar X da posição anterior
			if(!clearRelealed)
			{
				gotoxy(
					absoluteX + x * 3,
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
					absoluteX + x * 3 - 1,
					absoluteY + y
				);
				cout << " ";
				
				gotoxy(
					absoluteX + x * 3 + 1,
					absoluteY + y
				);
				cout << " ";
			}
			
			// Atualizar posição
			x += dX;
			y += dY;
			
			// Ajustar caso saia da área da matriz
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

// Troféu
void victory()
{
	system("cls");
	
	string left = "\t\t\t  ";
	cout << "\n\n\n";
	
	textcolor(YELLOW);
	cout << left << "   _____________     \n";
	cout << left << "  (_____________)    \n";
	cout << left << "___|           |___  \n";
	cout << left << "|  |           |  |  \n";
	cout << left << "|  | PARABÉNS! |  |  \n";
	cout << left << "|  |           |  |  \n";
	cout << left << " \\__\\         /__/ \n";
	cout << left << "     \\_______/      \n";
	cout << left << "       (___)         \n";
	cout << left << "        | |          \n";
	cout << left << "        |_|          \n";
	cout << left << "     __(___)__       \n";
	cout << left << "     |///////|       \n";
	cout << left << "     |_______|       \n";
	
	waitKey(KEY_ENTER);
}

// Instruções
void instructions()
{
	textcolor(LIGHTMAGENTA);
	cout << "\n\n\t\t\t\t   INSTRUÇÕES \n\n\n";
	
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
	
	cout << margin << "O jogo começa com um total de tiros disponiveis definidos \n";
	cout << margin << "pela dificuldade escolhida pelo jogador. A cada tiro dado \n";
	cout << margin << "em uma coordenada diferente, sua quantidade de tiros dis- \n";
	cout << margin << "poníveis diminui. \n\n";
	
	cout << margin << "O jogador vence quando todos os barcos e navios são afun- \n";
	cout << margin << "dados por  completo antes  que seus  tiros acabem e perde \n";
	cout << margin << "quando esta condição não é atendida. \n\n";
	
	cout << margin << "O ganhador tem sua posição no ranking relativa ao modo de \n";
	cout << margin << "jogo escolhido no início e à quantidade gasta de tiros.";
	
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
	
}





// MAIN
int main()
{
	// CONFIGS
	setlocale(LC_ALL, ""); // <- Habilita caracterer especiais
	
	system("mode 80, 85"); // <- Tamanho da tela
	
	srand((unsigned) time(0)); // <- Possibilita geração de números pseudo-randômicos diferentes a cada execução
	
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
	
		// MAIN MENÚ
		string margin = "\t\t ";
		textcolor(LIGHTGRAY);
		cout << "\n\n\n";
		cout << margin << " ____  ____  ______  ____  _     _    _  ____   \n";
		cout << margin << "|°   ||/   ||__  __||°   ||/|   | |__| ||   °|  \n";
		cout << margin << "| __/ |/__ |  |  |  | __ ||/|   |° __ °|| __ |  \n";
		cout << margin << "|   \\ |/|| |  |  |  | || ||/|__ | |  | || || | \n";
		cout << margin << "|°___||/||_|  |__|  |_||_||/___||_|  |_||_||_|  \n";
		
		textcolor(LIGHTBLUE);
		cout << margin << "//////////////////////////////////////////////  \n";
		
		margin = "\t\t\t";
		textcolor(LIGHTGRAY);
		cout << margin << " ___  _	 ____  _  _  ____  _      \n";
		cout << margin << "|°  \\| ||   °||/|| ||°   || |	  \n";
		cout << margin << "|°|\\   || __ ||/|| || __ || |	  \n";
		cout << margin << "|°| \\  || || ||/\\/ || || || |__  \n";
		cout << margin << "|°|  \\_||_||°| \\__/ |_||_||____| \n";
		
		textcolor(LIGHTBLUE);
		cout << margin << "////////////////////////////////   \n";
		
		margin = "\t\t\t  ";
		textcolor(WHITE);
		cout << "\n\n\n\n";
		cout << margin << "----------------------------- \n";
		cout << margin << "|      ";
		textcolor(LIGHTMAGENTA);
		cout << "MENU PRINCIPAL";
		textcolor(WHITE);
		cout << "       | \n";
		cout << margin << "----------------------------- \n";
		cout << margin << "|   NOVO JOGO               | \n";
		cout << margin << "|   INSTRUÇÕES DO JOGO      | \n";
		cout << margin << "|   RANKING                 | \n";
		cout << margin << "|   SAIR                    | \n";
		cout << margin << "----------------------------- \n";
		
		// MENU OPTIONS
		bool choosing = true;
		
		const int initX = 29;
		const int initY = 23;
		
		textcolor(LIGHTMAGENTA);
		
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
						cout << "\n DERROTA";
						waitKey(KEY_ENTER);
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
