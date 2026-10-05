# Practica 1 migrada a C

Implementacion completa de los tres algoritmos de la Practica 1 usando un
simulador discreto de eventos escrito en C y sin bibliotecas externas.

## Compilar y ejecutar

```bash
make test
```


## Estructura

```text
include/                 Interfaces publicas
src/                     Implementaciones
tests/test_practica1.c   Pruebas de los tres ejercicios
Makefile                 Compilacion y ejecucion
```

Los modulos principales son:

- `simulador`: cola estable de eventos ordenada por tiempo y orden de insercion;
- `mensaje`: mensajes con copia profunda de sus datos;
- `nodo`: identificador, vecinos, estado privado y manejador de recepcion;
- `vecinos`: implementacion de `MYNAME`;
- `topologia`: flooding de mensajes `POSITION`;
- `broadcast`: broadcast ingenuo desde el nodo distinguido 0.




## Propiedad de la memoria

- La prueba crea y destruye cada `Nodo`.
- Cada nodo es dueno de su arreglo de vecinos y de su estado especifico.
- Cada `Evento` es dueno de la copia de su `Mensaje`.
- `simulador_destruir` elimina cualquier evento que haya quedado pendiente.

El simulador registra punteros a nodos, pero no se vuelve dueno de ellos. Por
eso primero se destruye el simulador y despues los nodos.
