#include "broadcast.h"
#include "conjunto.h"
#include "nodo.h"
#include "simulador.h"
#include "topologia.h"
#include "vecinos.h"

#include <stdbool.h>
#include <stdio.h>

#define NUM_NODOS 6
#define TIEMPO_DE_EJECUCION 10U

#define COMPROBAR(condicion, ...)                         \
    do {                                                  \
        if (!(condicion)) {                               \
            fprintf(stderr, "    FALLO: ");              \
            fprintf(stderr, __VA_ARGS__);                 \
            fprintf(stderr, "\n");                       \
            correcta = false;                             \
        }                                                 \
    } while (0)

static const int ADY_0[] = {1, 2};
static const int ADY_1[] = {0, 3};
static const int ADY_2[] = {0, 3, 5};
static const int ADY_3[] = {1, 2, 4};
static const int ADY_4[] = {3, 5};
static const int ADY_5[] = {2, 4};

static const int *const ADYACENCIAS[NUM_NODOS] = {
    ADY_0, ADY_1, ADY_2, ADY_3, ADY_4, ADY_5
};

static const size_t GRADOS[NUM_NODOS] = {2, 2, 3, 3, 2, 2};

static void destruir_nodos(Nodo **nodos) {
    for (size_t i = 0; i < NUM_NODOS; ++i) {
        nodo_destruir(nodos[i]);
        nodos[i] = NULL;
    }
}

static bool conjunto_coincide(const ConjuntoInt *conjunto,
                              const int *esperados,
                              size_t cantidad) {
    if (conjunto == NULL || conjunto->cantidad != cantidad) {
        return false;
    }
    for (size_t i = 0; i < cantidad; ++i) {
        if (!conjunto_contiene(conjunto, esperados[i])) {
            return false;
        }
    }
    return true;
}

static bool test_ejercicio_uno(void) {
    bool correcta = true;
    Simulador simulador;
    Nodo *nodos[NUM_NODOS] = {0};

    COMPROBAR(simulador_inicializar(&simulador),
              "no se pudo inicializar el simulador");

    for (size_t i = 0; i < NUM_NODOS; ++i) {
        nodos[i] = nodo_vecinos_crear((int)i, ADYACENCIAS[i], GRADOS[i]);
        COMPROBAR(nodos[i] != NULL, "no se pudo crear el nodo %zu", i);
        if (nodos[i] != NULL) {
            COMPROBAR(simulador_registrar_nodo(&simulador, nodos[i]),
                      "no se pudo registrar el nodo %zu", i);
        }
    }

    if (correcta) {
        for (size_t i = 0; i < NUM_NODOS; ++i) {
            COMPROBAR(nodo_vecinos_iniciar(nodos[i], &simulador),
                      "no se pudo iniciar el nodo %zu", i);
        }
        COMPROBAR(simulador_ejecutar_hasta(&simulador, TIEMPO_DE_EJECUCION),
                  "fallo durante la simulacion");
    }

    static const int ESP_0[] = {0, 3, 5};
    static const int ESP_1[] = {1, 2, 4};
    static const int ESP_2[] = {1, 2, 4};
    static const int ESP_3[] = {0, 3, 5};
    static const int ESP_4[] = {1, 2, 4};
    static const int ESP_5[] = {0, 3, 5};
    static const int *const ESPERADOS[] = {
        ESP_0, ESP_1, ESP_2, ESP_3, ESP_4, ESP_5
    };
    static const size_t TAM_ESPERADOS[] = {3, 3, 3, 3, 3, 3};

    if (correcta) {
        for (size_t i = 0; i < NUM_NODOS; ++i) {
            const ConjuntoInt *obtenidos =
                nodo_vecinos_identificadores(nodos[i]);
            COMPROBAR(conjunto_coincide(obtenidos,
                                        ESPERADOS[i],
                                        TAM_ESPERADOS[i]),
                      "el nodo %zu no conoce los identificadores esperados", i);
        }
        COMPROBAR(simulador.mensajes_entregados == 14,
                  "se esperaban 14 mensajes MYNAME y se entregaron %llu",
                  (unsigned long long)simulador.mensajes_entregados);
    }

    simulador_destruir(&simulador);
    destruir_nodos(nodos);
    return correcta;
}

static bool test_ejercicio_dos(void) {
    bool correcta = true;
    Simulador simulador;
    Nodo *nodos[NUM_NODOS] = {0};

    COMPROBAR(simulador_inicializar(&simulador),
              "no se pudo inicializar el simulador");

    for (size_t i = 0; i < NUM_NODOS; ++i) {
        nodos[i] = nodo_topologia_crear((int)i,
                                        ADYACENCIAS[i],
                                        GRADOS[i],
                                        NUM_NODOS);
        COMPROBAR(nodos[i] != NULL, "no se pudo crear el nodo %zu", i);
        if (nodos[i] != NULL) {
            COMPROBAR(simulador_registrar_nodo(&simulador, nodos[i]),
                      "no se pudo registrar el nodo %zu", i);
        }
    }

    if (correcta) {
        for (size_t i = 0; i < NUM_NODOS; ++i) {
            COMPROBAR(nodo_topologia_iniciar(nodos[i], &simulador),
                      "no se pudo iniciar el nodo %zu", i);
        }
        COMPROBAR(simulador_ejecutar_hasta(&simulador, TIEMPO_DE_EJECUCION),
                  "fallo durante la simulacion");
    }

    if (correcta) {
        for (size_t i = 0; i < NUM_NODOS; ++i) {
            for (int proceso = 0; proceso < NUM_NODOS; ++proceso) {
                COMPROBAR(nodo_topologia_conoce_proceso(nodos[i], proceso),
                          "el nodo %zu no conoce al proceso %d", i, proceso);
            }

            for (int origen = 0; origen < NUM_NODOS; ++origen) {
                for (size_t j = 0; j < GRADOS[origen]; ++j) {
                    int destino = ADYACENCIAS[origen][j];
                    COMPROBAR(nodo_topologia_conoce_canal(nodos[i],
                                                          origen,
                                                          destino),
                              "el nodo %zu no conoce el canal <%d,%d>",
                              i,
                              origen,
                              destino);
                }
            }
            COMPROBAR(nodo_topologia_esta_completa(nodos[i]),
                      "el nodo %zu no marco la topologia como completa", i);
        }
    }

    simulador_destruir(&simulador);
    destruir_nodos(nodos);
    return correcta;
}

static bool test_ejercicio_tres(void) {
    bool correcta = true;
    const int mensaje_esperado = 2025;
    Simulador simulador;
    Nodo *nodos[NUM_NODOS] = {0};

    COMPROBAR(simulador_inicializar(&simulador),
              "no se pudo inicializar el simulador");

    for (size_t i = 0; i < NUM_NODOS; ++i) {
        nodos[i] = nodo_broadcast_crear((int)i,
                                        ADYACENCIAS[i],
                                        GRADOS[i],
                                        0,
                                        mensaje_esperado);
        COMPROBAR(nodos[i] != NULL, "no se pudo crear el nodo %zu", i);
        if (nodos[i] != NULL) {
            COMPROBAR(simulador_registrar_nodo(&simulador, nodos[i]),
                      "no se pudo registrar el nodo %zu", i);
        }
    }

    if (correcta) {
        for (size_t i = 0; i < NUM_NODOS; ++i) {
            COMPROBAR(nodo_broadcast_iniciar(nodos[i], &simulador),
                      "no se pudo iniciar el nodo %zu", i);
        }
        COMPROBAR(simulador_ejecutar_hasta(&simulador, TIEMPO_DE_EJECUCION),
                  "fallo durante la simulacion");
    }

    if (correcta) {
        for (size_t i = 0; i < NUM_NODOS; ++i) {
            COMPROBAR(nodo_broadcast_vio_mensaje(nodos[i]),
                      "el nodo %zu no vio el mensaje", i);
            COMPROBAR(nodo_broadcast_mensaje(nodos[i]) == mensaje_esperado,
                      "el nodo %zu tiene un mensaje incorrecto", i);
        }
        COMPROBAR(!simulador_tiene_eventos(&simulador),
                  "quedaron eventos sin procesar despues del tiempo limite");
    }

    simulador_destruir(&simulador);
    destruir_nodos(nodos);
    return correcta;
}

typedef bool (*FuncionPrueba)(void);

static int ejecutar_prueba(const char *nombre, FuncionPrueba prueba) {
    printf("[PRUEBA] %s\n", nombre);
    if (prueba()) {
        printf("  OK\n");
        return 0;
    }
    printf("  FALLO\n");
    return 1;
}

int main(void) {
    int fallos = 0;
    fallos += ejecutar_prueba("Ejercicio 1: vecinos de vecinos",
                              test_ejercicio_uno);
    fallos += ejecutar_prueba("Ejercicio 2: conocimiento de la topologia",
                              test_ejercicio_dos);
    fallos += ejecutar_prueba("Ejercicio 3: broadcast ingenuo",
                              test_ejercicio_tres);

    if (fallos == 0) {
        printf("\nTodas las pruebas pasaron.\n");
        return 0;
    }

    fprintf(stderr, "\nFallaron %d prueba(s).\n", fallos);
    return 1;
}
