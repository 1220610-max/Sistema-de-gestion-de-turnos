//Eri Santiago, Estructuras de datos, 1220610 Sistema de gesstion de turnos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALTA   1
#define NORMAL 0

// nodo de cola = paciente
typedef struct Paciente {
    char nombre[50];
    char sintoma[100];
    int prioridad;              // 1 = Alta, 0 = Normal
    struct Paciente *siguiente; // Enlace al siguiente paciente en la cola 
} Paciente;

//Lee una linea completa y le quita el salto de linea 
void leerCadena(const char *mensaje, char destino[], int tam) {
    printf("%s", mensaje);
    if (fgets(destino, tam, stdin) != NULL) {
        destino[strcspn(destino, "\n")] = '\0';
    } else {
        destino[0] = '\0';
    }
}
//Lee un entero
int leerEntero(const char *mensaje) {
    char buffer[32];
    int valor;
    while (1) {
        printf("%s", mensaje);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL &&
            sscanf(buffer, "%d", &valor) == 1) {
            return valor;
        }
        printf("Entrada invalida. Intente de nuevo.\n");
    }
}

//Devuelve 1 si la cola esta vacia, 0 si no
int estaVacia(Paciente *frente) {
    return frente == NULL;
}

//Agrega un paciente al FINAL de la cola (FIFO) 
void encolar(Paciente **frente, Paciente **final,
             char nombre[], char sintoma[], int prioridad) {
    Paciente *nuevo = (Paciente *)malloc(sizeof(Paciente));
    if (nuevo == NULL) {
        printf("Error: no hay memoria disponible.\n");
        return;
    }

    strncpy(nuevo->nombre, nombre, sizeof(nuevo->nombre) - 1);
    nuevo->nombre[sizeof(nuevo->nombre) - 1] = '\0';
    strncpy(nuevo->sintoma, sintoma, sizeof(nuevo->sintoma) - 1);
    nuevo->sintoma[sizeof(nuevo->sintoma) - 1] = '\0';
    nuevo->prioridad = prioridad;
    nuevo->siguiente = NULL;

    if (estaVacia(*frente)) {
        *frente = nuevo;         
    } else {
        (*final)->siguiente = nuevo; // Enlaza el nuevo nodo al final de la cola
    }
    *final = nuevo;
}

//Saca el paciente del FRENTE de la cola.Devuelve el nodo desenlazado o NULL si la cola estaba vacia.
Paciente *desencolar(Paciente **frente, Paciente **final) {
    if (estaVacia(*frente)) {
        return NULL; 
    }

    Paciente *atendido = *frente;
    *frente = atendido->siguiente;
    if (*frente == NULL) {
        *final = NULL;          
    }
    atendido->siguiente = NULL;
    return atendido;
}

//Imprime todos los pacientes de una cola, del inicio al final 
void mostrarCola(Paciente *frente) {
    if (estaVacia(frente)) {
        printf("   (vacia)\n");
        return;
    }
    int pos = 1;
    for (Paciente *aux = frente; aux != NULL; aux = aux->siguiente) {
        printf("   %d. %s - %s\n", pos++, aux->nombre, aux->sintoma);
    }
}

//Cuenta los pacientes de una cola
int contarCola(Paciente *frente) {
    int n = 0;
    for (Paciente *aux = frente; aux != NULL; aux = aux->siguiente) n++;
    return n;
}

//Busca un paciente por nombre; regresa su posicion o 0 si no esta
int buscarEnCola(Paciente *frente, const char nombre[]) {
    int pos = 1;
    for (Paciente *aux = frente; aux != NULL; aux = aux->siguiente, pos++) {
        if (strcmp(aux->nombre, nombre) == 0) return pos;
    }
    return 0;
}

// Escribe una cola en un archivo abierto
void guardarCola(FILE *f, Paciente *frente, const char *titulo) {
    fprintf(f, "%s\n", titulo);
    for (Paciente *aux = frente; aux != NULL; aux = aux->siguiente) {
        fprintf(f, "  %s | %s\n", aux->nombre, aux->sintoma);
    }
}

//Aqui se guarda el estado de las 2 colas en un archivo de texto
void guardarEstado(Paciente *frenteAlta, Paciente *frenteNormal) {
    FILE *f = fopen("estado_colas.txt", "w");
    if (f == NULL) {
        printf("No se pudo crear el archivo.\n");
        return;
    }
    guardarCola(f, frenteAlta, "PRIORIDAD ALTA:");
    guardarCola(f, frenteNormal, "PRIORIDAD NORMAL:");
    fclose(f);
    printf("Estado guardado en estado_colas.txt\n");
}

//Libera todos los nodos que sobran de una cola
void liberarCola(Paciente **frente, Paciente **final) {
    Paciente *p;
    while ((p = desencolar(frente, final)) != NULL) {
        free(p);
    }
}
// Programa principal
int main(void) {
    Paciente *frenteAlta = NULL,   *finalAlta = NULL;
    Paciente *frenteNormal = NULL, *finalNormal = NULL;

    int opcion;
    do {
        printf("\nSALA DE EMERGENCIAS\n");
        printf("1. Agregar paciente\n");
        printf("2. Atender siguiente paciente\n");
        printf("3. Mostrar colas\n");
        printf("4. Salir\n");
        printf("5. (Extra) Buscar paciente por nombre\n");
        printf("6. (Extra) Guardar estado en archivo\n");
        opcion = leerEntero("Opcion: ");

        switch (opcion) {
        case 1: {
            char nombre[50], sintoma[100];
            int prioridad;

            leerCadena("Nombre: ", nombre, sizeof(nombre));
            leerCadena("Sintoma: ", sintoma, sizeof(sintoma));
            do {
                prioridad = leerEntero("Prioridad (1 = Alta, 0 = Normal): ");
                if (prioridad != ALTA && prioridad != NORMAL)
                    printf("Prioridad invalida.\n");
            } while (prioridad != ALTA && prioridad != NORMAL);

            if (prioridad == ALTA)
                encolar(&frenteAlta, &finalAlta, nombre, sintoma, prioridad);
            else
                encolar(&frenteNormal, &finalNormal, nombre, sintoma, prioridad);

            printf("Paciente registrado en la cola %s.\n",
                   prioridad == ALTA ? "ALTA" : "NORMAL");
            break;
        }
        case 2: {
            Paciente *atendido = desencolar(&frenteAlta, &finalAlta);
            if (atendido == NULL) {
                atendido = desencolar(&frenteNormal, &finalNormal);
            }

            if (atendido == NULL) {
                printf("No hay pacientes en espera.\n");
            } else {
                printf("Atendiendo a: %s (%s) - Prioridad %s\n",
                       atendido->nombre, atendido->sintoma,
                       atendido->prioridad == ALTA ? "Alta" : "Normal");
                free(atendido); /* Se libera el nodo ya atendido */
            }
            break;
        }
        case 3:
            printf("\nPRIORIDAD ALTA (%d)\n", contarCola(frenteAlta));
            mostrarCola(frenteAlta);
            printf("\nPRIORIDAD NORMAL (%d)\n", contarCola(frenteNormal));
            mostrarCola(frenteNormal);
            printf("\nTotal en espera: %d\n",
                   contarCola(frenteAlta) + contarCola(frenteNormal));
            break;
        case 4:
            printf("Cerrando sistema...\n");
            break;
        case 5: {
            char nombre[50];
            leerCadena("Nombre a buscar: ", nombre, sizeof(nombre));
            int pos = buscarEnCola(frenteAlta, nombre);
            if (pos > 0) {
                printf("%s esta en la cola ALTA, posicion %d.\n", nombre, pos);
            } else if ((pos = buscarEnCola(frenteNormal, nombre)) > 0) {
                printf("%s esta en la cola NORMAL, posicion %d.\n", nombre, pos);
            } else {
                printf("Paciente no encontrado.\n");
            }
            break;
        }
        case 6:
            guardarEstado(frenteAlta, frenteNormal);
            break;
        default:
            printf("Opcion invalida.\n");
        }
    } while (opcion != 4);

//Se libera la memoria de los pacientes que no fueron atendidos
    liberarCola(&frenteAlta, &finalAlta);
    liberarCola(&frenteNormal, &finalNormal);

    return 0;
}