#include <conio2.h>  // <- Use gotoxy and colors

using namespace std;

// CLASSES

class Coordenada;
class Tipo;
class Embarcacao;
class Peca;

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
};

// Embarcação
class Embarcacao
{
	private:
		Tipo *tipo;
		Peca **pecas; // <- Array of pointers
	public:
		Embarcacao(Tipo *tipo)
		{
			this->tipo = tipo;
		}
		
		Tipo* getTipo()
		{
			return tipo;
		}
		
		Peca** getPecas()
		{
			return pecas;
		}
		
		void setPecas(Peca **pecas)
		{
			this->pecas = pecas;
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
