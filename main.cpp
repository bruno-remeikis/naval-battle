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

// FILES

#include "classes.cpp"

// CONSTANTS

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_RIGHT 77
#define KEY_LEFT 75
#define KEY_ENTER 13

#define GAME_WIDTH 20
#define GAME_HEIGHT 15

using namespace std;





// VARI�VEIS GLOBAIS

// Tipos pr�-definidos
Tipo *tipos[] = {
	new Tipo("Submarino"   , 3, 1, '1', LIGHTCYAN),
	new Tipo("Encoura�ado" , 2, 2, '2', GREEN    ),
	new Tipo("Cruzador"    , 3, 3, '3', DARKGRAY ),
	new Tipo("Hidroavi�o"  , 4, 1, '4', LIGHTRED ),
	new Tipo("Porta-avi�es", 1, 4, '5', YELLOW   )
};

// Matriz do jogo
Peca field[GAME_WIDTH][GAME_HEIGHT];





// FUNCTIONS

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

// Gerar embarca��o
void gerarEmbarcacoes(Tipo *tipoAtual)
{
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
					Embarcacao *e = new Embarcacao(tipoAtual);
					Peca *pecas[tipoAtual->getSize()];
					
					for(int iC = 0; iC < sizeof(coords) / sizeof(*coords); iC++)
					{
						int i = coords[iC].getX();
						int j = coords[iC].getY();
						
						field[i][j].setEmbarcacao(e);
						
						pecas[iC] = &field[i][j];
					}
					
					e->setPecas(pecas);
					
					directing = false;
					sortingCoord = false;
				}
			}
		}
	}
}





// INTERFACE FUNCTIONS

// Jogo
void game()
{
	int tiros = 100;
	
	// Gerar embarca��es
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
				textcolor(LIGHTBLUE);
				cout << " 0";
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
	
	// PRINTAR
	textcolor(WHITE);
	
	// Tiros restantes
	cout << "\n ------------------------";
	cout << "\n | Tiros restantes: ";
	
	const int
		xTiros = wherex(),
		yTiros = wherey();
	
	cout << tiros;
	gotoxy(xTiros + 3, yTiros);
	cout << " |\n ------------------------";
	
	// Campo
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
	
	// Status do tiro
	string left = "\n\t\t   ";
	cout << left << "----------------------------";
	cout << left << "|                          |";
	cout << left << "---------------------------- \n";
	
	// Tabela de status
	left = "\n    ";
	cout << left << "=============================================================";
	int xRightStatusTable = wherex() - 1;
	cout << left << "| ";
	
	int xTamanho = wherex() + 22;
	
	textcolor(LIGHTBLUE);
	cout << "0 - �gua";
	textcolor(WHITE);
	gotoxy(xRightStatusTable, wherey());
	cout << "|";
	
	cout << left << "|-----------------------------------------------------------|";
	
	for(Tipo *tipo: tipos)
	{
		textcolor(WHITE);
		cout << left << "| ";
		
		textcolor(tipo->getColor());
		cout << tipo->getCharacter() << " - " << tipo->getName();
		gotoxy(xTamanho, wherey());
		cout << " (tamanho " << tipo->getSize() << ")";
		gotoxy(xRightStatusTable, wherey());
		
		textcolor(WHITE);
		cout << "|";
	}
	
	cout << left << "=============================================================";
	
	// MOVIMENTO DO CURSOR
	int x = 0;
	int y = 0;
	bool revealing = false;
	bool clearRelealed = false;
	
	while(true)
	{
		// Printar posi��o atual
		if(!revealing)
		{
			textcolor(LIGHTMAGENTA);
			gotoxy(
				absoluteX + x * 3,
				absoluteY + y
			);
			cout << "X";
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
			// Enter
			case KEY_ENTER:
				if(!field[x][y].isRevealed())
				{
					revealing = true;
					
					// Atualizar n�mero de tiros
					tiros--;
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
					cout << ">";
					
					// Revelar pe�a
					field[x][y].reveal();
					
					// Printar pe�a revelada
					if(field[x][y].isWater())
					{
						textcolor(LIGHTBLUE);
						cout << "0";
					}
					else
					{
						Tipo *t = field[x][y].getEmbarcacao()->getTipo();
						textcolor(t->getColor());
						cout << t->getCharacter();
					}
					
					textcolor(LIGHTMAGENTA);
					cout << "<";
				}
				break;
		}
		
		// X em movimento
		if(!revealing)
		{
			// Limpar X da posi��o anterior
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
						textcolor(LIGHTBLUE);
						cout << "0";
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
	
}





// MAIN
int main()
{
	// CONFIGS
	setlocale(LC_ALL, ""); // <- Habilita caracterer especiais
	
	system("mode 80, 85"); // <- Tamanho da tela
	
	srand((unsigned) time(0)); // <- Possibilita gera��o de n�meros pseudo-rand�micos diferentes a cada execu��o
	
	// Variables
	int opt = 0;
	
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
		cout << margin << "----------------------------- \n";
		cout << margin << "|      ";
		textcolor(LIGHTMAGENTA);
		cout << "MENU PRINCIPAL";
		textcolor(WHITE);
		cout << "       | \n";
		cout << margin << "----------------------------- \n";
		cout << margin << "|   NOVO JOGO               | \n";
		cout << margin << "|   INSTRU��ES DO JOGO      | \n";
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
				game();
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
