using namespace std;

// CLASSES

class Embarcacao;

// Coordenada
class Coordenada
{
	private:
		int x, y;
		
	public:
		Coordenada() {}
		
		Coordenada(int x, int y)
		{
			this->x = x;
			this->y = y;
		}
		
		int getX()
		{
			return x;
		}
		
		void setX(int x)
		{
			this->x = x;
		}
		
		int getY()
		{
			return y;
		}
		
		void setY(int y)
		{
			this->y = y;
		}
};

// Tipo
class Tipo
{
	private:
		string name;
		int qtd;
		int size;
		char character;
		COLORS color;
		
		int qtdAfundados = 0;
		
	public:
		Tipo(string name, int qtd, int size, char character, COLORS color)
		{
			this->name = name;
			this->qtd = qtd;
			this->size = size;
			this->character = character;
			this->color = color;
		}
		
		string getName()
		{
			return name;
		}
		
		int getQtd()
		{
			return qtd;
		}
		
		int getSize()
		{
			return size;
		}
		
		char getCharacter()
		{
			return character;
		}
		
		COLORS getColor()
		{
			return color;
		}
		
		int getQtdAfundados()
		{
			return qtdAfundados;
		}
		
		void incrementarQtdAfundados()
		{
			qtdAfundados++;
		}
};

// Peça
class Peca
{
	private:
		Embarcacao *embarcacao = nullptr;
		bool revealed = false;
	public:
		Embarcacao* getEmbarcacao()
		{
			return embarcacao;
		}
		
		void setEmbarcacao(Embarcacao *embarcacao)
		{
			this->embarcacao = embarcacao;
		}
		
		bool isRevealed()
		{
			return revealed;
		}
		
		void reveal()
		{
			revealed = true;
		}
		
		bool isWater()
		{
			return embarcacao == nullptr;
		}
};

// Embarcação
class Embarcacao
{
	private:
		Tipo *tipo;
		list<Peca*> pecas;
	public:
		Embarcacao(Tipo *tipo)
		{
			this->tipo = tipo;
		}
		
		Tipo *getTipo()
		{
			return tipo;
		}
		
		list<Peca*> getPecas()
		{
			return pecas;
		}
		
		void setPecas(list<Peca*> pecas)
		{		
			this->pecas = pecas;
		}
		
		// Retorna 'true' se todas as peças foram reveladas
		bool isSunk()
		{
			for(Peca *p: pecas)
				if(!p->isRevealed())
					return false;
					
			return true;
		}
};

// Patent
class Patent
{
	private:
		string name;
		COLORS color;
		int min = 0;
		int max = -1;
	public:
		static const int infinite = -1;
		
		Patent(string name, COLORS color, int min, int max)
		{
			this->name = name;
			this->color = color;
			this->min = min;
			this->max = max;
		}
		
		string getName()
		{
			return name;
		}
		
		COLORS getColor()
		{
			return color;
		}
		
		int getMin()
		{
			return min;
		}
		
		int getMax()
		{
			return max;
		}
		
		bool isMaxInfinite()
		{
			return max == infinite;
		}
		
		bool isInRange(int score)
		{
			return score >= min && (score <= max || isMaxInfinite());
		}
};

// Player
class Player
{
	private:
		string name;
		int score;
		Patent *patent;
	public:
		Player(string name, int score, Patent *patent)
		{
			this->name = name;
			this->score = score;
			this->patent = patent;
		}
		
		string getName()
		{
			return name;
		}
		
		int getScore()
		{
			return score;
		}
		
		Patent* getPatent()
		{
			return patent;
		}
};
