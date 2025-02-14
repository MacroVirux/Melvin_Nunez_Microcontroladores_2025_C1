#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Estados posibles del portón eléctrico
typedef enum {
    INICIO,
    CERRADO,
    ABIERTO,
    ABRIENDO,
    CERRANDO,
    ERROR
} Estado;

// Definición de tiempos y códigos de error
#define TIEMPO_MAXIMO 180
#define ERROR_OK 0
#define ERROR_LIMITE 1
#define ERROR_TIEMPO 2
#define ERROR_INICIO 3
#define PARPADEO_LENTO 20 // Ajuste de parpadeo para indicador

// Estructura de control del sistema
typedef struct {
    bool sensorCerrado;
    bool sensorAbierto;
    bool botonActivacion;
    bool motorAbrir;
    bool motorCerrar;
    int contadorTiempo;
    bool ledAbrir;
    bool ledCerrar;
    bool ledError;
    int codigoError;
    bool datosListos;
    bool salir; // Permite salir del bucle principal
} ControlPorton;

// Prototipos de funciones
Estado iniciar_sistema(ControlPorton *porton);
Estado estado_cerrado(ControlPorton *porton);
Estado estado_abierto(ControlPorton *porton);
Estado estado_abriendo(ControlPorton *porton);
Estado estado_cerrando(ControlPorton *porton);
Estado manejar_error(ControlPorton *porton, int error);
void temporizador_50ms(ControlPorton *porton);

// Función principal
int main() {
    Estado estadoActual = INICIO;
    ControlPorton porton = {0}; // Inicializar estructura con valores en 0

    while (!porton.salir) {
        temporizador_50ms(&porton);

        switch (estadoActual) {
            case INICIO:      estadoActual = iniciar_sistema(&porton); break;
            case CERRADO:     estadoActual = estado_cerrado(&porton); break;
            case ABIERTO:     estadoActual = estado_abierto(&porton); break;
            case ABRIENDO:    estadoActual = estado_abriendo(&porton); break;
            case CERRANDO:    estadoActual = estado_cerrando(&porton); break;
            case ERROR:       estadoActual = manejar_error(&porton, porton.codigoError); break;
        }
    }
    return 0;
}

// Implementación de los estados
Estado iniciar_sistema(ControlPorton *porton) {
    if (!porton->datosListos) {
        porton->codigoError = ERROR_INICIO;
        return ERROR;
    }
    return porton->sensorCerrado ? CERRADO : ABIERTO;
}

Estado estado_cerrado(ControlPorton *porton) {
    porton->motorAbrir = false;
    porton->motorCerrar = false;
    porton->ledAbrir = false;
    porton->ledCerrar = true;
    if (porton->botonActivacion) {
        return ABRIENDO;
    }
    return CERRADO;
}

Estado estado_abierto(ControlPorton *porton) {
    porton->motorAbrir = false;
    porton->motorCerrar = false;
    porton->ledAbrir = true;
    porton->ledCerrar = false;
    if (porton->botonActivacion) {
        return CERRANDO;
    }
    return ABIERTO;
}

Estado estado_abriendo(ControlPorton *porton) {
    porton->motorAbrir = true;
    porton->motorCerrar = false;
    if (porton->sensorAbierto) {
        return ABIERTO;
    }
    if (porton->contadorTiempo >= TIEMPO_MAXIMO) {
        porton->codigoError = ERROR_TIEMPO;
        return ERROR;
    }
    return ABRIENDO;
}

Estado estado_cerrando(ControlPorton *porton) {
    porton->motorAbrir = false;
    porton->motorCerrar = true;
    if (porton->sensorCerrado) {
        return CERRADO;
    }
    if (porton->contadorTiempo >= TIEMPO_MAXIMO) {
        porton->codigoError = ERROR_TIEMPO;
        return ERROR;
    }
    return CERRANDO;
}

Estado manejar_error(ControlPorton *porton, int error) {
    porton->motorAbrir = false;
    porton->motorCerrar = false;
    porton->ledAbrir = false;
    porton->ledCerrar = false;
    porton->ledError = true;
    return ERROR;
}

void temporizador_50ms(ControlPorton *porton) {
    if (porton->motorAbrir || porton->motorCerrar) {
        porton->contadorTiempo++;
    }
}
