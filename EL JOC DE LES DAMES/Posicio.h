#ifndef POSICIO_H
#define POSICIO_H

#include <iostream>
#include <string>

using namespace std;

class Posicio
{
public:
	Posicio() : m_fila(-1), m_columna(-1), m_posicio("") {}
	Posicio(int fila, int columna) : m_fila(fila), m_columna(columna){}
	Posicio(const string& posicio) : m_fila(-1), m_columna(-1), m_posicio(posicio) {}

	string toString();
	bool posicioValida() const;

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
ostream& operator<<(ostream& out, const Posicio& pos);

#endif

//entrada i sortida, metode que  controli si esta dins del tauler