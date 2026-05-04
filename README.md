# Proyecto 1 — Hoja de Cálculo con Matriz Dispersa

**Curso:** Algoritmos y Estructuras de Datos  
**Lenguaje:** C++17 &nbsp;|&nbsp; **GUI:** Qt6 (fallback Qt5)  
**Integrantes:** _(completar con nombres del grupo)_

---

## Estructura de archivos

```
include/
  Node.h          — Nodo: (row, col, value, nextInRow, nextInCol)
  Header.h        — Cabecera unificada para filas y columnas (ver §Justificación)
  SparseMatrix.h  — Declaración completa de la clase

src/
  SparseMatrix.cpp  — Implementación de todas las operaciones
  main.cpp          — Demo de consola (prueba todas las operaciones y casos borde)
  main_gui.cpp      — Punto de entrada de la interfaz gráfica
  mainwindow.h      — Declaración de la ventana Qt
  mainwindow.cpp    — Lógica completa de la interfaz (sin archivo .ui)

CMakeLists.txt
README.md
```

> **Nota:** Las clases `RowHeader` y `ColHeader` que existían anteriormente fueron
> unificadas en una sola clase `Header` (ver §Justificación de la estructura).

---

## Compilación

### Requisitos

| Herramienta        | Versión mínima            |
| ------------------ | ------------------------- |
| CMake              | 3.16                      |
| GCC / Clang / MSVC | C++17                     |
| Qt                 | 6.x (o 5.x como fallback) |

### Ubuntu / Debian

```bash
sudo apt install cmake g++ qt6-base-dev   # o qtbase5-dev para Qt5
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Windows — Qt Creator

1. Abrir `CMakeLists.txt` con Qt Creator.
2. Seleccionar un kit con Qt6 MinGW o MSVC.
3. **Build** → **Build All** (Ctrl+B).

### macOS

```bash
brew install cmake qt
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt)
make -j$(nproc)
```

### Ejecutar

```bash
./build/PROYECTOAED_console   # demo de consola
./build/PROYECTOAED_gui       # interfaz gráfica
```

---

## Uso de la interfaz

La interfaz sigue el paradigma de Excel: **una barra superior horizontal** con todos los controles agrupados por función. No hay paneles flotantes ni barras secundarias.

```
[ Celda: A1 ] [ Valor: ... ] [ Ir ][ Insertar ][ Modificar ][ Elim. celda ][ Consultar ]
  |  [ Rango: A1:C3 ][ Suma ][ Prom. ][ Máx. ][ Mín. ][ Borrar ]
  |  [ Elim. fila ][ Elim. col. ][ Ver fila ][ Ver col. ]
  |  [ Suma ▾ ][ Operar sel. ]  [ Limpiar todo ]
```

| Campo           | Descripción                                                                                                        |
| --------------- | ------------------------------------------------------------------------------------------------------------------ |
| **Celda**       | Coordenada activa, ej. `B3`. También es la celda **destino** de los resultados.                                    |
| **Valor**       | Valor a insertar/modificar.                                                                                        |
| **Rango**       | Acepta `A1:C3` (rectangular) **o** `A1,C3,E5` (no contiguo).                                                       |
| **Operar sel.** | Opera sobre celdas seleccionadas con **Ctrl+Click** en la tabla, o sobre las escritas con comas en el campo Rango. |

### Hoja dinámica

La tabla **crece automáticamente** cuando se escribe cerca del borde: agrega 50 filas o 10 columnas según sea necesario. La `SparseMatrix` subyacente no tiene límite fijo — solo se crean nodos para celdas con contenido.

### Edición directa

Hacer **doble clic** en una celda (o presionar cualquier tecla sobre ella) permite editar el valor directamente en la tabla. Al confirmar (Enter / click fuera), el cambio se propaga automáticamente a la `SparseMatrix`. Borrar el contenido de una celda elimina el nodo correspondiente.

---

## Operaciones implementadas

| #   | Operación                             | Método en `SparseMatrix`                        | Acceso en GUI                       |
| --- | ------------------------------------- | ----------------------------------------------- | ----------------------------------- |
| 1   | Insertar / actualizar celda           | `insertCell(r, c, val)`                         | Botón **Insertar**                  |
| 2   | Consultar celda                       | `getCell(r, c)`                                 | Botón **Consultar** / clic en tabla |
| 3   | Modificar celda sin crear nodo        | `modifyCell(r, c, val)`                         | Botón **Modificar**                 |
| 4   | Eliminar celda                        | `deleteCell(r, c)`                              | Botón **Elim. celda**               |
| 5   | Eliminar fila completa                | `deleteRow(r)`                                  | Botón **Elim. fila**                |
| 6   | Eliminar columna completa             | `deleteColumn(c)`                               | Botón **Elim. col.**                |
| 7   | Eliminar rango rectangular            | `deleteRange(r1,c1,r2,c2)`                      | Botón **Borrar**                    |
| 8   | SUMA fila / columna                   | `sumRow` / `sumColumn`                          | Combo agregación                    |
| 9   | SUMA rango / selección                | `sumRange` / `operarSobreCeldas`                | Botón **Suma**                      |
| 10  | PROMEDIO fila / columna / rango       | `averageRow` / `averageColumn` / `averageRange` | Botón **Prom.**                     |
| 11  | MÁXIMO fila / columna / rango         | `findMaxRow` / `findMaxColumn` / `maxRange`     | Botón **Máx.**                      |
| 12  | MÍNIMO fila / columna / rango         | `findMinRow` / `findMinColumn` / `minRange`     | Botón **Mín.**                      |
| 13  | Visualizar celdas ocupadas            | `getAllNodes()`                                 | Status bar + edición directa        |
| 14  | Visualizar fila en orden              | `printRow(r)`                                   | Botón **Ver fila**                  |
| 15  | Visualizar columna en orden           | `printColumn(c)`                                | Botón **Ver col.**                  |
| 16  | Operaciones sobre celdas no contiguas | `operarSobreCeldas`                             | **Ctrl+Click** o `A1,C3,E5`         |

---

## Justificación de la estructura de datos

### Lista enlazada cruzada

Una hoja de cálculo puede contener millones de celdas, pero en la práctica
solo una fracción pequeña tiene contenido. Una matriz densa de 1000×1000
requiere **1 000 000 entradas** en memoria aunque solo haya 50 celdas ocupadas.

La **lista enlazada cruzada** almacena únicamente los nodos con contenido.
Cada nodo tiene dos punteros: `nextInRow` (siguiente en su fila) y `nextInCol`
(siguiente en su columna), permitiendo recorrer eficientemente tanto filas como columnas.

### Unificación de `RowHeader` y `ColHeader` → `Header`

Las clases `RowHeader` y `ColHeader` originales eran **estructuralmente idénticas**:
ambas tenían un entero de índice (`row` / `col`), un puntero `first` al primer nodo,
y un puntero `next` a la siguiente cabecera. La única diferencia era el nombre del campo.

Se unificaron en una sola clase `Header(int index)` y se instancian dos listas:

- `rowHeaders` — índice = fila
- `colHeaders` — índice = columna

Esto elimina duplicación de código, facilita el mantenimiento y demuestra principio DRY.

### Complejidad de operaciones

| Operación                | Lista dispersa |    Matriz densa    |
| ------------------------ | :------------: | :----------------: |
| Insertar celda           |  O(k_r + k_c)  |        O(1)        |
| Buscar / consultar celda |     O(k_r)     |        O(1)        |
| Eliminar celda           |  O(k_r + k_c)  |        O(1)        |
| Eliminar fila            |     O(k_r)     |        O(m)        |
| Eliminar columna         |     O(k_c)     |        O(n)        |
| SUMA / PROMEDIO fila     |     O(k_r)     |        O(m)        |
| SUMA rango               |     O(nz)      | O((r2−r1)·(c2−c1)) |

Donde **k_r**, **k_c** = nodos en la fila/columna (≪ dimensiones totales para hojas dispersas),
**nz** = celdas no vacías en el rango, **m**, **n** = dimensiones totales.

### Uso de memoria

| Configuración                                  | Memoria     |
| ---------------------------------------------- | ----------- |
| Matriz densa 1000×1000 (`int`, 4 bytes)        | **4 MB**    |
| Lista dispersa con 500 celdas (~64 bytes/nodo) | **≈ 32 KB** |

La lista dispersa usa aproximadamente **125× menos memoria** en este escenario.
Para hojas con menos del 1% de ocupación, las operaciones de fila/columna son
también órdenes de magnitud más rápidas que la versión densa.

---

## Casos borde manejados

| Caso                                 | Comportamiento                                        |
| ------------------------------------ | ----------------------------------------------------- |
| Consultar celda inexistente          | Retorna `""`, sin crash                               |
| Eliminar celda inexistente           | No-op, sin crash                                      |
| Eliminar fila / columna vacía        | No-op, sin crash                                      |
| Modificar celda vacía                | Avisa al usuario que use Insertar                     |
| Valor de texto en celda              | Se almacena; las agregaciones lo omiten (`isNumeric`) |
| Insertar en celda ya ocupada         | Actualiza el valor sin crear nodo duplicado           |
| Rango sin celdas numéricas           | Las funciones de agregación retornan `0.0`            |
| Coordenada fuera de límites          | La UI valida y muestra mensaje en la barra de estado  |
| Escribir cerca del borde de la tabla | La hoja crece automáticamente                         |
