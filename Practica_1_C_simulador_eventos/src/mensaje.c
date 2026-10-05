#include "mensaje.h"

#include <stdlib.h>
#include <string.h>

bool mensaje_crear(Mensaje *mensaje,
                   TipoMensaje tipo,
                   int origen,
                   int referente,
                   int valor,
                   const int *datos,
                   size_t longitud) {
    mensaje->tipo = tipo;
    mensaje->origen = origen;
    mensaje->referente = referente;
    mensaje->valor = valor;
    mensaje->datos = NULL;
    mensaje->longitud = longitud;

    if (longitud == 0) {
        return true;
    }
    if (datos == NULL) {
        mensaje->longitud = 0;
        return false;
    }

    mensaje->datos = malloc(longitud * sizeof(*mensaje->datos));
    if (mensaje->datos == NULL) {
        mensaje->longitud = 0;
        return false;
    }
    memcpy(mensaje->datos, datos, longitud * sizeof(*datos));
    return true;
}

bool mensaje_copiar(Mensaje *destino, const Mensaje *origen) {
    return mensaje_crear(destino,
                         origen->tipo,
                         origen->origen,
                         origen->referente,
                         origen->valor,
                         origen->datos,
                         origen->longitud);
}

void mensaje_destruir(Mensaje *mensaje) {
    free(mensaje->datos);
    mensaje->datos = NULL;
    mensaje->longitud = 0;
}
