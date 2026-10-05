#include "broadcast.h"

#include "mensaje.h"

#include <stdlib.h>

typedef struct {
    int nodo_fuente;
    int mensaje;
    bool visto;
} EstadoBroadcast;

static void destruir_estado_broadcast(void *estado) {
    free(estado);
}

static void recibir_broadcast(Nodo *nodo,
                              const Mensaje *mensaje,
                              Simulador *simulador) {
    if (nodo == NULL || nodo->estado == NULL || mensaje == NULL ||
        simulador == NULL || mensaje->tipo != MSG_BROADCAST) {
        return;
    }

    EstadoBroadcast *estado = nodo->estado;
    if (estado->visto) {
        return;
    }

    estado->mensaje = mensaje->valor;
    estado->visto = true;

    Mensaje reenvio = *mensaje;
    reenvio.origen = nodo->id;
    (void)simulador_enviar_a_vecinos(simulador, nodo, &reenvio,
                                    TICK_BROADCAST, -1);
}

Nodo *nodo_broadcast_crear(int id,
                           const int *vecinos,
                           size_t cantidad_vecinos,
                           int nodo_fuente,
                           int mensaje_inicial) {
    EstadoBroadcast *estado = calloc(1, sizeof(*estado));
    if (estado == NULL) {
        return NULL;
    }
    estado->nodo_fuente = nodo_fuente;
    estado->mensaje = mensaje_inicial;
    estado->visto = false;

    Nodo *nodo = nodo_crear(id,
                            vecinos,
                            cantidad_vecinos,
                            recibir_broadcast,
                            estado,
                            destruir_estado_broadcast);
    if (nodo == NULL) {
        destruir_estado_broadcast(estado);
    }
    return nodo;
}

bool nodo_broadcast_iniciar(Nodo *nodo, Simulador *simulador) {
    if (nodo == NULL || nodo->estado == NULL || simulador == NULL) {
        return false;
    }

    EstadoBroadcast *estado = nodo->estado;
    if (nodo->id != estado->nodo_fuente || estado->visto) {
        return true;
    }

    Mensaje mensaje;
    if (!mensaje_crear(&mensaje, MSG_BROADCAST, nodo->id,
                       estado->nodo_fuente, estado->mensaje, NULL, 0)) {
        simulador->error_memoria = true;
        return false;
    }

    estado->visto = true;
    bool correcto = simulador_enviar_a_vecinos(simulador, nodo, &mensaje,
                                               TICK_BROADCAST, -1);
    mensaje_destruir(&mensaje);
    return correcto;
}

bool nodo_broadcast_vio_mensaje(const Nodo *nodo) {
    if (nodo == NULL || nodo->estado == NULL) {
        return false;
    }
    const EstadoBroadcast *estado = nodo->estado;
    return estado->visto;
}

int nodo_broadcast_mensaje(const Nodo *nodo) {
    if (nodo == NULL || nodo->estado == NULL) {
        return 0;
    }
    const EstadoBroadcast *estado = nodo->estado;
    return estado->mensaje;
}
