#ifndef VECINOS_H
#define VECINOS_H

#include "conjunto.h"
#include "nodo.h"
#include "simulador.h"

#include <stdbool.h>
#include <stddef.h>

#define TICK_VECINOS 1U

Nodo *nodo_vecinos_crear(int id,
                         const int *vecinos,
                         size_t cantidad_vecinos);
bool nodo_vecinos_iniciar(Nodo *nodo, Simulador *simulador);
const ConjuntoInt *nodo_vecinos_identificadores(const Nodo *nodo);

#endif
