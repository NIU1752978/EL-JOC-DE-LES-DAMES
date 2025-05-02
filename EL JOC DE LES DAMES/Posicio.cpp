#include "Posicio.h"

bool Posicio::posicioValida() const
{
	return (m_fila >= 0 && m_fila < 8 && m_columna >= 0 && m_columna < 8);
}


bool Posicio::operator==(const Posicio& posicio) const
{
	bool igual = false;

	if (m_fila == posicio.m_fila && m_columna == posicio.m_columna)
	{
		igual = true;
	}

	return igual;
}

ostream& operator<<(ostream& out, const Posicio& pos)
{
    if (pos.posicioValida())
    {
        char lletraColumna = 'a' + pos.getColumna();
        int numeroFila = 8 - pos.getFila();
        out << lletraColumna << numeroFila;
    }
    else
    {
        out << "Posicio no valida";
    }
    return out;
}