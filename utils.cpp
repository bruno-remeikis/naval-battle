// FUNCTIONS

// Stops the system until the passed key is pressed
void waitKey(int key)
{
	char input;
	
	do
	{
		input = getch();
	}
	while(input != key);
}

// Stops the system until one of the passed keys is pressed
int waitKeys(vector<int> keys)
{
	char input;
	
	while(true)
	{
		input = getch();
		
		for(int key: keys)
			if(input == key)
				return key;
	}
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
					Embarcacao *e = new Embarcacao(tipoAtual);
					list<Peca*> pecas;
					
					for(int iC = 0; iC < sizeof(coords) / sizeof(*coords); iC++)
					{
						int i = coords[iC].getX();
						int j = coords[iC].getY();
						
						field[i][j].setEmbarcacao(e);
						
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

void savePlayers(vector<Player*> players)
{
	// Write in score file
	ofstream inFile(SCORE_FILE_NAME);
	
	for(Player *p: players)
		inFile << p->getName() << " - " << p->getScore() << "\n";
		
	inFile.close();
}

// Returns the score cut from a line
int getScore(string line)
{
	// In `name - score` format, finds the '-', add 1 to remove the ' ', cuts the string to get just the score and convert it to integer
	return stoi(line.substr(line.find('-') + 1));
}

Patent* getPatent(int score)
{
	for(Patent *p: patents)
		if(p->isInRange(score))
			return p;
		
	return nullptr;
}

vector<Player*> getPlayersInRanking()
{
	vector<Player*> players;
	
	// Read score file
	string line;
	int i = 1;
	
	ifstream outFile(SCORE_FILE_NAME);
	if(outFile.is_open())
	{
		while(getline(outFile, line))
		{
			int divisor = line.find("-");
			
			string name = line.substr(0, divisor - 1);
			int score = stoi(line.substr(divisor + 1));
			
			players.push_back(new Player(name, score, i, getPatent(score)));
		}
		
		outFile.close();
	}
	
	return players;
}

// remove player if his name already exists in the ranking
Player* removePlayer(string name, int score)
{
	vector<Player*> players = getPlayersInRanking();
	
	for(int i = 0; i < players.size(); i++)
	{
		Player* p = players.at(i);
		
		if(p->getName() == name)
		{
			if(p->getScore() < score)
			{
				players.erase(players.begin() + i); // <- Remove player from vector
				savePlayers(players); // <- Update file content
			}
			
			return new Player(p->getScore(), i + 1);
		}
	}
	
	return nullptr;
}

// Saves the player score in the document
// Returns the rankin of the current player
int saveScore(string name, int score)
{
	vector<Player*> players = getPlayersInRanking();
	
	Player *newP = new Player(name, score);
	int ranking = 1;
	
	bool inserted = false;
	bool nameFounded = false;
	for(int i = 0; i < players.size(); i++)
	{
		Player *p = players.at(i);
		
		if(p->getName() == name)
		{
			nameFounded = true;
		}
		else if(score > p->getScore() && !inserted && !nameFounded)
		{
			vector<Player*>::iterator it = (players.begin() + i);
			players.insert(it, newP);
			
			inserted = true;
		}
		else
			ranking++;
	}
	
	if(players.empty() || (!inserted && !nameFounded))
	{
		players.push_back(newP);
		inserted = true;
	}
	
	if(inserted)
		savePlayers(players);
		
	return ranking;
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

// Returns the size of a broken text
// Considers that the strings have no spaces between their parts
int getBrokenTextSize(vector<string> strs)
{
	int size = 0;
	for(string s: strs)
		size += s.size();
		
	return size;
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

// Show a centralized visual element (single line)
// Returns the x used to centralize the text
int printCentralized(string str, int y)
{
	int x = getPosToCenter(str.size());
	gotoxy(x, y);
	cout << str;
	
	return x;
}

// Show a centralized visual element (multiple lines)
// Uses the size of the first item in the vector as a base
// Returns the x used to centralize the text or -1 if the vector is empty
int printCentralized(vector<string> strs, int y)
{
	int x = -1;
	
	if(!strs.empty())
	{
		x = getPosToCenter(strs.at(0).size());
		
		for(int i = 0; i < strs.size(); i++)
		{
			gotoxy(x, y + i);
			cout << strs.at(i);
		}
	}
	
	return x;
}

// Show a centralized visual element (single lines), alternately coloring it
// Uses the size of the first item in the vector as a base
// Returns the x used to centralize the text or -1 if the vector is empty
int printCentralizedAndAlternatingColors(vector<string> strs, vector<COLORS> colors, int y)
{
	int x = -1;
	
	if(!strs.empty())
	{
		x = getPosToCenter(getBrokenTextSize(strs));
		
		gotoxy(x, y);
		printAlternatingColors(strs, colors);
	}
	
	return x;
}

// Show a centralized visual element (multiple lines), alternately coloring it
// Uses the size of the first item in the vector as a base
// Returns the x used to centralize the text or -1 if the vector is empty
int printCentralizedAndAlternatingColors(vector<vector<string>> strs, vector<COLORS> colors, int y)
{
	int x = -1;
	
	if(!strs.empty())
	{
		x = getPosToCenter(getBrokenTextSize(strs.at(0)));
		
		for(int i = 0; i < strs.size(); i++)
		{
			gotoxy(x, y + i);
			printAlternatingColors(strs.at(i), colors);
		}
	}
	
	return x;
}

// Show a name input box
// Returns a player name with NAME_SIZE size or an empty string if the function is canceled
string getName()
{
	// Player name text
	textcolor(LIGHTMAGENTA);
	printCentralized(
		"Nome de jogador",
		wherey() + 2
	);
	
	string line = "---";
	for(int i = 0; i < NAME_SIZE; i++)
		line += "--";
	
	// Player name value
	textcolor(WHITE);
	int x = getPosToCenter(line.size());
	
	gotoxy(x, wherey() + 1);
	cout << line;
	
	gotoxy(x, wherey() + 1);
	cout << "| ";
	int xName = wherex();
	int yName = wherey();
	for(int i = 0; i < NAME_SIZE; i++)
		cout << ". ";
	cout << "|";
	
	gotoxy(x, wherey() + 1);
	cout << line;
	
	// Player name input
	string name = "";
	while(true)
	{
		hideCursor();
		
		int c = getch();
		
		if(((c >= 48 && c <= 57)   // <- Number
		||  (c >= 65 && c <= 90)   // <- Uppercase
		||  (c >= 97 && c <= 122)) // <- Lowercase
		&& name.size() < NAME_SIZE) 
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
		else if(c == KEY_ENTER && name.size() == NAME_SIZE)
		{
			return name;
		}
		else if(c == KEY_ESC)
		{
			return "";
		}
	}
}
