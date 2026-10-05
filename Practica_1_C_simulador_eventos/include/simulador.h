#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "mensaje.h"
#include "nodo.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Evento Evento;

struct Simulador {
    uint64_t tiempo_actual;
    uint64_t siguiente_orden;
    uint64_t mensajes_programados;
    uint64_t mensajes_entregados;
    Nodo **nodos;
    size_t capacidad_nodos;
    Evento *eventos;
    bool error_memoria;
};

bool simulador_inicializar(Simulador *simulador);
bool simulador_registrar_nodo(Simulador *simulador, Nodo *nodo);
bool simulador_enviar(Simulador *simulador,
                      int destino,
                      const Mensaje *mensaje,
                      uint64_t demora);
bool simulador_enviar_a_vecinos(Simulador *simulador,
                                const Nodo *nodo,
                                const Mensaje *mensaje,
                                uint64_t demora,
                                int vecino_excluido);
bool simulador_ejecutar_hasta(Simulador *simulador, uint64_t limite_tiempo);
bool simulador_tiene_eventos(const Simulador *simulador);
void simulador_destruir(Simulador *simulador);

#endif
