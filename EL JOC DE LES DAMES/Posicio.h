#include <iostream>
#include <string>

using namespace std;

class Posicio
{
public:
	Posicio() : m_fila(0), m_columna(0), m_posicio("") {}
	Posicio(const string& posicio) : m_fila(0), m_columna(0), m_posicio(posicio) {}

	string toString();

	string getPosicio() const { return m_posicio; }
	int getFila() const { return m_fila; }
	int getColumna() const { return m_columna; }

	void setPosicio(const string& posicio) { m_posicio = posicio; }
	void setFila(int fila) { m_fila = fila; }
	void setColumna(int columna) { m_columna = columna; }

	bool operator==(const Posicio& posicio) const;
private:
	string m_posicio;
	int m_fila;
	int m_columna;
};

