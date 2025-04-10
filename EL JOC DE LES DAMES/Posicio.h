#include <iostream>
#include <string>

using namespace std;

class Posicio
{
public:
	Posicio();
	Posicio(int fila, int columna, const string& posicio);

	bool operator==(const Posicio& posicio) const;
private:
	string posicio;
	int fila;
	int columna;
};

