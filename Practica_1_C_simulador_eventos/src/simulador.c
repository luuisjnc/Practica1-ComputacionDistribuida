#include "simulador.h"

#include <limits.h>
#include <stdlib.h>

struct Evento {
    uint64_t tiempo;
    uint64_t orden;
    int destino;
    Mensaje mensaje;
    Evento *siguiente;
};

bool simulador_inicializar(Simulador *simulador) {
    if (simulador == NULL) {
        return false;
    }
    simulador->tiempo_actual = 0;
    simulador->siguiente_orden = 0;
    simulador->mensajes_programados = 0;
    simulador->mensajes_entregados = 0;
    simulador->nodos = NULL;
    simulador->capacidad_nodos = 0;
    simulador->eventos = NULL;
    simulador->error_memoria = false;
    return true;
}

static bool asegurar_capacidad_nodos(Simulador *simulador, size_t necesaria) {
    if (necesaria <= simulador->capacidad_nodos) {
        return true;
    }

    size_t nueva = simulador->capacidad_nodos == 0 ? 8 : simulador->capacidad_nodos;
    while (nueva < necesaria) {
        if (nueva > SIZE_MAX / 2) {
            return false;
        }
        nueva *= 2;
    }

    Nodo **nodos = realloc(simulador->nodos, nueva * sizeof(*nodos));
    if (nodos == NULL) {
        return false;
    }
    for (size_t i = simulador->capacidad_nodos; i < nueva; ++i) {
        nodos[i] = NULL;
    }
    simulador->nodos = nodos;
    simulador->capacidad_nodos = nueva;
    return true;
}

bool simulador_registrar_nodo(Simulador *simulador, Nodo *nodo) {
    if (simulador == NULL || nodo == NULL || nodo->id < 0) {
        return false;
    }
    size_t indice = (size_t)nodo->id;
    if (!asegurar_capacidad_nodos(simulador, indice + 1)) {
        simulador->error_memoria = true;
        return false;
    }
    if (simulador->nodos[indice] != NULL) {
        return false;
    }
    simulador->nodos[indice] = nodo;
    return true;
}

static void insertar_evento(Simulador *simulador, Evento *nuevo) {
    Evento **actual = &simulador->eventos;
    while (*actual != NULL &&
           ((*actual)->tiempo < nuevo->tiempo ||
            ((*actual)->tiempo == nuevo->tiempo &&
             (*actual)->orden < nuevo->orden))) {
        actual = &(*actual)->siguiente;
    }
    nuevo->siguiente = *actual;
    *actual = nuevo;
}

bool simulador_enviar(Simulador *simulador,
                      int destino,
                      const Mensaje *mensaje,
                      uint64_t demora) {
    if (simulador == NULL || mensaje == NULL || destino < 0 ||
        (size_t)destino >= simulador->capacidad_nodos ||
        simulador->nodos[destino] == NULL ||
        UINT64_MAX - simulador->tiempo_actual < demora) {
        return false;
    }

    Evento *evento = calloc(1, sizeof(*evento));
    if (evento == NULL) {
        simulador->error_memoria = true;
        return false;
    }
    if (!mensaje_copiar(&evento->mensaje, mensaje)) {
        free(evento);
        simulador->error_memoria = true;
        return false;
    }

    evento->tiempo = simulador->tiempo_actual + demora;
    evento->orden = simulador->siguiente_orden++;
    evento->destino = destino;
    insertar_evento(simulador, evento);
    ++simulador->mensajes_programados;
    return true;
}

bool simulador_enviar_a_vecinos(Simulador *simulador,
                                const Nodo *nodo,
                                const Mensaje *mensaje,
                                uint64_t demora,
                                int vecino_excluido) {
    bool correcto = true;
    for (size_t i = 0; i < nodo->cantidad_vecinos; ++i) {
        int vecino = nodo->vecinos[i];
        if (vecino != vecino_excluido &&
            !simulador_enviar(simulador, vecino, mensaje, demora)) {
            correcto = false;
        }
    }
    return correcto;
}

bool simulador_ejecutar_hasta(Simulador *simulador, uint64_t limite_tiempo) {
    if (simulador == NULL) {
        return false;
    }

    while (simulador->eventos != NULL &&
           simulador->eventos->tiempo <= limite_tiempo) {
        Evento *evento = simulador->eventos;
        simulador->eventos = evento->siguiente;
        simulador->tiempo_actual = evento->tiempo;

        Nodo *destino = simulador->nodos[evento->destino];
        if (destino != NULL && destino->al_recibir != NULL) {
            destino->al_recibir(destino, &evento->mensaje, simulador);
            ++simulador->mensajes_entregados;
        }

        mensaje_destruir(&evento->mensaje);
        free(evento);

        if (simulador->error_memoria) {
            return false;
        }
    }
    return !simulador->error_memoria;
}

bool simulador_tiene_eventos(const Simulador *simulador) {
    return simulador != NULL && simulador->eventos != NULL;
}

void simulador_destruir(Simulador *simulador) {
    if (simulador == NULL) {
        return;
    }
    while (simulador->eventos != NULL) {
        Evento *evento = simulador->eventos;
        simulador->eventos = evento->siguiente;
        mensaje_destruir(&evento->mensaje);
        free(evento);
    }
    free(simulador->nodos);
    simulador->nodos = NULL;
    simulador->capacidad_nodos = 0;
}
