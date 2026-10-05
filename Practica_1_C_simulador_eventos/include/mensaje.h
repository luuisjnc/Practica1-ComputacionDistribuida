#ifndef MENSAJE_H
#define MENSAJE_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    MSG_MYNAME,
    MSG_POSITION,
    MSG_BROADCAST
} TipoMensaje;

typedef struct {
    TipoMensaje tipo;
    int origen;
    int referente;
    int valor;
    int *datos;
    size_t longitud;
} Mensaje;

bool mensaje_crear(Mensaje *mensaje,
                   TipoMensaje tipo,
                   int origen,
                   int referente,
                   int valor,
                   const int *datos,
                   size_t longitud);
bool mensaje_copiar(Mensaje *destino, const Mensaje *origen);
void mensaje_destruir(Mensaje *mensaje);

#endif
