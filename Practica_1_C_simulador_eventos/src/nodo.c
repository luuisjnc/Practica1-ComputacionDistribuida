#include "nodo.h"

#include <stdlib.h>
#include <string.h>

Nodo *nodo_crear(int id,
                 const int *vecinos,
                 size_t cantidad_vecinos,
                 ManejadorMensaje al_recibir,
                 void *estado,
                 DestructorEstado destruir_estado) {
    Nodo *nodo = calloc(1, sizeof(*nodo));
    if (nodo == NULL) {
        return NULL;
    }

    if (cantidad_vecinos > 0) {
        if (vecinos == NULL) {
            free(nodo);
            return NULL;
        }
        nodo->vecinos = malloc(cantidad_vecinos * sizeof(*nodo->vecinos));
        if (nodo->vecinos == NULL) {
            free(nodo);
            return NULL;
        }
        memcpy(nodo->vecinos,
               vecinos,
               cantidad_vecinos * sizeof(*vecinos));
    }

    nodo->id = id;
    nodo->cantidad_vecinos = cantidad_vecinos;
    nodo->al_recibir = al_recibir;
    nodo->estado = estado;
    nodo->destruir_estado = destruir_estado;
    return nodo;
}

void nodo_destruir(Nodo *nodo) {
    if (nodo == NULL) {
        return;
    }
    if (nodo->destruir_estado != NULL) {
        nodo->destruir_estado(nodo->estado);
    }
    free(nodo->vecinos);
    free(nodo);
}
