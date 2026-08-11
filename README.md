```
 ██████╗ ███████╗███████╗███████╗██████╗ ██╗   ██╗ █████╗ ███████╗
 ██╔══██╗██╔════╝██╔════╝██╔════╝██╔══██╗██║   ██║██╔══██╗██╔════╝
 ██████╔╝█████╗  ███████╗█████╗  ██████╔╝██║   ██║███████║███████╗
 ██╔══██╗██╔══╝  ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══██║╚════██║
 ██║  ██║███████╗███████║███████╗██║  ██║ ╚████╔╝ ██║  ██║███████║
 ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚═╝  ╚═╝╚══════╝
```

### Sistema de Reservación de Laboratorios de Cómputo

Programa en C para administrar la reservación de laboratorios de cómputo compartidos entre grupos y maestros. Las solicitudes se procesan en el orden en que llegan mediante una cola, y el historial se mantiene ordenado por fecha y hora.

---

### Índice

- [Cómo funciona](#cómo-funciona)
- [Compilación](#compilación)
- [Uso](#uso)
- [Menú](#menú)
- [Persistencia](#persistencia)
- [Estructura interna](#estructura-interna)
- [Limitaciones](#limitaciones)

---

### Cómo funciona

Pedir un horario y confirmarlo son dos pasos distintos. Toda solicitud entra primero a una cola FIFO; se atiende en el orden de llegada y, hasta ese momento, se valida si el horario sigue disponible.

```
solicitud  ─▶  cola FIFO  ─▶  procesar  ─▶  ¿horario libre?
                                              │
                                   sí ────────┘────────── no
                                    │                       │
                             reserva confirmada      solicitud descartada
```

---

### Compilación

```
gcc -o reservas reservas.c -Wall
```

### Uso

```
./reservas
```

En Windows, compilado con MinGW o similar:

```
reservas.exe
```

---

### Menú

| Opción | Acción                                |
|:------:|----------------------------------------|
|   1    | Registrar laboratorio                   |
|   2    | Solicitar reservación                   |
|   3    | Procesar siguiente solicitud en cola    |
|   4    | Cancelar reservación                    |
|   5    | Consultar disponibilidad                |
|   6    | Ver historial ordenado por fecha/hora   |
|   7    | Ver catálogo de laboratorios            |
|   8    | Buscar reservas por solicitante         |
|   9    | Salir                                   |

---

### Persistencia

Cada reserva confirmada o cancelada se escribe en `reservas.txt`. Al iniciar, el programa carga automáticamente lo que encuentre ahí, así que la información sobrevive entre ejecuciones sin depender de una base de datos externa.

---

### Estructura interna

| Componente          | Implementación                                   |
|----------------------|--------------------------------------------------|
| Solicitudes en espera | Cola circular sobre arreglo (FIFO)               |
| Reservas confirmadas  | Arreglo con baja lógica (bandera `activa`)       |
| Búsqueda de laboratorio / historial | Búsqueda secuencial                |
| Historial por fecha/hora | Ordenamiento por inserción                    |
| Validación de fecha/hora | Rango real por campo (día, mes, año, hora, minuto) |

---

### Limitaciones

Sin conexión a internet, sin usuarios ni contraseñas, sin notificaciones. Las reservas cubren un horario exacto, sin bloques de tiempo ni duración. El archivo de persistencia no está cifrado.

---

`Gustavo Leonel Gálvez Hernández · LIA02326MA`
