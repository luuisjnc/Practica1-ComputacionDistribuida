#include "conjunto.h"

#include <stdlib.h>

void conjunto_inicializar(ConjuntoInt *conjunto) {
    conjunto->elementos = NULL;
    conjunto->cantidad = 0;
    conjunto->capacidad = 0;
}

bool conjunto_contiene(const ConjuntoInt *conjunto, int elemento) {
    for (size_t i = 0; i < conjunto->cantidad; ++i) {
        if (conjunto->elementos[i] == elemento) {
            return true;
        }
    }
    return false;
}

bool conjunto_agregar(ConjuntoInt *conjunto, int elemento) {
    if (conjunto_contiene(conjunto, elemento)) {
        return true;
    }

    if (conjunto->cantidad == conjunto->capacidad) {
        size_t nueva_capacidad = conjunto->capacidad == 0 ? 4 : conjunto->capacidad * 2;
        int *nuevos = realloc(conjunto->elementos,
                              nueva_capacidad * sizeof(*nuevos));
        if (nuevos == NULL) {
            return false;
        }
        conjunto->elementos = nuevos;
        conjunto->capacidad = nueva_capacidad;
    }

    conjunto->elementos[conjunto->cantidad++] = elemento;
    return true;
}

void conjunto_destruir(ConjuntoInt *conjunto) {
    free(conjunto->elementos);
    conjunto->elementos = NULL;
    conjunto->cantidad = 0;
    conjunto->capacidad = 0;
}
