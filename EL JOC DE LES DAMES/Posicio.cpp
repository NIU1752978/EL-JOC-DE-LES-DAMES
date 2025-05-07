#include "Posicio.h"

Posicio::Posicio(const string& posicio) : m_posicio(posicio) // he cambiat el constructor, possible solucio al problema de que no apraeixen ben representades les fitxes al tauler
{
    if (posicio.length() >= 2) // longitud cadena sigui correcta
    {
        char lletraColumna = posicio[0];
        int numeroFila = posicio[1] - '0';

		m_columna = lletraColumna - 'a'; // exemple b - a = 1
		m_fila = 8 - numeroFila;    
    }
    else
    {
        m_fila = -1;
		m_columna = -1; // no valida    
    }
}

bool Posicio::posicioValida() const // comprovem si esta dins tauler
{
	return (m_fila >= 0 && m_fila < 8 && m_columna >= 0 && m_columna < 8);
}

bool Posicio::operator==(const Posicio& posicio) const // comprovem si dues posicions son iguals
{
	bool igual = false;

	if (m_fila == posicio.m_fila && m_columna == posicio.m_columna)
	{
		igual = true;
	}

	return igual;
}

ostream& operator<<(ostream& out, const Posicio& pos) // sobrecarguem l'operador << per mostrar la posicio
{
    if (pos.posicioValida())
    {
		char lletraColumna = 'a' + pos.getColumna();  // exemple a + 1 = b
        int numeroFila = 8 - pos.getFila();
        out << lletraColumna << numeroFila;
    }
    else
    {
        out << "Posicio no valida";
    }
    return out;
}