Mini Shell en C

Este proyecto implementa un interprete de comandos en lenguaje C, que permite al usuario ejecutar diferentes operaciones del sistema, gestionar procesos, memoria, archivos y directorios.  

La shell incluye un conjunto de comandos propios que simulan o amplían funcionalidades del sistema operativo, y un sistema de ayuda interno (`help`) para consultar el uso de cada uno.

---

Funcionalidades principales

Gestión de memoria
- `allocate` / `deallocate`: reserva y libera memoria (malloc, mmap, memoria compartida).
- `memfill` / `memdump`: rellena o muestra contenido de memoria.
- `memory`: muestra información de direcciones, bloques y mapas de memoria.

Gestión de archivos y directorios
- `makefile`, `makedir`: creación de archivos y directorios.
- `listfile`, `listdir`, `listrec`, `listrev`: listados detallados de ficheros o directorios.
- `erase`, `delrec`: eliminación de archivos y directorios (incluso recursiva).

Operaciones de sistema
- `pid`, `ppid`, `infosys`: información sobre el proceso actual y el sistema.
- `cd`, `cwd`, `date`: comandos básicos del entorno.
- `open`, `close`, `dup`, `listopen`: manejo de descriptores de archivo.

Lectura y escritura de memoria/archivos
- `readfile`, `writefile`, `read`, `write`: operaciones de lectura y escritura entre ficheros y direcciones de memoria.

Otros comandos útiles
- `historic`: mantiene un historial de comandos.
- `recurse`: prueba recursión y muestra direcciones de variables.
- `help`: muestra la lista de comandos disponibles y cómo usarlos.
- `quit`, `exit`, `bye`: finalizan la shell.

