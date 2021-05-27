// LIBRARIES

#include <iostream>  // <- Input and output
#include <locale>    // <- Change language
#include <windows.h> // <- Redimention window
#include <conio2.h>  // <- Use gotoxy and colors
#include <cstdlib>   // <- Use random
#include <array>	 // <- Use array
#include <list>		 // <- Use list
#include <vector>	 // <- Use vector
#include <iterator>  // <- Manipulate list
#include <ctime>	 // <- Use time in random numbers
#include <windows.h>
#include <fstream> 	 // <- Write and read files

// CONSTANTS

#define WINDOW_WIDTH 100

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_RIGHT 77
#define KEY_LEFT 75
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_BACKSPACE 8

#define GAME_WIDTH 20
#define GAME_HEIGHT 15

// FILES

#include "classes.cpp"
#include "utils.cpp"

using namespace std;





// VARIÁVEIS GLOBAIS

// Marcadores
char const
	CROSSHAIR 		  = 'x', // <- Mira
	OPEN_SHOT_MARKER  = '>', // <- Esquerda do marcador de tiro
	CLOSE_SHOT_MARKER = '<'; // <- Direita do marcador de tiro

// Água
char const AGUA_CHAR = '#';
COLORS const AGUA_COLOR = LIGHTBLUE;

// Tipos pré-definidos
Tipo *tipos[] = {
	new Tipo("Submarino"   , 3, 1, '1', LIGHTCYAN),
	new Tipo("Encouraçado" , 2, 2, '2', GREEN    ),
	new Tipo("Cruzador"    , 3, 3, '3', DARKGRAY ),
	new Tipo("Hidroavião"  , 4, 1, '4', LIGHTRED ),
	new Tipo("Porta-aviões", 1, 4, '5', YELLOW   ),
};

// Patentes pré-definidas
Patent *patents[] = {
	new Patent("Marinheiro", YELLOW   , 0  , 88  ),
	new Patent("Tenente"   , GREEN    , 89 , 152 ),
	new Patent("Capitão"   , LIGHTBLUE, 153, 216 ),
	new Patent("Almirante" , LIGHTRED , 217, 280 ),
	new Patent("Pirata"    , MAGENTA  , 281, Patent::infinite),
};





// INTERFACE FUNCTIONS

// Victory
void victory(int score)
{
	int ranking = getRanking(score);
	
	system("cls");
	
	// Trophy
	textcolor(YELLOW);
	printCentralized({
		"   _____________   ",
		"  (_____________)    ",
		"___|           |___  ",
		"|  |           |  |  ",
		"|  | PARABÉNS! |  |  ",
		"|  |           |  |  ",
		" \\__\\         /__/ ",
		"     \\_______/      ",
		"       (___)         ",
		"        | |          ",
		"        |_|          ",
		"     __(___)__       ",
		"     |///////|       ",
		"     |_______|       "
	}, 4);
	
	// Ranking text
	textcolor(LIGHTMAGENTA);
	printCentralized(
		"Ranking",
		wherey() + 2
	);
	
	// Ranking value
	textcolor(WHITE);
	printCentralized(
		to_string(ranking),
		wherey() + 1
	);
	
	// Score text
	textcolor(LIGHTMAGENTA);
	printCentralized(
		"Score",
		wherey() + 2
	);
	
	// Score value
	textcolor(WHITE);
	printCentralized(
		to_string(score),
		wherey() + 1
	);
	
	// Player name text
	textcolor(LIGHTMAGENTA);
	printCentralized(
		"Nome de jogador",
		wherey() + 2
	);
	
	// Player name value
	textcolor(WHITE);
	string str =  "-----------";
	int x = getPosToCenter(str.size());
	
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
	string name = "";
	while(true)
	{
		hideCursor();
		
		int c = getch();
		
		if(((c >= 48 && c <= 57)   // <- Number
		||  (c >= 65 && c <= 90)   // <- Uppercase
		||  (c >= 97 && c <= 122)) // <- Lowercase
		&& name.size() < 4) 
		{
			name += (char) c;
			
			gotoxy(xName + (name.size() - 1) * 2, yName);
			cout << (char) c;
		}
		// Delete
		else if(c == KEY_BACKSPACE && name.size() > 0)
		{
			gotoxy(xName + (name.size() - 1) * 2, yName);
			cout << ".";
			
			name.pop_back();
		}
		// Continue
		else if(c == KEY_ENTER && name.size() == 4)
		{
			break;
		}
	}
	
	// Save score to file
	cout << "Saving score...";
	saveScore(name, score, ranking);
	cout << "...End";
	
	// Back
	printCentralizedAndAlternatingColors(
		{"Aperte ", "ENTER ", "para ir ao menú principal"},
		{WHITE, LIGHTMAGENTA},
		y + 4
	);
	hideCursor();
	waitKey(KEY_ENTER);
}

// Defeat
void defeat()
{
	system("cls");
	
	textcolor(LIGHTRED);
	printCentralized({
		" ____   _____  ____  ____  _____  ______  _____ ",
		"|  _ \\ |  ___||    \\|    \\|  _  ||__  __||     | ",
		"| | \\ ||  _|  |   _/|   _/| | | |  |  |  |  _  |   ",
		"| |_/ || |___ |   \\ |   \\ | |_| |  |  |  | | | |  ",
		"|____/ |_____||_|\\_\\|_|\\_\\|_____|  |__|  |_| |_|"
	}, 4);
	
	// Back
	printCentralizedAndAlternatingColors(
		{"Aperte ", "ENTER ", "Para ir ao menú principal"},
		{WHITE, LIGHTMAGENTA},
		wherey() + 4
	);
	hideCursor();
	waitKey(KEY_ENTER);
}

// Jogo
void game()
{
	// Matriz do jogo
	Peca field[GAME_WIDTH][GAME_HEIGHT];
	
	// Embarcações não afundadas
	list<Embarcacao*> shipsNotSunk;

	// Tiros
	int tiros = 500;
	int inWater = 0;
	
	// GERAR EMBARCAÇÕES
	for(Tipo *tipo: tipos)
		shipsNotSunk.merge(
			gerarEmbarcacoes(field, tipo)
		);
	
	// Variáveis para movimento do cursor
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
		
		// Espaço destinado a cada coordenada
		int charSpace = to_string(GAME_WIDTH).size();
		
		// Espaço a ser colocado a cada linha antes da primeira peça
		string initRowSpace = "";
		for(int i2 = 0; i2 < charSpace / 2; i2++)
			initRowSpace += " ";
			
		// Espaço entre as peças
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
			cout << " | " << char(j % 26 + 65) << " | " << initRowSpace; // <- 26 = número de letras no alfabeto; 65 = código ASCII do caractere 'A'
			
			for(int i = 0; i < GAME_WIDTH; i++)
			{
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
		
		int xInWater = xRightStatusTable - txtAfundados.size() - 1;
		int yInWater = wherey();
		
		gotoxy(
			xInWater - to_string(inWater).size() + 1,
			yInWater
		);
		cout << inWater << " tiros";
		
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
			// Printar posição atual
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
							"Não",
							"Sim"
						}, 0
					);
					
					movingCursor = false; // <- Exit loop
					
					if(opt == 1)
						return;
					
					break;
				// Enter
				case KEY_ENTER:
					if(!field[x][y].isRevealed())
					{
						revealing = true;
						
						// Atualizar número de tiros
						tiros--;
						
						if(tiros == 0)
						{
							defeat();
							return;
						}
						
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
						
						// Revelar peça
						field[x][y].reveal();
						
						string status;
						COLORS statusColor;
						if(field[x][y].isWater())
						{
							inWater++;
							
							// Printar peça revelada
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
							// Printar peça revelada
							Tipo *t = field[x][y].getEmbarcacao()->getTipo();
							textcolor(t->getColor());
							cout << t->getCharacter();
							
							// Atualiza status de tiro
							if(!field[x][y].getEmbarcacao()->isSunk())
							{
								statusColor = WHITE;
								status += "Embarcação atingida";
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
								{
									victory(200);
									return;
								}
								
								// Shot status
								Tipo* tipo = field[x][y].getEmbarcacao()->getTipo();
								
								statusColor = tipo->getColor();
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
						
						// Printar ponteiro de tiro (direito)
						gotoxy(
							absoluteX + x * (betweenPieces.size() + 1) + 1,
							absoluteY + y
						);
						textcolor(LIGHTMAGENTA);
						cout << CLOSE_SHOT_MARKER;
						
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
				// Limpar X da posição anterior
				if(!clearRelealed)
				{
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
}

/*
void backToMenuText(int y)
{
	// Back text
	textcolor(LIGHTMAGENTA);
	vector<string> strs = {
		"Aperte ",
		"ENTER ",
		"para ir ao menú principal"
	};
	
	string str = "";
	for(string s: strs)
		str += s;
	int x = getPosToCenter(str.size());
	
	gotoxy(x, y);
	printAlternatingColors(strs, {WHITE, LIGHTMAGENTA});

	
	waitKey(KEY_ENTER);
}
*/

// Instruções
void instructions()
{
	textcolor(LIGHTMAGENTA);
	printCentralized("INSTRUÇÕES", 4);
	
	vector<COLORS> colors = {WHITE, LIGHTMAGENTA};
	
	int x = printCentralizedAndAlternatingColors({
		{
			"Utilize as setas do teclado ou as teclas ",
			"W", ", ", "A", ", ", "S ", "e ", "D ",
			"para"
		},
		{
			"controlar o ", "X ", "no campo da Batalha Naval."
		}
	}, colors, wherey() + 2);
	
	gotoxy(x, wherey() + 1);
	printAlternatingColors({"controlar o ", "X ", "no campo da Batalha Naval."}, colors);
	
	gotoxy(x, wherey() + 2);
	printAlternatingColors({"Aperte ", "Enter ", "quando quiser atirar no ponto marcado pelo ", "X", "."}, colors);
	
	printCentralized({
		"O jogo começa com um total de tiros disponiveis definidos",
		"pela dificuldade escolhida pelo jogador. A cada tiro dado",
		"em uma coordenada diferente, sua quantidade de tiros dis-",
		"poníveis diminui.",
		"",
		"O jogador vence quando todos os barcos e navios são afun-",
		"dados por  completo antes  que seus  tiros acabem e perde",
		"quando esta condição não é atendida.",
		"",
		"O ganhador tem sua posição no ranking relativa ao modo de",
		"jogo escolhido no início e à quantidade gasta de tiros."
	}, wherey() + 2);
	
	// Back
	printCentralizedAndAlternatingColors(
		{"Aperte ", "ENTER ", "para retornar ao menú principal"},
		{WHITE, LIGHTMAGENTA},
		wherey() + 4
	);
	hideCursor();
	waitKey(KEY_ENTER);
}

// Ranking Informations
void rankingInfo()
{
	system("cls");
	
	textcolor(LIGHTMAGENTA);
	printCentralized("Patentes", 4);
	
	textcolor(WHITE);
	printCentralized({
		"As patentes  são classificações  de honra",
		"dadas aos pertencentes do Ranking. A cor,",
		"cujo seu  nome apresenta, classifica-o em",
		"uma patente, sendo:\n"
	}, wherey() + 2);
	
	string
		divisor = " - ";
		
	int
		nameSize  = 0,
		scoreSize = 0;
	
	for(Patent *p: patents)
	{
		if(p->getName().size() > nameSize)
			nameSize = p->getName().size();
		
		int thisScoreSize = to_string(p->getMin()).size() + (p->getMax() != -1 ? to_string(p->getMax()).size() + strlen(" a  pontos") : strlen("+ pontos"));
		if(thisScoreSize > scoreSize)
			scoreSize = thisScoreSize;
	}
	
	int x = getPosToCenter(nameSize + scoreSize + divisor.size());
	
	for(Patent *p: patents)
	{
		textcolor(p->getColor());
		
		gotoxy(x, wherey() + 1);
		cout << p->getName();
		
		gotoxy(x + nameSize, wherey());
		
		textcolor(WHITE);
		cout << divisor;
		
		textcolor(p->getColor());
		cout << p->getMin();
		if(p->getMax() != -1)
			cout << " a " << p->getMax();
		else
			cout << "+";
		cout << " pontos";
	}
	
	printCentralizedAndAlternatingColors(
		{"Aperte ", "ENTER ", "para retornar ao ranking"},
		{WHITE, LIGHTMAGENTA},
		wherey() + 3
	);
	hideCursor();
	waitKey(KEY_ENTER);
}

// Ranking
void ranking()
{
	system("cls");
	
	// Read score file
	string line;
	int i = 1;
	
	vector<Player*> players;
	
	ifstream outFile("score.txt");
	if(outFile.is_open())
	{
		while(getline(outFile, line))
		{
			int divisor = line.find("-");
			
			string name = line.substr(0, divisor - 1);
			int score = stoi(line.substr(divisor + 1));
			
			for(Patent *p: patents)
				if(p->isInRange(score))
				{
					players.push_back(new Player(name, score, p));
					break;
				}
		}
		
		outFile.close();
	}
	
	// Loop to get out of 'rankingInfo()'
	while(true)
	{
		system("cls");
	
		textcolor(LIGHTRED);
		printCentralized({
			"-----------",
			"| RANKING | ",
			"-----------"
		}, 4);
		
		printCentralized({
			"////////////////////////////////////",
			"------------------------------------",
			""
		}, wherey() + 2);
		
		int x;
		for(int i = 0; i < players.size() && i < 3; i++)
		{
			Player *p = players.at(i);
			
			string txt = to_string(i + 1) + "° - " + p->getName() + " - " + to_string(p->getScore());
			if(i == 0)
				x = getPosToCenter(txt.size());
			
			gotoxy(x, wherey() + 1);
			textcolor(p->getPatent()->getColor());
			cout << txt;
		}
		
		textcolor(WHITE);
		for(int i = players.size() + 1; i <= 3; i++)
		{
			string txt = to_string(i) + "° - .... - ?";
			if(i == 0)
				x = getPosToCenter(txt.size());
				
			gotoxy(x, wherey() + 1);
			cout << txt;
		}
		
		textcolor(LIGHTRED);
		printCentralized({
			"------------------------------------",
			"////////////////////////////////////",
			""
		}, wherey() + 2);
		
		// Exit or show more informations
		printCentralizedAndAlternatingColors({
			{"Aperte ", "ENTER ", "para voltar ao ranking"},
			{"Aperte ", "ESC   ", "para mais informações"}
		}, {WHITE, LIGHTMAGENTA}, wherey() + 2);
		
		hideCursor();
		int key = waitKeys({KEY_ENTER, KEY_ESC});
	
		if(key == KEY_ESC)
			rankingInfo();
		else
			return;
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

	// Possibilitar geração de números pseudo-randômicos diferentes a cada execução
	srand((unsigned) time(0));
	
	// Variables
	int opt = 0;
	
	// Main loop
	while(true)
	{
		system("cls");
	
		// TITLE
		
		textcolor(LIGHTGRAY);
		printCentralized({
			" ____  ____  ______  ____  _     _    _  ____ ",
			"|°   ||/   ||__  __||°   ||/|   | |__| ||   °|  ",
			"| __/ |/__ |  |  |  | __ ||/|   |° __ °|| __ |  ",
			"|   \\ |/|| |  |  |  | || ||/|__ | |  | || || | ",
			"|°___||/||_|  |__|  |_||_||/___||_|  |_||_||_|  "
		}, wherey() + 5);
		
		textcolor(LIGHTBLUE);
		printCentralized(
			"//////////////////////////////////////////////",
			wherey() + 1
		);
		
		textcolor(LIGHTGRAY);
		printCentralized({
			" ___  _  ____  _  _  ____  _     ",
			"|°  \\| ||   °||/|| ||°   || |	    ",
			"|°|\\   || __ ||/|| || __ || |	    ",
			"|°| \\  || || ||/\\/ || || || |__  ",
			"|°|  \\_||_||°| \\__/ |_||_||____| "
		}, wherey() + 1);
		
		textcolor(LIGHTBLUE);
		printCentralized(
			"////////////////////////////////",
			wherey() + 1
		);
		
		// MENÚ
		
		textcolor(WHITE);
		cout << "\n\n\n\n";
		
		opt = menu(
			"MENU PRINCIPAL",
			{
				"NOVO JOGO",
				"INSTRUÇÕES DO JOGO",
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
