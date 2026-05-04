#ifndef HEADER_H
#define HEADER_H

#include "Node.h"

// Una sola estructura para cabeceras de fila y de columna.
// Antes existían RowHeader y ColHeader como clases separadas, pero
// su estructura era idéntica — solo diferían en el nombre del campo
// (row vs col). Se unificaron en Header<index> para evitar duplicación.
// Se instancian dos listas: una para filas (index = fila) y otra
// para columnas (index = columna).

class Header {
public:
    int     index;   // fila O columna según la lista en que vive
    Node*   first;   // primer nodo de esta fila/columna
    Header* next;    // siguiente cabecera en la lista enlazada

    explicit Header(int idx)
        : index(idx), first(nullptr), next(nullptr) {}
};

#endif // HEADER_H
