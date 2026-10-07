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

     for (size_t i = 0; i < estado->cantidad_nodos; i++) {
        if (!estado->procesos_conocidos[i]) {
            return false;
        }
    }
    
    
    for (size_t i = 0; i < estado->cantidad_nodos; i++) {
             for (size_t j = 0; j < estado->cantidad_nodos; j++) {
               size_t idx = indice_canal(estado, (int)i, (int)j);
            if (estado->canales_conocidos[idx]) {
                       if (!estado->procesos_conocidos[i] || !estado->procesos_conocidos[j]) {
                     return false;
                     }
               }
          }
    }   
    return true;
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


static void recibir_position(Nodo *nodo,
                             const Mensaje *mensaje,
                             Simulador *simulador) {
           if (nodo == NULL || nodo->estado == NULL || mensaje == NULL) {
          return;
    }
    
    EstadoTopologia *estado = (EstadoTopologia *)nodo->estado;
    
    //Verificar que sea MSG_POSITION
          if (mensaje->tipo != MSG_POSITION) {
          return;
    }
    
    // Obtener el proceso k
         int k = mensaje->referente;
     
     // Ignorar si k ya era conocido
    if (estado->procesos_conocidos[k]) {
                     return;
    }

    // Marcar k como conocido
           estado->procesos_conocidos[k] = true;
    
            //                   RegistrAr los canales <k,l>
    for (size_t i = 0; i < mensaje->longitud; i++) {
                int vecino = mensaje->datos[i];
        if (id_valido(estado, vecino)) {
                        estado->canales_conocidos[indice_canal(estado, k, vecino)] = true;
        }
    }
    
    //  Reenviar a todos los vecinos excepto al emisor
    Mensaje msg_copy;
    if (mensaje_copiar(&msg_copy, mensaje)) {
        simulador_enviar_a_vecinos(simulador, nodo, &msg_copy, 1U, mensaje->origen);
        mensaje_destruir(&msg_copy);
    }
    
    // 7. Comprobar si la topología está completa
    if (verificar_completa(estado)) {
        estado->completa = true;
    }
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


bool nodo_topologia_iniciar(Nodo *nodo, Simulador *simulador) {
    (void)nodo;
    (void)simulador;

    
    if (nodo == NULL || nodo->estado == NULL) {
        return false;
    }
    
    
    Mensaje msg;
    if (!mensaje_crear(&msg,
                       MSG_POSITION,
                       nodo->id,
                       nodo->id,
                       0,
                       nodo->vecinos,
                       nodo->cantidad_vecinos)) {
        return false;
    }
    
    // 2. Enviar a todos los vecinos
    for (size_t i = 0; i < nodo->cantidad_vecinos; i++) {
        simulador_enviar(simulador, nodo->vecinos[i], &msg, 1U);
    }
    
    // Liberar el mensaje
    mensaje_destruir(&msg);
    
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
