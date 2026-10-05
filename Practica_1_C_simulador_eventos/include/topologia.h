#ifndef TOPOLOGIA_H
#define TOPOLOGIA_H

#include "nodo.h"
#include "simulador.h"

#include <stdbool.h>
#include <stddef.h>

#define TICK_TOPOLOGIA 1U

Nodo *nodo_topologia_crear(int id,
                           const int *vecinos,
                           size_t cantidad_vecinos,
                           size_t cantidad_nodos);
bool nodo_topologia_iniciar(Nodo *nodo, Simulador *simulador);
bool nodo_topologia_conoce_proceso(const Nodo *nodo, int proceso);
bool nodo_topologia_conoce_canal(const Nodo *nodo, int origen, int destino);
bool nodo_topologia_esta_completa(const Nodo *nodo);

#endif
