#include "Posicio.h"

bool Posicio::posicioValida() const
{
	return false;
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
	return out;
}
