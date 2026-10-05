#include "vecinos.h"

#include "mensaje.h"

#include <stdlib.h>

typedef struct {
    ConjuntoInt identificadores;
} EstadoVecinos;

static void destruir_estado_vecinos(void *estado_generico) {
    EstadoVecinos *estado = estado_generico;
    if (estado == NULL) {
        return;
    }
    conjunto_destruir(&estado->identificadores);
    free(estado);
}

static void recibir_myname(Nodo *nodo,
                           const Mensaje *mensaje,
                           Simulador *simulador) {
    if (nodo == NULL || nodo->estado == NULL || mensaje == NULL ||
        simulador == NULL || mensaje->tipo != MSG_MYNAME) {
        return;
    }

    EstadoVecinos *estado = nodo->estado;
    for (size_t i = 0; i < mensaje->longitud; ++i) {
        if (!conjunto_agregar(&estado->identificadores, mensaje->datos[i])) {
            simulador->error_memoria = true;
            return;
        }
    }
}

Nodo *nodo_vecinos_crear(int id,
                         const int *vecinos,
                         size_t cantidad_vecinos) {
    EstadoVecinos *estado = malloc(sizeof(*estado));
    if (estado == NULL) {
        return NULL;
    }
    conjunto_inicializar(&estado->identificadores);

    Nodo *nodo = nodo_crear(id,
                            vecinos,
                            cantidad_vecinos,
                            recibir_myname,
                            estado,
                            destruir_estado_vecinos);
    if (nodo == NULL) {
        destruir_estado_vecinos(estado);
    }
    return nodo;
}

bool nodo_vecinos_iniciar(Nodo *nodo, Simulador *simulador) {
    if (nodo == NULL || nodo->estado == NULL || simulador == NULL) {
        return false;
    }

    Mensaje mensaje;
    if (!mensaje_crear(&mensaje, MSG_MYNAME, nodo->id, nodo->id, 0,
                       nodo->vecinos, nodo->cantidad_vecinos)) {
        simulador->error_memoria = true;
        return false;
    }

    bool correcto = simulador_enviar_a_vecinos(simulador, nodo, &mensaje,
                                               TICK_VECINOS, -1);
    mensaje_destruir(&mensaje);
    return correcto;
}

const ConjuntoInt *nodo_vecinos_identificadores(const Nodo *nodo) {
    if (nodo == NULL || nodo->estado == NULL) {
        return NULL;
    }
    const EstadoVecinos *estado = nodo->estado;
    return &estado->identificadores;
}
