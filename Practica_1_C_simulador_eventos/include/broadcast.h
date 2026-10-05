#ifndef BROADCAST_H
#define BROADCAST_H

#include "nodo.h"
#include "simulador.h"

#include <stdbool.h>
#include <stddef.h>

#define TICK_BROADCAST 1U

Nodo *nodo_broadcast_crear(int id,
                           const int *vecinos,
                           size_t cantidad_vecinos,
                           int nodo_fuente,
                           int mensaje_inicial);
bool nodo_broadcast_iniciar(Nodo *nodo, Simulador *simulador);
bool nodo_broadcast_vio_mensaje(const Nodo *nodo);
int nodo_broadcast_mensaje(const Nodo *nodo);

#endif
