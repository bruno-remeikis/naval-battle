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

// Stops the system until the key passed is pressed
void waitKey(int key)
{
	char input;
	
	do
	{
		input = getch();
	}
	while(input != key);
}

// Returns the x position needed to center a string
int getPosToCenter(int strSize)
{
	return WINDOW_WIDTH / 2 - strSize / 2;
}

// Generate ship type in the field
template <size_t rows, size_t cols>
list<Embarcacao*> gerarEmbarcacoes(Peca (&field)[rows][cols], Tipo *tipoAtual)
{
	list<Embarcacao*> ships;
	
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
					ships.push_back(e);
					
					directing = false;
					sortingCoord = false;
				}
			}
		}
	}
	
	return ships;
}

// Returns the positon in the ranking
int saveScore(string name, int score)
{
	// Write score file
	ofstream inFile;
	inFile.open("score.txt");
	inFile << name << " - " << score;
	inFile.close();
	
	return 0;
}





// VISUAL FUNCTIONS

// Mover the cursor to [1; 1]
void hideCursor()
{
	gotoxy(1, 1);
}

int menu(string title, vector<string> options, int opt)
{
	// CONSTRUIR MENÚ
	int biggerTxt = title.size();
	
	for(string option: options)
		if(option.size() + 4 > biggerTxt)
			biggerTxt = option.size() + 4;
			
	string border = "----";
	for(int i = 0; i < biggerTxt; i++)
		border += "-";
	border += "\n";
	
	int x = getPosToCenter(border.size());
	
	// Top border
	gotoxy(x, wherey());
	textcolor(WHITE);
	cout << border;
	
	// Title
	gotoxy(x, wherey());
	cout << "|";
	const int initX = wherex() + 1;
	const int initY = wherey() + 2;
	
	gotoxy(x + border.size() / 2 - title.size() / 2, wherey()); // <- Centralizar título
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

// Show a centralized visual element (single line)
void printCentralized(string str, int y)
{
	int x = getPosToCenter(str.size());
	gotoxy(x, y);
	cout << str;
}

// Show a centralized visual element (multiple lines)
void printCentralized(vector<string> strs, int y)
{
	if(!strs.empty())
	{
		int x = getPosToCenter(strs.at(0).size());
		
		for(int i = 0; i < strs.size(); i++)
		{
			gotoxy(x, y + i);
			cout << strs.at(i);
		}
	}
}

// Receive a broken text and print it, alternately coloring it
void printAlternatingColors(vector<string> strs, vector<COLORS> colors)
{
	for(int i = 0; i < strs.size(); i++)
	{
		textcolor(colors[i % colors.size()]);
		cout << strs.at(i);
	}
}

// Returns the size of a broken text
// considers that the strings have no spaces between their parts
int getBrokenTextSize(vector<string> strs)
{
	int size = 0;
	for(string s: strs)
		size += s.size();
		
	return size;
}

