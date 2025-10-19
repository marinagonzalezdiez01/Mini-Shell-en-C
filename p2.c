/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * -- Raquel Varela Romani: raquel.varela.romani 
 * -- Marina Gonzalez Diez: marina.gonzalez1
 */
#include "p2.h"
extern char **environ; // accede a la variable global environ

int TrocearCadena(char *cadena, char *argumentos[]){
     /* Trocea el comando en trozos (cadenas de caracteres hasta llegar a un espacio)
     * y devuelve el numero de trozos
     */
    int i=1;
    if ((argumentos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((argumentos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void actHistoric(char command[MAX_SIZE], tList *L){
    tItem item;
    if(command[0] == ('\n')){
        return;
    }
    if(isEmptyList(*L))
        item.pos = 1;
    else
        item.pos = getItem(last(*L),*L).pos + 1;
    strcpy(item.com,command);
    insertItem(item, L);

}

bool procesarEntrada(char command[MAX_SIZE], int *contador, tList historic, tList *fileList, tListM *memList,tListP *procList, char *envp[], char *environ[]){
    char *args[10];
    int trozos = TrocearCadena(command,args);
    ++*contador;
    bool finish = false;
    if(trozos > 0){
        if (strcmp(args[0], "authors") == 0) {
            command_authors(args[1], trozos);
        } else if (strcmp(args[0], "pid") == 0) {
            if(trozos > 1){
                perror("Error: pid. Comando invalido, no necesita argumentos.\n");
            } else {
                command_pid();
            }
        } else if (strcmp(args[0], "ppid") == 0) {
            if(trozos > 1){
                perror("Error: ppid. Comando invalido, no necesita argumentos.\n");
            } else {
                command_ppid();
            }
        } else if (strcmp(args[0], "cd") == 0) {
            command_cdDir(args[1], trozos);
        } else if (strcmp(args[0], "date") == 0) {
            if(trozos == 1){
                perror("Error: date [-d|-t]. El comando necesita un argumento\n");
            } else {
                command_date(args[1], trozos); 
            }
        } else if (strcmp(args[0], "historic") == 0) {
            command_historic(args[1], trozos, historic);
        } else if(strcmp(args[0], "listopen") == 0){
            command_listOpen(*fileList);
        } else if (strcmp(args[0], "open") == 0) {
            command_open(args[1], args[2], trozos, fileList);
        } else if (strcmp(args[0], "close") == 0) {
            command_close(args[1], trozos, fileList); 
        } else if (strcmp(args[0], "dup") == 0) {
            command_dup(args[1], trozos, fileList);
        } else if (strcmp(args[0], "infosys") == 0) {
            if(trozos > 1){
                perror("Error: infosys. Comando invalido, no necesita argumentos.\n");
            } else {
                command_infosys();
            }
        } else if (strcmp(args[0], "help") == 0) {
            command_help(args[1], trozos);
        } else if (strcmp(args[0], "makefile") == 0) {
            command_makeFile(trozos, args[1]);
        } else if(strcmp(args[0], "listfile") == 0){
            command_list(trozos, args); 
        } else if (strcmp(args[0], "makedir") == 0) {
            command_makeDir(trozos, args[1]);
        } else if (strcmp(args[0], "cwd") == 0) {
            cwd();
        } else if (strcmp(args[0], "listdir") == 0) {
            command_list(trozos, args);    
        } else if (strcmp(args[0], "erase") == 0) {
            command_erase(trozos, args);
        } else if (strcmp(args[0], "delrec") == 0) {
            command_delrec(trozos, args);    
        } else if (strcmp(args[0], "listrev") == 0) {
            command_revlist(trozos,args);
        } else if (strcmp(args[0], "listrec") == 0) {
            command_reclist(trozos,args);  
        } else if (strcmp(args[0], "allocate") == 0) {
            command_allocate(args, trozos, memList);
        } else if(strcmp(args[0], "deallocate") == 0) {
            command_deallocate(args,trozos, memList);
        } else if(strcmp(args[0], "memfill") == 0) {
            if (trozos < 4) {
                fprintf(stderr, "Error: Uso correcto: memfill <direccion> <bytes> <valor>\n");
            } else {
                //Convertir el string a direccion de memoria
                void *p = (void *)strtoull(args[1], NULL, 16);
                //Convertir el string a size_t
                size_t cont = (size_t)strtol(args[2], NULL, 10);
                //Convertir el string a byte
                unsigned char byte = (unsigned char)strtoul(args[3], NULL, 16);
                if (cont == 0) {
                    fprintf(stderr, "Error: El número de bytes debe ser mayor que 0.\n");
                } else {
                    command_memfill(p, cont, byte);
                }
            }
        } else if(strcmp(args[0], "memdump") == 0) {
            if(trozos < 3){
                fprintf(stderr, "Error: Uso correcto: memdump <direccion> <bytes>\n");
            } else {
                // Convertir el string a direccion de memoria
                void *p = (void *)strtoull(args[1], NULL, 16);
                //Convertir el string a entero
                int nbytes = (int)strtol(args[2], NULL, 10);
                command_memdump(p, nbytes);
            }
        } else if(strcmp(args[0], "readfile") == 0) {
            command_readfile(args, trozos);
        } else if (strcmp(args[0], "writefile") == 0) {
            command_writefile(args, trozos);
        }else if (strcmp(args[0], "memory") == 0) {
            command_memory(args, trozos, memList);
        } else if(strcmp(args[0], "read") == 0) {
            command_read(args, trozos);
        } else if(strcmp(args[0], "write") == 0) {
            command_write(args, trozos);
        } else if(strcmp(args[0], "recurse") == 0 || strcmp(args[0], "recursiva") == 0 ) {
            command_recurse(args, trozos);
        } else if(strcmp(args[0], "getuid") == 0){
            command_getuid();
        } else if(strcmp(args[0], "setuid") == 0) {
            command_setuid(args, trozos);
        } else if(strcmp(args[0], "showvar") == 0) {
            command_showvar(args, trozos, envp);
        }else if(strcmp(args[0], "changevar") == 0) {
            command_changevar(args, trozos, envp,environ);
        }else if(strcmp(args[0], "subsvar") == 0) {
            command_subsvar(args, trozos, envp,environ);
        }else if(strcmp(args[0], "environ") == 0) {
            command_environ(args, trozos, envp,environ);
        }else if(strcmp(args[0], "fork") == 0) {
            command_fork(procList);
        } else if(strcmp(args[0], "listjobs") == 0) {
            command_listjobs(procList);
        } else if(strcmp(args[0], "deljobs") == 0) {
            command_deljobs(args,trozos,procList);
        }  else if ((strcmp(args[0], "quit") == 0) || (strcmp(args[0], "exit") == 0) || (strcmp(args[0], "bye") == 0)) {
            if(trozos > 1){
                perror("Error: Comando invalido, no necesita argumentos.\n");
            } else {
                finish = true;  
            }
        } else {
            perror("Error: Comando invalido.\n");
        }
    }
    return finish;
}


int main(int argc, char *argv[], char *envp[]) {
    char command[MAX_SIZE];  //Cadena para almacenar los comandos que se lean desde el teclado
    int contador;            //Para contar cuantas veces se ha ejecutado la funcion procesarEntrada (para evitar bucles infinitos)
    tList historic;          //Lista para almacenar los comandos
    tList fileList;          //Lista para almacenar los archivos relacionados con los comandos
    tListM memList;
    tListP procList;
    bool terminado = false;

    createEmptyList(&historic);
    createEmptyList(&fileList);
    createEmptyListM(&memList);
    createEmptyListP(&procList);
    tItem newFile;
    //Creamos un nuevo fichero y le reservamos los valores de descriptor 0 1 y 2, le asignamos las salidas como nombre y el modo de apertura, y lo insertamos a la lista de ficheros.
    newFile.pos=0;
    strcpy(newFile.com,"entrada estandar");
    insertItem(newFile,&fileList);

    newFile.pos=1;
    strcpy(newFile.com,"salida estandar");
    insertItem(newFile,&fileList);

    newFile.pos=2;
    strcpy(newFile.com,"error estandar");
    insertItem(newFile,&fileList);

    while (!terminado){
        contador = 0;
        printf("-->");
        fgets(command, sizeof(command), stdin);
        actHistoric(command,&historic);
        terminado = procesarEntrada(command, &contador, historic, &fileList, &memList, &procList, envp, environ);
    }

    deleteList(&historic);
    deleteList(&fileList);
    deleteListM(&memList);
    deleteListP(&procList);

    return 0;
}
