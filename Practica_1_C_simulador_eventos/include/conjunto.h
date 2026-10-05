#ifndef CONJUNTO_H
#define CONJUNTO_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int *elementos;
    size_t cantidad;
    size_t capacidad;
} ConjuntoInt;

void conjunto_inicializar(ConjuntoInt *conjunto);
bool conjunto_agregar(ConjuntoInt *conjunto, int elemento);
bool conjunto_contiene(const ConjuntoInt *conjunto, int elemento);
void conjunto_destruir(ConjuntoInt *conjunto);

#endif
