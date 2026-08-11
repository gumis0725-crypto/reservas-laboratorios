#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#define MAX_LABS 20
#define MAX_RESERVAS 100
#define MAX_COLA 50
#define MAX_NOM 50
#define MAX_FECHA 11
#define MAX_HORA 6
#define ARCHIVO_RESERVAS "reservas.txt"

typedef struct {
    int id;
    char nombre[MAX_NOM];
    int capacidad;
} Laboratorio;

typedef struct {
    int idLab;
    char solicitante[MAX_NOM];
    char fecha[MAX_FECHA];
    char hora[MAX_HORA];
} Solicitud;

typedef struct {
    int idLab;
    char solicitante[MAX_NOM];
    char fecha[MAX_FECHA];
    char hora[MAX_HORA];
    int activa;
} Reserva;

Laboratorio listaLabs[MAX_LABS];
int totalLabs = 0;

Reserva reservas[MAX_RESERVAS];
int totalReservas = 0;

Solicitud cola[MAX_COLA];
int frente = 0, final = -1, totalCola = 0;

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void leerLinea(char *dest, int tam) {
    if (fgets(dest, tam, stdin) != NULL) {
        int len = strlen(dest);
        if (len > 0 && dest[len - 1] == '\n') {
            dest[len - 1] = '\0';
        } else {
            limpiarBuffer();
        }
    }
}

int leerEntero(char *msg) {
    char linea[64];
    int val;
    printf("%s", msg);
    leerLinea(linea, sizeof(linea));
    if (sscanf(linea, "%d", &val) != 1) {
        return -1;
    }
    return val;
}

int fechaValida(char *fecha) {
    int d, m, a;
    if (sscanf(fecha, "%d/%d/%d", &d, &m, &a) != 3) {
        return 0;
    }
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0;
    if (a < 2024 || a > 2100) return 0;

    int diasPorMes[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (d > diasPorMes[m - 1]) return 0;

    return 1;
}

int horaValida(char *hora) {
    int h, min;
    if (sscanf(hora, "%d:%d", &h, &min) != 2) {
        return 0;
    }
    if (h < 0 || h > 23) return 0;
    if (min < 0 || min > 59) return 0;
    return 1;
}

// pide la fecha repetidamente hasta que el usuario meta una valida
void leerFechaValida(char *dest, int tam) {
    do {
        printf("Fecha (DD/MM/AAAA): ");
        leerLinea(dest, tam);
        if (!fechaValida(dest)) {
            printf("Fecha invalida, intenta de nuevo.\n");
        }
    } while (!fechaValida(dest));
}

void leerHoraValida(char *dest, int tam) {
    do {
        printf("Hora (HH:MM): ");
        leerLinea(dest, tam);
        if (!horaValida(dest)) {
            printf("Hora invalida, intenta de nuevo.\n");
        }
    } while (!horaValida(dest));
}

void cargarLabsIniciales() {
    strcpy(listaLabs[0].nombre, "Laboratorio A");
    listaLabs[0].id = 1;
    listaLabs[0].capacidad = 25;

    strcpy(listaLabs[1].nombre, "Laboratorio B");
    listaLabs[1].id = 2;
    listaLabs[1].capacidad = 30;

    strcpy(listaLabs[2].nombre, "Laboratorio C");
    listaLabs[2].id = 3;
    listaLabs[2].capacidad = 20;

    totalLabs = 3;
}

int buscarLabPorNombre(char *nombre) {
    for (int i = 0; i < totalLabs; i++) {
        if (strcasecmp(listaLabs[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

void registrarLab() {
    if (totalLabs >= MAX_LABS) {
        printf("Ya no hay espacio para mas laboratorios.\n");
        return;
    }
    char nombre[MAX_NOM];
    printf("Nombre del laboratorio: ");
    leerLinea(nombre, sizeof(nombre));
    int cap = leerEntero("Capacidad: ");

    listaLabs[totalLabs].id = totalLabs + 1;
    strcpy(listaLabs[totalLabs].nombre, nombre);
    listaLabs[totalLabs].capacidad = cap;
    totalLabs++;

    printf("Laboratorio agregado con id %d\n", totalLabs);
}

void mostrarLabs() {
    printf("\nID  NOMBRE                CAPACIDAD\n");
    for (int i = 0; i < totalLabs; i++) {
        printf("%-3d %-20s %d\n", listaLabs[i].id, listaLabs[i].nombre, listaLabs[i].capacidad);
    }
}

// se mete la solicitud a la cola, se atiende despues en orden de llegada
void solicitarReserva() {
    char nombreLab[MAX_NOM];
    printf("Laboratorio: ");
    leerLinea(nombreLab, sizeof(nombreLab));

    int idx = buscarLabPorNombre(nombreLab);
    if (idx == -1) {
        printf("No existe ese laboratorio.\n");
        return;
    }

    if (totalCola >= MAX_COLA) {
        printf("La cola de solicitudes esta llena.\n");
        return;
    }

    Solicitud s;
    s.idLab = listaLabs[idx].id;
    printf("Nombre del solicitante: ");
    leerLinea(s.solicitante, sizeof(s.solicitante));
    leerFechaValida(s.fecha, sizeof(s.fecha));
    leerHoraValida(s.hora, sizeof(s.hora));

    final = (final + 1) % MAX_COLA;
    cola[final] = s;
    totalCola++;

    printf("Solicitud puesta en la cola, posicion %d\n", totalCola);
}

int horarioOcupado(int idLab, char *fecha, char *hora) {
    for (int i = 0; i < totalReservas; i++) {
        if (reservas[i].activa && reservas[i].idLab == idLab &&
            strcmp(reservas[i].fecha, fecha) == 0 &&
            strcmp(reservas[i].hora, hora) == 0) {
            return 1;
        }
    }
    return 0;
}

void procesarSolicitud() {
    if (totalCola == 0) {
        printf("No hay solicitudes en espera.\n");
        return;
    }

    Solicitud s = cola[frente];
    frente = (frente + 1) % MAX_COLA;
    totalCola--;

    printf("Procesando solicitud de %s...\n", s.solicitante);

    if (horarioOcupado(s.idLab, s.fecha, s.hora)) {
        printf("Ese horario ya esta ocupado, no se pudo reservar.\n");
        return;
    }

    if (totalReservas >= MAX_RESERVAS) {
        printf("Ya no hay espacio para mas reservas.\n");
        return;
    }

    reservas[totalReservas].idLab = s.idLab;
    strcpy(reservas[totalReservas].solicitante, s.solicitante);
    strcpy(reservas[totalReservas].fecha, s.fecha);
    strcpy(reservas[totalReservas].hora, s.hora);
    reservas[totalReservas].activa = 1;
    totalReservas++;

    printf("Reserva registrada.\n");
}

void cancelarReserva() {
    char nombreLab[MAX_NOM];
    printf("Laboratorio de la reserva: ");
    leerLinea(nombreLab, sizeof(nombreLab));

    int idx = buscarLabPorNombre(nombreLab);
    if (idx == -1) {
        printf("No existe ese laboratorio.\n");
        return;
    }

    char fecha[MAX_FECHA], hora[MAX_HORA];
    leerFechaValida(fecha, sizeof(fecha));
    leerHoraValida(hora, sizeof(hora));

    for (int i = 0; i < totalReservas; i++) {
        if (reservas[i].activa && reservas[i].idLab == listaLabs[idx].id &&
            strcmp(reservas[i].fecha, fecha) == 0 &&
            strcmp(reservas[i].hora, hora) == 0) {
            reservas[i].activa = 0;
            printf("Reserva cancelada.\n");
            return;
        }
    }
    printf("No se encontro esa reserva.\n");
}

void consultarDisponibilidad() {
    char nombreLab[MAX_NOM];
    printf("Laboratorio a consultar: ");
    leerLinea(nombreLab, sizeof(nombreLab));

    int idx = buscarLabPorNombre(nombreLab);
    if (idx == -1) {
        printf("No existe ese laboratorio.\n");
        return;
    }

    printf("\nHorarios ocupados en %s:\n", listaLabs[idx].nombre);
    int encontrados = 0;
    for (int i = 0; i < totalReservas; i++) {
        if (reservas[i].activa && reservas[i].idLab == listaLabs[idx].id) {
            printf("%s %s - %s\n", reservas[i].fecha, reservas[i].hora, reservas[i].solicitante);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("No hay reservas activas para este laboratorio.\n");
    }
}

void buscarPorSolicitante() {
    char nombre[MAX_NOM];
    printf("Nombre del solicitante a buscar: ");
    leerLinea(nombre, sizeof(nombre));

    int encontrados = 0;
    printf("\nFECHA      HORA  LAB SOLICITANTE          ESTADO\n");
    for (int i = 0; i < totalReservas; i++) {
        if (strcasecmp(reservas[i].solicitante, nombre) == 0) {
            printf("%-10s %-5s %-3d %-20s %s\n", reservas[i].fecha, reservas[i].hora,
                   reservas[i].idLab, reservas[i].solicitante,
                   reservas[i].activa ? "Activa" : "Cancelada");
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("No se encontraron reservas de ese solicitante.\n");
    }
}

// convierte fecha y hora a un numero para poder compararlas
long convertirAOrden(char *fecha, char *hora) {
    int d, m, a, h, min;
    sscanf(fecha, "%d/%d/%d", &d, &m, &a);
    sscanf(hora, "%d:%d", &h, &min);
    return (long)a * 100000000L + (long)m * 1000000L + (long)d * 10000L + (long)h * 100L + min;
}

void ordenarHistorial() {
    for (int i = 1; i < totalReservas; i++) {
        Reserva temp = reservas[i];
        long claveTemp = convertirAOrden(temp.fecha, temp.hora);
        int j = i - 1;
        while (j >= 0 && convertirAOrden(reservas[j].fecha, reservas[j].hora) > claveTemp) {
            reservas[j + 1] = reservas[j];
            j--;
        }
        reservas[j + 1] = temp;
    }
}

void mostrarHistorial() {
    ordenarHistorial();
    printf("\nFECHA      HORA  LAB SOLICITANTE          ESTADO\n");
    for (int i = 0; i < totalReservas; i++) {
        printf("%-10s %-5s %-3d %-20s %s\n", reservas[i].fecha, reservas[i].hora,
               reservas[i].idLab, reservas[i].solicitante,
               reservas[i].activa ? "Activa" : "Cancelada");
    }
    printf("Total: %d\n", totalReservas);
}

void guardarReservas() {
    FILE *f = fopen(ARCHIVO_RESERVAS, "w");
    if (f == NULL) {
        printf("No se pudo guardar el archivo.\n");
        return;
    }
    for (int i = 0; i < totalReservas; i++) {
        fprintf(f, "%d;%s;%s;%s;%d\n", reservas[i].idLab, reservas[i].solicitante,
                reservas[i].fecha, reservas[i].hora, reservas[i].activa);
    }
    fclose(f);
}

void cargarReservas() {
    FILE *f = fopen(ARCHIVO_RESERVAS, "r");
    if (f == NULL) {
        return; // no hay archivo previo, se arranca vacio
    }

    while (totalReservas < MAX_RESERVAS &&
           fscanf(f, "%d;%49[^;];%10[^;];%5[^;];%d\n",
                  &reservas[totalReservas].idLab,
                  reservas[totalReservas].solicitante,
                  reservas[totalReservas].fecha,
                  reservas[totalReservas].hora,
                  &reservas[totalReservas].activa) == 5) {
        totalReservas++;
    }

    fclose(f);
}

void menu() {
    printf("\n1) Registrar laboratorio\n");
    printf("2) Solicitar reservacion\n");
    printf("3) Procesar siguiente solicitud\n");
    printf("4) Cancelar reservacion\n");
    printf("5) Consultar disponibilidad\n");
    printf("6) Ver historial\n");
    printf("7) Ver laboratorios\n");
    printf("8) Buscar reservas por solicitante\n");
    printf("9) Salir\n");
    printf("Solicitudes en cola: %d | Reservas: %d\n", totalCola, totalReservas);
}

int main() {
    cargarLabsIniciales();
    cargarReservas();
    int opcion;

    do {
        menu();
        opcion = leerEntero("Opcion: ");

        switch (opcion) {
            case 1: registrarLab(); break;
            case 2: solicitarReserva(); break;
            case 3: procesarSolicitud(); guardarReservas(); break;
            case 4: cancelarReserva(); guardarReservas(); break;
            case 5: consultarDisponibilidad(); break;
            case 6: mostrarHistorial(); break;
            case 7: mostrarLabs(); break;
            case 8: buscarPorSolicitante(); break;
            case 9: printf("Adios\n"); break;
            default: printf("Opcion invalida\n");
        }
    } while (opcion != 9);

    return 0;
}
