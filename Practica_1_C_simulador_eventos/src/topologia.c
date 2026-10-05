#include "topologia.h"

#include "mensaje.h"

#include <stdlib.h>

typedef struct {
    size_t cantidad_nodos;
    bool *procesos_conocidos;
    bool *canales_conocidos;
    bool completa;
} EstadoTopologia;

static bool id_valido(const EstadoTopologia *estado, int id) {
    return id >= 0 && (size_t)id < estado->cantidad_nodos;
}

static size_t indice_canal(const EstadoTopologia *estado,
                           int origen,
                           int destino) {
    return (size_t)origen * estado->cantidad_nodos + (size_t)destino;
}

static bool verificar_completa(const EstadoTopologia *estado) {
(void)estado;

    /*
     * TODO:
     * Revisar todos los canales conocidos.
     * Para cada canal <i,j>, verificar que i y j sean
     * procesos conocidos.
     */

    return false;
}

static void destruir_estado_topologia(void *estado_generico) {
    EstadoTopologia *estado = estado_generico;
    if (estado == NULL) {
        return;
    }
    free(estado->procesos_conocidos);
    free(estado->canales_conocidos);
    free(estado);
}

//implementar
static void recibir_position(Nodo *nodo,
                             const Mensaje *mensaje,
                             Simulador *simulador) {
      (void)nodo;
    (void)mensaje;
    (void)simulador;

    /*
     * TODO:
     * 1. Verificar que sea MSG_POSITION.
     * 2. Obtener el proceso k descrito por el mensaje.
     * 3. Ignorar el mensaje si k ya era conocido.
     * 4. Marcar k como conocido.
     * 5. Registrar los canales <k,l>.
     * 6. Reenviar POSITION a los vecinos, excepto al emisor.
     * 7. Comprobar si la topologia está completa.
     */
}

Nodo *nodo_topologia_crear(int id,
                           const int *vecinos,
                           size_t cantidad_vecinos,
                           size_t cantidad_nodos) {
    if (id < 0 || (size_t)id >= cantidad_nodos || cantidad_nodos == 0) {
        return NULL;
    }

    EstadoTopologia *estado = calloc(1, sizeof(*estado));
    if (estado == NULL) {
        return NULL;
    }
    estado->cantidad_nodos = cantidad_nodos;
    estado->procesos_conocidos = calloc(cantidad_nodos,
                                        sizeof(*estado->procesos_conocidos));
    estado->canales_conocidos = calloc(cantidad_nodos * cantidad_nodos,
                                       sizeof(*estado->canales_conocidos));
    if (estado->procesos_conocidos == NULL ||
        estado->canales_conocidos == NULL) {
        destruir_estado_topologia(estado);
        return NULL;
    }

    estado->procesos_conocidos[id] = true;
    for (size_t i = 0; i < cantidad_vecinos; ++i) {
        if (vecinos[i] < 0 || (size_t)vecinos[i] >= cantidad_nodos) {
            destruir_estado_topologia(estado);
            return NULL;
        }
        estado->canales_conocidos[indice_canal(estado, id, vecinos[i])] = true;
    }
    estado->completa = verificar_completa(estado);

    Nodo *nodo = nodo_crear(id,
                            vecinos,
                            cantidad_vecinos,
                            recibir_position,
                            estado,
                            destruir_estado_topologia);
    if (nodo == NULL) {
        destruir_estado_topologia(estado);
    }
    return nodo;
}

//implementar
bool nodo_topologia_iniciar(Nodo *nodo, Simulador *simulador) {
    (void)nodo;
    (void)simulador;

    /*
     * TODO:
     * 1. Crear POSITION(i, vecinos_i).
     * 2. Enviar el mensaje a todos los vecinos.
     * 3. Liberar el mensaje temporal.
     */

    return true;	
}

bool nodo_topologia_conoce_proceso(const Nodo *nodo, int proceso) {
    if (nodo == NULL || nodo->estado == NULL) {
        return false;
    }
    const EstadoTopologia *estado = nodo->estado;
    return id_valido(estado, proceso) && estado->procesos_conocidos[proceso];
}

bool nodo_topologia_conoce_canal(const Nodo *nodo, int origen, int destino) {
    if (nodo == NULL || nodo->estado == NULL) {
        return false;
    }
    const EstadoTopologia *estado = nodo->estado;
    return id_valido(estado, origen) && id_valido(estado, destino) &&
           estado->canales_conocidos[indice_canal(estado, origen, destino)];
}

bool nodo_topologia_esta_completa(const Nodo *nodo) {
    if (nodo == NULL || nodo->estado == NULL) {
        return false;
    }
    const EstadoTopologia *estado = nodo->estado;
    return estado->completa;
}
