# Proyecto 1 — Hoja de Cálculo con Matriz Dispersa

Curso: Algoritmos y Estructuras de Datos  
Lenguaje: C++17 | GUI: Qt6

---

## Estructura de archivos

```
include/
  Node.h          – Nodo con (row, col, value, nextInRow, nextInCol)
  RowHeader.h     – Cabecera de fila
  ColHeader.h     – Cabecera de columna
  SparseMatrix.h  – Declaración completa de la clase
src/
  SparseMatrix.cpp – Implementación de todas las operaciones
  main.cpp         – Demo de consola (prueba todas las operaciones)
  main_gui.cpp     – Punto de entrada de la interfaz gráfica
  MainWindow.h/cpp – Ventana Qt5 con tabla interactiva
CMakeLists.txt
README.md
```

---

## Compilación

### Requisitos
- CMake ≥ 3.16
- Compilador C++17 

### Ubuntu/Debian
```bash
sudo apt install cmake g++ qtbase5-dev
mkdir build && cd build
cmake ..
make
```

### Ejecutar consola
```bash
./build/PROYECTOAED_console
```

### Ejecutar GUI
```bash
./build/PROYECTOAED_guigit status
```

---s

## Operaciones implementadas

| # | Operación | Método |
|---|-----------|--------|
| 1 | Insertar / actualizar celda | `insertCell(r,c,val)` |
| 2 | Consultar celda | `getCell(r,c)` |
| 3 | Modificar celda sin crear nodo | `modifyCell(r,c,val)` |
| 4 | Eliminar celda | `deleteCell(r,c)` |
| 5 | Eliminar fila | `deleteRow(r)` |
| 6 | Eliminar columna | `deleteColumn(c)` |
| 7 | Eliminar rango rectangular | `deleteRange(r1,c1,r2,c2)` |
| 8 | SUMA fila / columna | `sumRow`, `sumColumn` |
| 9 | SUMA rango | `sumRange(r1,c1,r2,c2)` |
| 10 | PROMEDIO fila / columna / rango | `averageRow`, `averageColumn`, `averageRange` |
| 11 | MÁXIMO fila / columna / rango | `findMaxRow`, `findMaxColumn`, `maxRange` |
| 12 | MÍNIMO fila / columna / rango | `findMinRow`, `findMinColumn`, `minRange` |
| 13 | Visualizar celdas ocupadas | `printOccupiedCells` / `getAllNodes` |
| 14 | Visualizar fila | `printRow(r)` |
| 15 | Visualizar columna | `printColumn(c)` |

---

## Justificación de la estructura de datos

### ¿Por qué lista enlazada cruzada?

Una hoja de cálculo real puede tener millones de celdas, pero en la práctica
solo una fracción pequeña contiene datos. Una matriz densa de 1000×1000
requiere 1 000 000 entradas en memoria aunque haya 50 celdas ocupadas.
La lista enlazada cruzada almacena **solo** los nodos con contenido.

### Complejidad de operaciones

| Operación | Lista dispersa | Matriz densa |
|-----------|---------------|--------------|
| Insertar celda | O(k_r + k_c) | O(1) |
| Buscar celda | O(k_r) | O(1) |
| Eliminar celda | O(k_r + k_c) | O(1) |
| Eliminar fila | O(k_r · k̄_c) | O(m) |
| SUMA fila | O(k_r) | O(m) |
| SUMA rango | O(nz_range) | O((r2-r1)·(c2-c1)) |

Donde k_r = nodos en la fila, k_c = nodos en la columna, nz_range = celdas no
vacías en el rango. Para hojas dispersas (< 1 % ocupación) todas las
operaciones son órdenes de magnitud más rápidas que la versión densa.

### Uso de memoria

- Matriz densa 1000×1000 con int (4 bytes): **4 MB**
- Lista dispersa con 500 celdas: **500 × ~64 bytes ≈ 32 KB**

---

## Casos borde manejados

- Consultar o eliminar celda inexistente → retorna `""` / no-op, sin crash.
- Eliminar fila/columna vacía → no-op.
- Agregar valor de texto a celdas → se almacena; las funciones de agregación
  lo omiten en lugar de lanzar excepción (`isNumeric` + `toDouble` con try/catch).
- Insertar en celda ya ocupada → actualiza el valor sin crear nodo duplicado.
- Rango vacío en funciones de agregación → retorna 0.0.
