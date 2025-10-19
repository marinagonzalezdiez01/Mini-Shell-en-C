# Nombre del ejecutable
TARGET = p2

# Compilador
CC = gcc

# Opciones del compilador
CFLAGS = -Wall -g

# Archivos fuente y cabeceras
SRCS = p2.c commands.c list.c memList.c procList.c
HEADERS = p2.h commands.h list.h memList.h procList.h

# Objetos a generar
OBJS = $(SRCS:.c=.o)

# Regla principal, compilar todo
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regla para compilar archivos .o a partir de .c
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar los archivos generados
clean:
	rm -f $(OBJS) $(TARGET)