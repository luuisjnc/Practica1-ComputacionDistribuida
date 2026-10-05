#ifndef NODO_H
#define NODO_H

#include "mensaje.h"

#include <stddef.h>

typedef struct Nodo Nodo;
typedef struct Simulador Simulador;

typedef void (*ManejadorMensaje)(Nodo *nodo,
                                 const Mensaje *mensaje,
                                 Simulador *simulador);
typedef void (*DestructorEstado)(void *estado);

struct Nodo {
    int id;
    int *vecinos;
    size_t cantidad_vecinos;
    ManejadorMensaje al_recibir;
    void *estado;
    DestructorEstado destruir_estado;
};

Nodo *nodo_crear(int id,
                 const int *vecinos,
                 size_t cantidad_vecinos,
                 ManejadorMensaje al_recibir,
                 void *estado,
                 DestructorEstado destruir_estado);
void nodo_destruir(Nodo *nodo);

#endif
