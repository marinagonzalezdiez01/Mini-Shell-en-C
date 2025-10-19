/*
 * --- PŔACTICA SISTEMAS OPERATIVOS ---
 * - Raquel Varela Romani -- Marina Gonzalez Diez
 */
#include "commands.h"

int globalVar1 = 10;
int globalVar2 = 10;
int globalVar3 = 10;
int globalVarUninit1;
int globalVarUninit2;
int globalVarUninit3;
static int staticVar1 = 20;
static int staticVar2 = 20;
static int staticVar3 = 20;
static int staticVarUninit1;
static int staticVarUninit2;
static int staticVarUninit3;
extern char **environ; //Accede a la variable global environ

void command_authors(char option[MAX_SIZE], int trozos){
    /*
     * authors: Imprime los nombres y los nombres de usuario de los autores del programa
     * [-l] : Imprime los nombres de usuario
     * [-n] : Imprime los nombres
     */
    if(trozos == 1){
        puts("Raquel Varela Romani: raquel.varela.romani");
        puts("Marina Gonzalez Diez: marina.gonzalez1");
    } else{
        if(strcmp(option,"-l") == 0){
            puts("raquel.varela.romani");
            puts("marina.gonzalez1");
        } else if (strcmp(option,"-n") == 0) {
            puts("Raquel Varela Romani");
            puts("Marina Gonzalez Diez");
        } else {
            perror("Error: Argumento no valido\n");
        }
    }
}

void command_pid(){
    pid_t pid = getpid();
    printf("Pid de shell: %d\n",pid);
}

void command_ppid(){
    pid_t pid = getppid();
    printf("Pid del padre del shell: %d \n", pid);
}

void command_cdDir(char option[MAX_SIZE], int trozos){
    char cwd[PATH_MAX];
    if(trozos==1){
        if(getcwd(cwd,sizeof (cwd))!=LNULL){
            printf("Directorio %s\n",cwd);
        } else perror("Error al obtener el directorio actual\n");
    }else{
        if(chdir(option)!=0){
            perror("Imposible cambiar de directorio\n");
        }else{
            if(getcwd(cwd,sizeof (cwd))!=LNULL){
                printf("Directorio cambiado a %s\n",cwd);
            }else{
                perror("Error al obtener el directorio actual tras el cambio\n");
            }
        }
    }
}


void command_date(char option[MAX_SIZE], int trozos){
    /*
     * opcion = -d (date): Imprime por pantalla la fecha actual en formato DD/MM/YYYY
     * opcion = -t (time): Imprime por pantalla la hora actual en formato HH:MM:SS
     */
    time_t currentTime;
    struct tm *tm;
    char hora[MAX_SIZE];
    char fecha[MAX_SIZE];

    time(&currentTime); //obtiene la fecha en segundos
    tm = localtime(&currentTime); //convierte la fecha
    strftime(hora, MAX_SIZE, "%H:%M:%S",tm);
    strftime(fecha, MAX_SIZE, "%d/%m/%Y",tm);

    if(trozos == 1){
        printf("%s\n",hora);
        printf("%s\n",fecha);
    } else if(strcmp(option, "-d") == 0){
        printf("%s\n",fecha);
    } else if(strcmp(option, "-t") == 0) {
        printf("%s\n",hora);
    } else {
        perror("Error: date [-d|-t]. Argumento no valido.\n");
    }
    
}

void command_historic(char option[MAX_SIZE], int trozos, tList historic){
    tPos pos;
    tItem item;
    if (trozos==1){
        pos=first(historic);
        while(pos!=LNULL){
            item=getItem(pos,historic);
            printf("%d -> %s", item.pos,item.com);
            pos=next(pos,historic);
        }
    }else if(isdigit(option[0])){ // Comprobamos si la opción es un número
        int N = atoi(option);
        if (N <= 0 || N > listSize(historic)){
            printf("Error: No existe ese comando\n");
        } else {
            pos = first(historic);
            for (int i = 1; i < N && pos != LNULL; ++i) {
                pos = next(pos, historic);
            }
            if (pos != LNULL) { 
                item = getItem(pos, historic);
                printf("%d -> %s", item.pos, item.com); 
            }
        }

    }else if(option[0]=='-'){
        pos = previous(last(historic), historic);
        int N = atoi(option+1);
        if (N <= 0) {
            printf("Error: N debe ser positivo\n");
        } else if (N >= listSize(historic)) {
            printf("Error: No hay suficientes comandos en el historial\n");
        } else {
            // Iniciar en el penúltimo comando (ignorando el que ejecuta la función)
            pos = previous(last(historic), historic);

            // Retroceder exactamente N comandos (N-1 en realidad porque ya estamos en el penúltimo)
            for (int i = 1; i < N && pos != LNULL; i++) {
                pos = previous(pos, historic);
            }

            // Imprimir los N comandos anteriores
            for (; pos != last(historic); pos = next(pos, historic)) {
                item = getItem(pos, historic);
                printf("%d -> %s", item.pos, item.com);  // Imprimir con salto de línea
            }
        }
    } else {
        printf("Error: Opción no válida\n");
    }
}

void command_listOpen(tList fileList){
    // Iterar sobre fileList y mostrar cada elemento
    for (tPos i = first(fileList); i != LNULL; i = next(i, fileList)) { // Asumiendo que fileList tiene un tamaño
        tItem item = getItem(i, fileList); // Obtener el elemento i de fileList
        printf("Descriptor: %d, Archivo: %s\n", item.pos, item.com);

        // Obtener el modo de apertura usando fcntl
        int modo = fcntl(item.pos, F_GETFL);

        // Determinar y mostrar el modo de apertura
        if (modo & O_CREAT) printf("O_CREAT\n");
        else if (modo & O_APPEND) printf("O_APPEND\n");
        else if (modo & O_EXCL) printf("O_EXCL\n");
        else if (modo & O_RDWR) printf("O_RDWR\n");
        else if (modo & O_WRONLY) printf("O_WRONLY\n");
        else if (modo & O_TRUNC) printf("O_TRUNC\n");
        else printf("O_RDONLY\n");
    }
}

void command_open(char file[MAX_SIZE], char option[MAX_SIZE], int trozos, tList *fileList){
    /*
     * Abre un archivo y lo agrega (junto con el descriptor del archivo y el
     * modo de apertura a la lista de archivos abiertos del shell.
     */
    int i, fd, mode = 0;
    tItem item;
    char ruta[MAX_SIZE] = "";

    if (trozos == 1) {
        command_listOpen(*fileList);
    } else{
        // obtenemos la ruta del directorio actual
        if(file[0] != '/'){     //Comprueba si es una ruta absoluta
            if (getcwd(ruta, sizeof(ruta)) == NULL) {  //obtiene la ruta del directorio actual y lo guarda en la variable ruta
                perror("Error al obtener el directorio actual\n");
            }
            strcat(ruta, "/");   //Añade una barra diagonal al final d ela ruta actual almacenada en ruta
            strcat(ruta, file);  //Añade el nombre del archivo a la ruta completa
        } else{
            strcat(ruta, file);  //Si es una ruta absoluta, se añade el nombre del archivo a la ruta.
        }

        printf("%s\n",ruta);

        if(trozos >= 3){
            for (i = 3; i <= trozos ; ++i) {
                if (!strcmp(option, "cr")) mode |= O_CREAT;
                else if (!strcmp(option, "ex")) mode |= O_EXCL;
                else if (!strcmp(option, "ro")) mode |= O_RDONLY;
                else if (!strcmp(option, "wo")) mode |= O_WRONLY;
                else if (!strcmp(option, "rw")) mode |= O_RDWR;
                else if (!strcmp(option, "ap")) mode |= O_APPEND;
                else if (!strcmp(option, "tr")) mode |= O_TRUNC;
            }

        }
        if ((fd = open(ruta, mode, 0777)) == -1)
            perror("Imposible abrir fichero\n");
        else {
            item.pos = fd;  //Si open tiene éxito, se copia el descriptor del archivo en item.pos
            strcpy(item.com,ruta);  //La ruta se copia en item.com
            insertItem(item,fileList); //Agrega la entrada a la lista de archivos abiertos
            printf("Anadida entrada %d a la tabla ficheros abiertos\n", fd);
        }
    }
}

void command_close(char option[MAX_SIZE], int trozos, tList *fileList){
    long fd;
    tItem item;
    tPos pos;
    char *end;

    if (trozos == 1) {
        command_listOpen(*fileList);  // Mostrar la lista de archivos abiertos
    } else {
        // Verificación de la conversión a número
        fd = strtol(option, &end, 10);
        if (*end != '\0' || fd <= 0) {
            printf("Error: Descriptor de archivo inválido\n");
            return;
        }

        // Intentar cerrar el archivo
        if (close((int)fd) == -1) {
            perror("Imposible cerrar descriptor\n");
        } else {
            item.pos = (int)fd;

            // Encontrar el elemento en la lista
            pos = findItem(item, *fileList);
            if (pos == LNULL) {
                perror("Error: Descriptor de archivo no encontrado en la lista\n");
            } else {
                // Eliminar el elemento si existe
                deleteAtPosition(pos, fileList);
                printf("Descriptor %ld cerrado y eliminado de la lista\n", fd);
            }
        }
    }
}

void command_dup(char option[MAX_SIZE], int trozos, tList *fileList){
    /*
      dup fd : Duplica el descriptor de fichero fd y añade una nueva entrada a la lista ficheros abiertos
     */
    int fd;
    tItem item, dupFile;
    char aux[MAX_BUFFER], *p;

    if(trozos == 1){
        command_listOpen(*fileList);
    } else{
        fd = dup(atoi(option));
        if(fd == -1){
            perror("No se pudo duplicar el archivo\n");
        } else{
            item.pos = atoi(option);    //Asignamos el descriptor original a item.pos
            dupFile = getItem(findItem(item,*fileList),*fileList);
            p = dupFile.com;        //p apunta a la cadena que contiene el nombre del archivo asociado con el descriptor original
            sprintf (aux,"dup %d (%s)\n",fd, p);  //Se almacena en aux una cadena que contiene el descriptor nuevo y el nombre del archivo original
            item.pos = fd;      //Ponemos el nuevo descriptor
            strcpy(item.com,aux);  //Copiamos la descripcion
            insertItem(item,fileList); //Inserta el nuevo item en fileList
        }
    }
}

void command_infosys(){
    struct utsname info;
    if(uname(&info)!=0){
        perror("Error al obtener información\n");
    }
    printf("Sistema operativo : %s\n",info.sysname);
    printf("Nombre del nodo de red: %s\n",info.nodename);
    printf("Versión del sistema operativo: %s\n",info.release);
    printf("Versión del kernel: %s\n", info.version);
    printf("Arquitectura del hardware: %s\n", info.machine);
}

void command_help(char option[MAX_SIZE], int trozos){
    /*
     * help : Muestra una lista de comandos disponibles.
     * [cmd] : Muestra información de como usar el comando "cmd".
     */
    if(trozos == 1){
        puts("'help [cmd]' ayuda sobre comandos");
        puts("\tComandos disponibles:");
        printf("authors pid ppid cd date historic listopen open\n");
        printf("close dup infosys help makefile makedir listfile\n"
                "listfile cwd listdir listrec listrev erase delrec\n"
                "allocate deallocate memfill memdump memory readfile\n"
                "writefile read write recurse\n"
                "quit exit bye\n");

    } else{
        //P0
        if(strcmp(option,"authors") == 0) printf("authors [-l|-n]: Prints the names and logins of the program authors.\n");
        if(strcmp(option,"pid") == 0) printf("pid: Prints the pid of the process executing the shell.\n");
        if(strcmp(option,"ppid") == 0) printf("ppid: Prints the pid of the shell's parent process.\n");
        if(strcmp(option,"cd") == 0) printf("cd [dir]: Changes the current working directory of the shell to dir.\n");
        if(strcmp(option,"date") == 0) printf("date[-d|-t]: Prints the current date in the format DD/MM/YYYY or in the format hh:mm:ss\n");
        if(strcmp(option,"historic") == 0){
            puts("historic [N|-N]: Shows/clears the historic of commands executed by this shell.");
            puts("\t -historic: Prints all the commands that have been input with their order number");
            puts("\t -historic -N: Prints only the last N commands");
            puts("\t -historic N: Repeats command number N (from historic list)\n");
        }
        if(strcmp(option,"open") == 0) printf("open [file] mode: Opens a file and adds it to the list of shell open files.\n");
        if(strcmp(option,"close") == 0) printf("close [df]: Closes the df file descriptor and eliminates the corresponding item from the list.\n");
        if(strcmp(option,"dup") == 0) printf("dup [df]: Duplicates the df file descriptor.\n");
        if(strcmp(option,"listopen") == 0) printf("listopen: Lists the shell open files.\n");
        if(strcmp(option,"infosys") == 0) printf("infosys: Prints information on the machine running the shell.\n");
        if(strcmp(option,"help") == 0) printf("help [cmd]: Displays a list of available commands or gives a brief help on the usage of a command.\n");
        //P1
        if(strcmp(option,"makefile") == 0) printf("makefile [name]: creates a file.\n");                                                  
        if(strcmp(option,"makedir") == 0) printf("makedir [name]: creates a directory.\n");                                               
        if(strcmp(option,"listfile") == 0) {                                                                                              
            puts("listfile [-long][-link][-acc] name1 name2 ..	lists files;");
            puts("\t -long: long list");
            puts("\t -acc: accesstime");
	        puts("\t -link: if it is a symbolic link, the contained path");    
        }                                                              
        if(strcmp(option,"cwd") == 0) printf("cwd: prints current working directory\n");                                                  
        if(strcmp(option,"listdir") == 0) {                                                                                               
            puts("listdir [-hid][-long][-link][-acc] n1 n2 ..	lists directories' content");
            puts("\t -hid: includes hidden files");
            puts("\t -acc: accesstime");
            puts("\t -link: if it is a symbolic link, the contained path\n");
        }
        if(strcmp(option,"listrec") == 0) {                                                                                               
            puts("listrec [-hid][-long][-link][-acc] n1 n2 ..   lists directories recursively (subdirectories after).\n");
            puts("\t -hid: includes hidden files");
            puts("\t -acc: accesstime");
            puts("\t -link: if it is a symbolic link, the contained path\n");
        }
        if(strcmp(option,"listrev") == 0) {                                                                                               
            puts("listrev [-hid][-long][-link][-acc] n1 n2 ..   lists directories recursively (subdirectories before).\n");
            puts("\t -hid: includes hidden files");
            puts("\t -acc: accesstime");
            puts("\t -link: if it is a symbolic link, the contained path\n");
        }
        if(strcmp(option,"erase") == 0) printf("erase [name1 name2 ...]: deletes files and/or empty directories.\n");                     
        if(strcmp(option,"delrec") == 0) printf("delrec [name1 name2 ...]: deletes files and/or non empty directories recursively.\n");   
        if(strcmp(option,"quit") == 0 || strcmp(option,"bye") == 0 ||strcmp(option,"exit") == 0){
            printf("%s: Ends the shell.\n",option);
        }
        //P2
        if(strcmp(option,"allocate") == 0) {                                                                                              
            puts("allocate [-malloc][-mmap][-createshared] [-shared]\n");
            puts("\t -malloc n: Allocates a block of malloc memory of size n bytes. Updates the list memory blocks\n");
            puts("\t -mmap file perm: Maps a fille to memory with permissions perm. Updates the list of memory blocks\n");
	        puts("\t -createshared cl n: if it is a symbolic link, the contained pathCreates a block of shared memory with key cl and size n and attaches it to the process address space. Updates the list of memory blocks\n");    
            puts("\t -shared cl: Attaches block a shared memory to the process address space (the lock must be already created but not necessarily attached to the process space). Updates the list of memory blocks\n");
        } 
        if(strcmp(option,"deallocate") == 0) {                                                                                              
            puts("deallocate [-malloc][-mmap][-shared][-delkey][-addr]\n");
            puts("\t -malloc n: Deallocates a block of malloc memory of size n (provided it has been previously allocated with allocate). Updates the list of memory blocks\n");
            puts("\t -mmap file: Unmaps a file from memory (provided it has been previously mapped).Updates the list of memory blocks\n");
	        puts("\t -shared cl: Detaches a block of shared memory of key cl (provided it has been previously allocated).Updates the list of memory blocks\n");    
            puts("\t -delkey cl: Removes de memory block of key cl from the system.\n");
            puts("\t addr: Deallocates the block with addres addr. (if it is a malloc block, it frees it; if it is a sharedcmemory block, it detaches it….). Updates the list of memory blocks\n");
        } 
        if(strcmp(option,"memfill") == 0) printf("memfill [addr cont ch]: Fills the memory with character ch, starting at address addr, for cont bytes\n");
        if(strcmp(option,"memdump") == 0) printf("memdump [addr cont]: dumps the contents of cont bytes of memory at address addr to the screen. Dumps hex codes, and for printable characters the associated character\n");
        if(strcmp(option,"memory") == 0) {                                                                                              
            puts("memory [-funcs][-vars][-blocks][-all][-pmap]\n");
            puts("\t -funcs: Prints the addresses of 3 program functions and 3 library functions\n");
            puts("\t -vars: Prints the addresses of 3 external, 3 external initialized, 3 static, 3 static initialized and 3 automatic variables\n");
	        puts("\t -blocks: Prints the list of allocated blocks\n");    
            puts("\t -all: Prints all of the above (-funcs, -vars and -blocks)\n");
            puts("\t -pmap: Shows the output of the command pmap for the shell process (vmmap en macos)\n");
        } 
        if(strcmp(option,"readfile") == 0) printf("readfile [file addr cont]: Reads cont bytes of a file into memory address addr\n");
        if(strcmp(option,"writefile") == 0) printf("writefile [file addr cont]: writes to a file cont bytes starting at memory address addr\n");
        if(strcmp(option,"read") == 0) printf("read [df addr cont]: Reads cont bytes of an already opened file descriptor into memory address addr\n");
        if(strcmp(option,"write") == 0) printf("write [df addr cont]: writes to an already opened file descriptor cont bytes starting at memory address addr\n");
        if(strcmp(option,"recurse") == 0) printf("recurse [n]: executes the recursive function n times. This funcion has an automatic array of size 2048. a static array of size 2048 and prints the addresses of both arrays and its parameter (as well as the number o recursion) before calling itself\n");
    }
}

void cwd(){
    /*Muestra el directorio actual*/
    char directory[MAX_SIZE];
    if(getcwd(directory,MAX_SIZE) != NULL)
        printf("%s\n", directory);
}

void command_makeFile(int trozos,char param1[MAX_SIZE]){
    FILE *file;
    if(trozos==1){
        cwd();
    }else{
        if (access(param1, F_OK) == 0) {  // F_OK comprueba si el archivo existe
            perror("Create not possible");
        } else {
            // Intentar abrir el archivo si no existe
            file = fopen(param1, "w");
            if (file == NULL) {  // Verificar si fopen devolvió NULL
                perror("Create not possible");
            } else {
                fclose(file);  // Cerrar el archivo si se abrió correctamente
            }
        }
    }
}

void command_makeDir(int trozos,char param1[MAX_SIZE]){
    if(trozos==1){
        cwd();
    } else{
        if(mkdir(param1,0777)!=0){
            perror("Create not possible");
        }
    }
}


char LetraTF (mode_t m)
{
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */ 
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}

char * ConvierteModo2 (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    
    return permisos;
}

void getName(char * ruta,char * nombre){
    /*
     * Dada una ruta, obtiene el nombre del directorio o fichero actual
     */
    const char *lastSlash = strrchr(ruta, '/');

    if (lastSlash != NULL) {
        // Copia el nombre del archivo (después de la última barra) en nombreArchivo
        strcpy(nombre, lastSlash + 1);
    } else {
        // Si no se encontró una barra, copia la ruta completa en nombreArchivo
        strcpy(nombre, ruta);
    }

}

void longMode(char *directory, bool lnk, bool acc){
    /*
     * Imprime stat en formato largo (se ha introducido como mínimo
     * la opción "-long").
     */
    struct stat stats;
    struct group *grp;
    struct passwd *usr;
    char user[MAX_SIZE], group[MAX_SIZE], symblnk[MAX_SIZE] = "", directoryName[MAX_SIZE];
    ssize_t numBytes, bufferSize;

    getName(directory,directoryName);
    if(lstat(directory,&stats) == 0){
        if(acc){ // fecha
            date(stats.st_atime);
        } else{
            date(stats.st_mtime);
        }
        if((usr = getpwuid(stats.st_uid))!=NULL){ // nombre
            sprintf(user, "%s", usr->pw_name);
        }else{
            sprintf(user, "%d", stats.st_uid);
        }
        if((grp = getgrgid(stats.st_gid))!=NULL){ // grupo
            sprintf(group, "%s", grp->gr_name);
        }else{
            sprintf(group, "%d", stats.st_gid);
        }
        printf("%4hu (%8llu) %8s %8s %s\t%lld%2s", stats.st_nlink, stats.st_ino, user, group,
               ConvierteModo2(stats.st_mode), stats.st_size, directoryName);
        if(lnk == true && S_ISLNK(stats.st_mode)){
            bufferSize = stats.st_size;
            numBytes = readlink(directory,symblnk,bufferSize);
            if(numBytes < 0){
                perror("**** error al acceder");
            } else{
                printf(" -> %s", symblnk);
            }
        }
        puts("");
    } else{
        perror("**** error al acceder");
    }
}

void shortMode(char *directory){
    /*
     * Imprime stat en caso de que no se intruzca ninguna opción
     */
    struct stat stats;
    char directoryName[MAX_SIZE];

    getName(directory, directoryName);

    if(lstat(directory,&stats) == 0){
        printf("%9lld %s\n", stats.st_size, directoryName);
    } else{
        perror("**** error al acceder");
    }
}

void date(time_t timeDir){
    /*
     * Imprime la fecha de un directorio (acceso o modificación)
     */
    struct tm tm;
    tm = *localtime(&timeDir);
    printf("%04d/%02d/%02d-%02d:%02d ", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,  tm.tm_hour, tm.tm_min);
}

void directoryContent(char* param, bool lng, bool acc, bool lnk, bool hid, bool rec, bool rev){
    /*
     * Recorre los directorios y sus subdirectorios.
     */
    DIR * directory = opendir(param);
    struct dirent * directoryEntry;
    char directoryPath[MAX_BUFFER];

    if(directory != NULL) {
        while ((directoryEntry = readdir(directory)) != NULL){
            sprintf(directoryPath, "%s/%s", param, directoryEntry->d_name);
            if (strcmp(directoryEntry->d_name, ".") != 0 && strcmp(directoryEntry->d_name, "..") != 0){
                // se muestra por pantalla el contenido de todos los directorios menos el de los ocultos
                longList(directoryPath,lng,acc,lnk,hid,rec,rev);
            }
        }
        closedir(directory);
    }
}

void longList(char *param, bool lng, bool acc, bool lnk, bool hid, bool rec, bool rev){
    /*
     * Muestra por pantalla el contenido de los directorios.
     */
    DIR * directory = opendir(param);
    struct dirent * directoryEntry;
    char directoryPath[MAX_BUFFER];

    if(directory != NULL) {
        if(rev) directoryContent(param,lng,acc,lnk,hid,false,rev); // si es listrev
        // imprimir contenido
        printf("**********%s\n", param);
        while ((directoryEntry = readdir(directory)) != NULL){
            sprintf(directoryPath, "%s/%s", param, directoryEntry->d_name);
            if((directoryEntry->d_name[0] != '.') || (hid && directoryEntry->d_name[0] == '.')){
                //ignora /. y /.. excepto cuando se incluye -hid
                if(lng){
                    longMode(directoryPath,lnk,acc); //formato largo
                } else{
                    shortMode(directoryPath); //formato corto
                }
            }
        }
        if(rec) directoryContent(param,lng,acc,lnk,hid,rec,false); // si es listrec
        closedir(directory);
    }
}

void command_list(int trozos, char *param1[]){
    
    bool rec = false, rev = false, hid = false, lng = false, acc = false, lnk = false;
    int numOptions = 0, i, dirName = 0;
    struct stat stats;
    char * name;

    if(strcmp(param1[0], "listdir") == 0){
        for (i = 1; i < trozos; ++i){ //recorremos los parametros para obtener las opciones
            if(strcmp(param1[i],"-hid") == 0){
                hid = true;
                ++numOptions;
            } else if(strcmp(param1[i],"-long") == 0){
                lng = true;
                ++numOptions;
            } else if(strcmp(param1[i],"-link") == 0){
                lnk = true;
                ++numOptions;
            } else if(strcmp(param1[i], "-acc") == 0){
                acc = true;
                ++numOptions;
            } else{
                ++dirName;
            }
        }
    } else if ((strcmp(param1[0], "listfile") == 0) || (strcmp(param1[0], "listrec") == 0) || (strcmp(param1[0], "listrev") == 0)) {
        if(strcmp(param1[0], "listrec") == 0){
            rec = true;
        } else if(strcmp(param1[0], "listrev") == 0){
            rev = true;
        }
        for (i = 1; i < trozos; ++i){ //recorremos los parametros para obtener las opciones
            if(strcmp(param1[i],"-long") == 0){
                lng = true;
                ++numOptions;
            } else if(strcmp(param1[i], "-acc") == 0){
                acc = true;
                ++numOptions;
            } else if(strcmp(param1[i],"-link") == 0){
                lnk = true;
                ++numOptions;
            } else{
                ++dirName;
            }
        }
    }

    if(trozos == 1 || dirName == 0){
        cwd();
    } else{
        for(i = 1 + numOptions; i < trozos; ++i){
            name = param1[i];
            if(stat(name,&stats) == 0){
                if(S_ISDIR(stats.st_mode)){ // se trata de un directorio
                    if(strcmp(param1[0], "listfile") == 0){
                        // In case of listfile, also print directory size and name
                        shortMode(param1[i]); // Use shortMode to print size and name
                    } else {
                        longList(param1[i],lng,acc,lnk,hid,rec,rev);
                    }
                } else {
                    if(lng){ // se trata de un fichero
                        // formato largo
                        longMode(param1[i],lnk,acc);
                    } else{
                        // formato corto
                        shortMode(param1[i]);
                    }
                }
            } else{
                perror("No se ha podido abrir");
            }

        }
    }
}

void command_erase(int trozos, char *param1[]){
    /*
     * erase [files] : borra los ficheros/directorios vacíos
     */
    int i;
    char *name;
    struct stat fileStat;

    if(trozos == 1){
        cwd();
    } else{
        for(i = 1; i < trozos; ++i){   //Recorre el comando para recoger los parámetros
            name = param1[i];
            if(stat(name,&fileStat) == 0){  //Si stat devuelve 0, significa que se encontró el archivo/directorio
                if(S_ISDIR(fileStat.st_mode)){  //Verifica si es un directorio
                    if(rmdir(name) != 0){
                        perror("Delete not possible");
                    }
                } else if (S_ISREG(fileStat.st_mode)){  //Si es un archivo regular
                    if(remove(name) != 0){
                        perror("Delete not possible");
                    }
                }
            } else {
                perror("Delete not possible");
            }
        }
    }
}

void notEmptyDirDelete(char directoryName[MAX_SIZE]){
    /*
     * Borra recursivamente los directorios/archivos.
     */
    DIR *directory = opendir(directoryName);
    struct dirent *directoryEntry;
    char directoryEntryPATH[MAX_BUFFER];
    struct stat fileStat;

    if(directory == NULL){
        perror("Error deleting directory");
        return; // Exit if the directory cannot be opened
    } 

    while ((directoryEntry = readdir(directory)) != NULL) {
        // Omit '.' and '..' directories
        if (strcmp(directoryEntry->d_name, ".") != 0 && strcmp(directoryEntry->d_name, "..") != 0) {
            // Create full path for the directory entry
            snprintf(directoryEntryPATH, sizeof(directoryEntryPATH), "%s/%s", directoryName, directoryEntry->d_name);
            
            if (stat(directoryEntryPATH, &fileStat) == 0) {
                if (S_ISDIR(fileStat.st_mode)) {
                    // If it's a directory, recursively delete its contents
                    notEmptyDirDelete(directoryEntryPATH);
                } else if (S_ISREG(fileStat.st_mode)) {
                    // If it's a regular file, delete it
                    if (unlink(directoryEntryPATH) != 0) {
                        perror("Failed to delete file");
                    }
                }
            } else {
                perror("Failed to get file status");
            }
        }
    }

    closedir(directory);

    // After deleting the contents, delete the directory itself
    if (rmdir(directoryName) != 0) {
        perror("Failed to delete directory");
    }
}

void command_delrec(int trozos, char *param1[]){
    /*
     * delrec [name1 name2 ...]: Borra ficheros o directorios no vacíos recursivamente
     */
    int i;
    char *name;
    struct stat fileStat;

    if(trozos == 1){
        cwd();  // Function to display current directory (this is your placeholder)
    } else {
        for (i = 1; i < trozos; ++i){
            name = param1[i];  // Get the directory/file name from the command arguments
            if (stat(name, &fileStat) == 0) {  // Check if the directory/file exists
                if (S_ISDIR(fileStat.st_mode)) {  // If it's a directory, delete recursively
                    notEmptyDirDelete(name);
                } else if (S_ISREG(fileStat.st_mode)) {  // If it's a file, try to delete it
                    if (unlink(name) != 0) {
                        perror("Failed to delete file");
                    }
                }
            } else {
                perror("Failed to access file or directory");
            }
        }
    }
}

void revlist(char *param, bool hid, bool lng, bool acc, bool lnk, int depth) {
    DIR *directory = opendir(param);
    struct dirent *directoryEntry;
    char directoryPath[MAX_BUFFER];
    struct stat stats;
    char *dirNames[MAX_BUFFER];
    int dirCount = 0;

    if (directory == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    // Primera pasada: almacenar subdirectorios y archivos
    while ((directoryEntry = readdir(directory)) != NULL) {
        sprintf(directoryPath, "%s/%s", param, directoryEntry->d_name);

        if (strcmp(directoryEntry->d_name, ".") == 0 || strcmp(directoryEntry->d_name, "..") == 0) {
            if (hid) {
                if (stat(directoryPath, &stats) == 0) {
                    printf("%8lld  %s\n", (long long)stats.st_size, directoryEntry->d_name);
                }
            }
            continue;
        }

        if (directoryEntry->d_name[0] == '.' && !hid) {
            continue;
        }

        if (stat(directoryPath, &stats) == 0) {
            if (S_ISDIR(stats.st_mode)) {
                dirNames[dirCount] = strdup(directoryEntry->d_name);
                dirCount++;
            }
        }
    }

    // Pasada recursiva: procesar subdirectorios
    for (int i = 0; i < dirCount; i++) {
        sprintf(directoryPath, "%s/%s", param, dirNames[i]);
        reclist(directoryPath, hid, lng, acc, lnk, depth + 1);
        free(dirNames[i]);
    }

    rewinddir(directory);

    printf("************%s\n", param);

    // Segunda pasada: listar archivos en el directorio actual
    while ((directoryEntry = readdir(directory)) != NULL) {
        sprintf(directoryPath, "%s/%s", param, directoryEntry->d_name);

        // Omitir "." y ".." si no se permite '-hid'
        if (strcmp(directoryEntry->d_name, ".") == 0 || strcmp(directoryEntry->d_name, "..") == 0) {
            if (!hid) continue;
        }

        // Omitir archivos/directorios ocultos si no se permite '-hid'
        if (directoryEntry->d_name[0] == '.' && !hid) {
            continue;
        }

        // Obtener información del archivo o directorio
        if (lstat(directoryPath, &stats) == 0) {
            if (lng) {
                char *permisos = ConvierteModo2(stats.st_mode);
                char timeBuffer[80];
                struct tm *tm_info = localtime(&stats.st_mtime);
                strftime(timeBuffer, sizeof(timeBuffer), "%Y/%m/%d-%H:%M", tm_info);

                printf("%s   %hu (%lld)   %s   %lld %s\n", timeBuffer, stats.st_nlink, (long long)stats.st_size, permisos, (long long)stats.st_size, directoryEntry->d_name);
                if (lnk && S_ISLNK(stats.st_mode)) {
                    char symblnk[MAX_SIZE];
                    ssize_t numBytes = readlink(directoryPath, symblnk, sizeof(symblnk) - 1);
                    if (numBytes >= 0) {
                        symblnk[numBytes] = '\0';
                        printf(" -> %s\n", symblnk);
                    }
                }
            } else {
                printf("%8lld  %s\n", (long long)stats.st_size, directoryEntry->d_name);
            }
        }
    }

    closedir(directory);
}


void command_revlist(int trozos, char *param1[]) {
    bool hid = false, lng = false, acc = false, lnk = false;
    int dirName = 0;

    for (int i = 1; i < trozos; ++i) {
        if (strcmp(param1[i], "-hid") == 0) {
            hid = true;
        } else if (strcmp(param1[i], "-long") == 0) {
            lng = true;
        } else if (strcmp(param1[i], "-acc") == 0) {
            acc = true;
        } else if (strcmp(param1[i], "-link") == 0) {
            lnk = true;
        } else {
            dirName++;
        }
    }

    if (trozos == 1 || dirName == 0) {
        // Mostrar el directorio actual si no se especifican parámetros
        char directory[MAX_SIZE];
        if (getcwd(directory, MAX_SIZE) != NULL) {
            revlist(directory, hid, lng, acc, lnk, 0);
        }
    } else {
        for (int i = 1; i < trozos; ++i) {
            if (param1[i][0] != '-') { // Solo procesar nombres de directorios
                revlist(param1[i], hid, lng, acc, lnk, 0);
            }
        }
    }
}

void reclist(char *param, bool hid, bool lng, bool acc, bool lnk, int depth) {
    DIR *directory = opendir(param);
    struct dirent *directoryEntry;
    char directoryPath[MAX_BUFFER];
    struct stat stats;
    char *dirNames[MAX_BUFFER];
    char *fileNames[MAX_BUFFER];
    int dirCount = 0, fileCount = 0;

    if (directory == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    printf("************%s\n", param);

    // Primera pasada: listar archivos y almacenar subdirectorios
    while ((directoryEntry = readdir(directory)) != NULL) {
        sprintf(directoryPath, "%s/%s", param, directoryEntry->d_name);

        // Mostrar '.' y '..' solo si se permite '-hid'
        if (strcmp(directoryEntry->d_name, ".") == 0 || strcmp(directoryEntry->d_name, "..") == 0) {
            if (hid) {
                if (stat(directoryPath, &stats) == 0) {
                    printf("%8lld  %s\n", (long long)stats.st_size, directoryEntry->d_name);
                }
            }
            continue; // Pasar a la siguiente entrada
        }

        // Ignorar otros archivos ocultos si no está activado '-hid'
        if (directoryEntry->d_name[0] == '.' && !hid) {
            continue; // Saltar archivos/directorios ocultos si no se permite '-hid'
        }

        if (stat(directoryPath, &stats) == 0) {
            if (S_ISDIR(stats.st_mode)) {
                // Almacenar el nombre y tamaño de subdirectorios
                dirNames[dirCount] = strdup(directoryEntry->d_name);
                dirCount++;
            } else {
                // Almacenar archivos
                fileNames[fileCount] = strdup(directoryEntry->d_name);
                fileCount++;
            }
        }
    }

    // Mostrar los subdirectorios
    for (int i = 0; i < dirCount; i++) {
        sprintf(directoryPath, "%s/%s", param, dirNames[i]);
        if (stat(directoryPath, &stats) == 0) {
            if (lng) { // Solo mostrar detalles si lng está activado
                char *permisos = ConvierteModo2(stats.st_mode);
                char timeBuffer[80];
                struct tm *tm_info = localtime(&stats.st_mtime);
                strftime(timeBuffer, sizeof(timeBuffer), "%Y/%m/%d-%H:%M", tm_info);
                
                printf("%s   %hu (%lld)   %s   %lld %s\n", timeBuffer, stats.st_nlink, (long long)stats.st_size, permisos, (long long)stats.st_size, dirNames[i]);
            } else {
                printf("%8lld  %s\n", (long long)stats.st_size, dirNames[i]); // Formato simple
            }
        }
    }

    // Luego mostrar archivos
    for (int i = 0; i < fileCount; i++) {
        sprintf(directoryPath, "%s/%s", param, fileNames[i]);
        if (stat(directoryPath, &stats) == 0) {
            if (lng) { // Solo mostrar detalles si lng está activado
                char *permisos = ConvierteModo2(stats.st_mode);
                char timeBuffer[80];
                struct tm *tm_info = localtime(&stats.st_mtime);
                strftime(timeBuffer, sizeof(timeBuffer), "%Y/%m/%d-%H:%M", tm_info);
                
                printf("%s   %hu (%lld)   %s   %lld %s\n", timeBuffer, stats.st_nlink, (long long)stats.st_size, permisos, (long long)stats.st_size, fileNames[i]);
                if (lnk && S_ISLNK(stats.st_mode)) {
                    char symblnk[MAX_SIZE];
                    ssize_t numBytes = readlink(directoryPath, symblnk, sizeof(symblnk) - 1);
                    if (numBytes >= 0) {
                        symblnk[numBytes] = '\0';
                        printf(" -> %s\n", symblnk);
                    }
                }
            } else {
                printf("%8lld  %s\n", (long long)stats.st_size, fileNames[i]); // Formato simple
            }
        }
        free(fileNames[i]); // Liberar memoria de archivos
    }

    // Procesar recursivamente los subdirectorios almacenados
    for (int i = 0; i < dirCount; i++) {
        sprintf(directoryPath, "%s/%s", param, dirNames[i]);
        reclist(directoryPath, hid, lng, acc, lnk, depth + 1);
        free(dirNames[i]); // Liberar memoria de subdirectorios
    }

    closedir(directory);
}

void command_reclist(int trozos, char *param1[]) {
    bool hid = false, lng = false, acc = false, lnk = false;
    int dirName = 0;

    for (int i = 1; i < trozos; ++i) {
        if (strcmp(param1[i], "-hid") == 0) {
            hid = true;
        } else if (strcmp(param1[i], "-long") == 0) {
            lng = true;
        } else if (strcmp(param1[i], "-acc") == 0) {
            acc = true;
        } else if (strcmp(param1[i], "-link") == 0) {
            lnk = true;
        } else {
            dirName++;
        }
    }

    if (trozos == 1 || dirName == 0) {
        // Mostrar el directorio actual si no se especifican parámetros
        char directory[MAX_SIZE];
        if (getcwd(directory, MAX_SIZE) != NULL) {
            reclist(directory, hid, lng, acc, lnk, 0);
        }
    } else {
        for (int i = 1; i < trozos; ++i) {
            if (param1[i][0] != '-') { // Solo procesar nombres de directorios
                reclist(param1[i], hid, lng, acc, lnk, 0);
            }
        }
    }
    
}


//Practica 2

void recursiva (int n) {
    char automatico[TAMANO];
    static char estatico[TAMANO];

    printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

    if (n>0)
        recursiva(n-1);
}

void *ObtenerMemoriaShmget(key_t clave, size_t tam) { //funcion de ayudap2
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;

    if (tam)     /*tam distito de 0 indica crear */
        flags = flags | IPC_CREAT | IPC_EXCL;
    if (clave == IPC_PRIVATE) { /*no nos vale*/
        errno = EINVAL;
        return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1)
        return (NULL);
    if ((p = shmat(id, NULL, 0)) == (void *) -1) {
        aux = errno;
        if (tam)
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        return (NULL);
    }
    shmctl(id, IPC_STAT, &s);
    return p;
}

void mostrarMemList(tListM M, pid_t pid, cmd tElemento) {
    /*
     * Muestra por pantalla memList
     */
    tPosM pos;
    tItemM item;

    printf("******Lista de bloques asignados malloc para el proceso %d\n", pid);
    if (!isEmptyListM(M)) { // recorre la lista y la imprime
        for (pos = firstM(M); pos <= lastM(M); ++pos) {
            item = getItemM(pos, M);
            switch (tElemento) {
                case MALLOC:
                    if (item.cmdType == tElemento) { // filtra en la lista los elementos por tipo (MALLOC, SHARED, MMAP)
                        printf("\t%p\t\t%zu %s malloc\n", item.dir, item.size, item.date);
                    }
                    break;
                case MMAP:
                    if (item.cmdType == tElemento) { // filtra en la lista los elementos por tipo (MALLOC, SHARED, MMAP)
                        printf("\t%p\t\t%zu %s %s (descriptor %d) \n", item.dir, item.size, item.date,
                               item.Union.file.filename, item.Union.file.fd);
                    }
                    break;
                case SHARED:
                    if (item.cmdType == tElemento) { // filtra en la lista los elementos por tipo (MALLOC, SHARED, MMAP)
                        printf("\t%p\t\t%zu %s shared (key %d)\n", item.dir, item.size, item.date, item.Union.key);
                    }
                    break;
                default:
                    break;

            }

        }
    }
}

void mostrarMemListCompleta(tListM M){
    /*
     * Muestra la lista completa
     */
    tPosM pos;
    tItemM item;
    pid_t pid = getpid();

    printf("******Lista de bloques asignados para el proceso %d\n", pid);
    if(!isEmptyListM(M)){ // recorre la lista sin ningun filtro
        for (pos = firstM(M); pos <= lastM(M); ++pos) {
            item = getItemM(pos, M);
            if (item.cmdType == MALLOC) {
                printf("\t%p\t\t%zu %s malloc\n", item.dir, item.size, item.date);
            }
            if (item.cmdType == MMAP) {
                printf("\t%p\t\t%zu %s %s (descriptor %d) \n", item.dir, item.size, item.date,
                       item.Union.file.filename, item.Union.file.fd);
            }
            if (item.cmdType == SHARED) {
                printf("\t%p\t\t%zu %s shared (key %d)\n", item.dir, item.size, item.date, item.Union.key);
            }
        }
    }
}

void sharedCreate(char *param1[], pid_t pid, tListM *M, int trozos) { // funcion de ayudap2
    key_t cl;
    size_t tam;
    time_t t = time(0);
    struct tm *tlocal = localtime(&t);
    void *p;
    tItemM item;


    if (trozos < 4) { // si no hay suficientes paramentros
        mostrarMemList(*M, pid, SHARED);
    } else {
        cl = (key_t) strtoul(param1[2], NULL, 10);
        tam = (size_t) strtoul(param1[3], NULL, 10);
        if (tam == 0) {
            printf("No se asignan bloques de 0 bytes\n");
            return;
        }
        if ((p = ObtenerMemoriaShmget(cl, tam)) != NULL) { // asignar memoria
            item.dir = p;
            item.size = tam;
            item.Union.key = cl;
            item.cmdType = SHARED;
            strftime(item.date, 128, "%b %d %H:%M", tlocal);
            if (insertItemM(item, M)) {
                printf("Asignados %lu bytes en %p\n", (unsigned long) tam, p);
            } else {
                puts("Error: No se pudo asignar memoria");
            }
        } else {
            printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) cl, strerror(errno));
        }
    }
}

void *MapearFichero(char *fichero, int protection, tListM *M) {
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;
    tItemM item;
    time_t t = time(0);
    struct tm *tlocal = localtime(&t);

    if (protection & PROT_WRITE) modo = O_RDWR;

    // Check if file exists and can be accessed with the specified mode
    if (stat(fichero, &s) == -1) {
        perror("Error: No se pudo obtener información del fichero");
        return NULL;
    }
    if ((df = open(fichero, modo)) == -1) {
        perror("Error: No se pudo abrir el fichero");
        return NULL;
    }

    // Attempt to map the file to memory
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED) {
        perror("Error: No se pudo mapear el fichero");
        close(df); // Close file descriptor on error
        return NULL;
    }

    // Store mapped file details in the list
    item.dir = p;
    item.size = s.st_size;
    item.cmdType = MMAP;
    item.Union.file.fd = df;
    strftime(item.date, 128, "%b %d %H:%M", tlocal);
    strcpy(item.Union.file.filename, fichero);
    insertItemM(item, M);

    return p;
}

void command_allocate(char *param1[], int trozos, tListM *M) {
    pid_t pid = getpid();
    size_t size;
    tItemM item;
    time_t t = time(0);
    struct tm *tlocal = localtime(&t);

    if (trozos == 1) { // Mostrar la lista de bloques asignados
         printf("******Lista de bloques asignados para el proceso %d\n", pid);
    } else if (strcmp(param1[1], "-malloc") == 0) { // Asignar memoria con malloc
        if (trozos < 3 || param1[2] == NULL) {
            mostrarMemList(*M, pid, MALLOC);
            return;
        }
        size = atoi(param1[2]);
        if (size <= 0) {
            printf("No se asignan bloques de 0 bytes\n");
            return;
        }

        item.cmdType = MALLOC;
        item.size = size;
        item.dir = malloc(item.size);
        strftime(item.date, 128, "%b %d %H:%M", tlocal);

        if (item.dir == NULL) { // Error en malloc
            puts("Error: No se pudo reservar memoria");
        } else if (insertItemM(item, M)) { // Insertar en la lista
            printf("Asignados %zu bytes en %p\n", item.size, item.dir);
        } else { // Error al insertar, liberar memoria
            free(item.dir);
            puts("Error: No se pudo insertar en la lista");
        }
    } else if (strcmp(param1[1], "-mmap") == 0) {
        void *p;
        int protection = 0;

        if (trozos < 3) {
            mostrarMemList(*M, pid, MMAP);
            return;
        }

        if (trozos >= 4 && param1[3] != NULL && strlen(param1[3]) < 4) {
            if (strchr(param1[3], 'r') != NULL) protection |= PROT_READ;
            if (strchr(param1[3], 'w') != NULL) protection |= PROT_WRITE;
            if (strchr(param1[3], 'x') != NULL) protection |= PROT_EXEC;
        }

        //Intentar mapear el archivo
        p = MapearFichero(param1[2], protection, M);
        if (p == NULL) {
            perror("Imposible mapear fichero");
        } else {
            printf("Fichero %s mapeado en %p\n", param1[2], p);
        }
    } else if (strcmp(param1[1], "-createshared") == 0) {
        sharedCreate(param1, pid, M, trozos);
    } else if (strcmp(param1[1], "-shared") == 0) {
        if(trozos < 3) {
            mostrarMemList(*M, pid, SHARED);
        } else {
            key_t cl = (key_t)strtoul(param1[2], NULL, 10);
            void *p = ObtenerMemoriaShmget(cl, 0); // Adjuntar memoria compartida
            if (p != NULL) {
                struct shmid_ds s;
                shmctl(shmget(cl, 0, 0), IPC_STAT, &s);  // Obtenemos el tamaño

                item.dir = p;     //Direccion de la memoria compartida
                item.size = s.shm_segsz;
                item.Union.key = cl;
                item.cmdType = SHARED;
                strftime(item.date, 128, "%b %d %H:%M", tlocal);
                if (insertItemM(item, M)) {
                    printf("Bloque de memoria compartida adjuntado en %p\n", p);
                } else {
                    puts("Error: No se pudo adjuntar memoria compartida");
                }
            } else {
                printf("Imposible adjuntar memoria compartida con clave %lu: %s\n", (unsigned long)cl, strerror(errno));
            }
        }
    }
}

void deallocate_malloc(size_t size, tListM *M) {
    tPosM pos;
    tItemM item;

    // Recorrer la lista en busca del bloque malloc con el tamaño especificado
    for (pos = firstM(*M); pos != LNULLM; pos = nextM(pos, *M)) {
        item = getItemM(pos, *M);
        if (item.cmdType == MALLOC && item.size == size) {
            // Liberar la memoria y eliminar el elemento de la lista
            free(item.dir);
            deleteItemM(pos, M);
            printf("Bloque de %zu bytes liberado de %p\n", size, item.dir);
            return;
        }
    }
    printf("No se encontró ningún bloque malloc de %zu bytes\n", size);
}

void deallocate_mmap(char *filename, tListM *M) {
    tPosM pos;
    tItemM item;
    int found = 0;  // Marca si el archivo fue encontrado

    // Verificar si el archivo existe en el sistema antes de intentar operar sobre él
    if (access(filename, F_OK) == -1) {
        printf("Error: El fichero %s no existe en el sistema\n", filename);
        return;
    }

    if (M == NULL) {
        printf("Error: La lista de memoria es NULL\n");
        return;
    }

    // Verificar si la lista está vacía
    if (isEmptyListM(*M)) {
        printf("Error: La lista de memoria está vacía\n");
        return;
    }

    // Recorrer la lista buscando el archivo mapeado
    for (pos = firstM(*M); pos != LNULLM; pos = nextM(pos, *M)) {
        item = getItemM(pos, *M);

        // Comprobar si el tipo de comando es MMAP y si el nombre del archivo coincide
        if (item.cmdType == MMAP && strcmp(item.Union.file.filename, filename) == 0) {
            found = 1;

            // Verificar que la dirección de memoria no sea NULL
            if (item.dir == NULL) {
                printf("Advertencia: El archivo %s no tiene memoria asignada (dir == NULL)\n", filename);
                return;  // O continuar con el siguiente archivo
            }

            // Verificar que la dirección no sea inválida (por ejemplo, menor que 0x1000)
            if ((uintptr_t)item.dir < 0x1000) {
                printf("Advertencia: Dirección inválida para %s: %p\n", filename, item.dir);
                return;
            }

            // Verificar que el tamaño sea válido (mayor que 0)
            if (item.size <= 0) {
                printf("Error: El tamaño de memoria para el archivo %s no es válido (%zu)\n", filename, item.size);
                return;
            }

            // Intentar desmapear el archivo con munmap
            if (munmap(item.dir, item.size) == -1) {
                perror("Error al desmapear el fichero");
                return;
            }

            // Verificar el descriptor de archivo antes de cerrarlo
            if (item.Union.file.fd >= 0) {
                close(item.Union.file.fd);
            } else {
                printf("Error: Descriptor de archivo inválido para %s\n", filename);
            }

            // Eliminar el elemento de la lista de memoria
            deleteItemM(pos, M);
            printf("Fichero %s desmapeado de %p\n", filename, item.dir);
            return;
        }
    }

    // Si no encontramos el archivo mapeado
    if (!found) {
        printf("Error: No se encontró ningún fichero mapeado con el nombre %s\n", filename);
    }
}

void deallocate_shared(key_t key, tListM *M) {
    tPosM pos;
    tItemM item;

    // Recorrer la lista en busca del bloque compartido con la clave especificada
    for (pos = firstM(*M); pos != LNULLM; pos = nextM(pos, *M)) {
        item = getItemM(pos, *M);
        if (item.cmdType == SHARED && item.Union.key == key) {
            // Desadjuntar el bloque compartido y eliminar el elemento de la lista
            if (shmdt(item.dir) == 0) {
                deleteItemM(pos, M);
                printf("Bloque de memoria compartida con clave %d desadjuntado de %p\n", key, item.dir);
            } else {
                perror("Error al desadjuntar el bloque compartido");
            }
            return;
        }
    }
    printf("No se encontró ningún bloque compartido con la clave %d\n", key);
}

void deallocate_delkey(key_t key) {
    // Eliminar el bloque de memoria compartida del sistema
    if (shmctl(shmget(key, 0, 0777), IPC_RMID, NULL) == 0) {
        printf("Bloque de memoria compartida con clave %d eliminado del sistema\n", key);
    } else {
        perror("Error al eliminar el bloque compartido");
    }
}


void deallocate_address(void *address, tListM *M) {
    tPosM pos;
    tItemM item;

    // Verificar que la dirección no sea NULL
    if (address == NULL) {
        printf("Dirección %p no asignada con malloc, shared o mmap\n", address);
        return;
    }

    // Recorrer la lista en busca del bloque con la dirección especificada
    for (pos = firstM(*M); pos != LNULLM; pos = nextM(pos, *M)) {
        item = getItemM(pos, *M);

        // Verificar que la dirección en la lista coincide con la proporcionada
        if (item.dir == address) {
            switch (item.cmdType) {
                case MALLOC:
                    free(item.dir);
                    break;
                case MMAP:
                    if (munmap(item.dir, item.size) != 0) {
                        perror("Error al desmapear");
                        return;
                    }
                    close(item.Union.file.fd);
                    break;
                case SHARED:
                    if (shmdt(item.dir) != 0) {
                        perror("Error al desadjuntar el bloque compartido");
                        return;
                    }
                    break;
                default:
                    break;
            }
            deleteItemM(pos, M);
            printf("Bloque en la dirección %p liberado\n", address);
            return;
        }
    }

    // Si no encontramos la dirección en la lista, imprimir el mensaje correspondiente
    
}

void command_deallocate(char *param1[], int trozos, tListM *M) {
    if (trozos < 2) { // Mostrar lista completa de bloques asignados
        printf("******Lista de bloques asignados para el proceso %d\n", getpid());
        return;
    }

    if (strcmp(param1[1], "-malloc") == 0) {
        if (trozos == 2) { // Mostrar solo bloques malloc
            mostrarMemList(*M, getpid(), MALLOC);
        } else if (trozos == 3) { // Liberar bloque malloc de tamaño especificado
            size_t size = (size_t)atoi(param1[2]);
            deallocate_malloc(size, M);
        } else {
            printf("Error: Comando invalido para -malloc\n");
        }
    } else if (strcmp(param1[1], "-mmap") == 0) {
        if (trozos == 2) { // Mostrar solo bloques mmap
            mostrarMemList(*M, getpid(), MMAP);
        }else if (trozos == 3) { // Desmapear archivo especificado
            deallocate_mmap(param1[2], M);
        } else {
            printf("Error: Comando invalido para -mmap\n");
        }
    } else if (strcmp(param1[1], "-shared") == 0) {
        if (trozos == 2) { // Mostrar solo bloques shared
            mostrarMemList(*M, getpid(), SHARED);
        }else if (trozos == 3) { // Desadjuntar bloque compartido especificado
            key_t key = (key_t)strtoul(param1[2], NULL, 10);
            deallocate_shared(key, M);
        } else {
            printf("Error: Comando invalido para -shared\n");
        }
    } else if (strcmp(param1[1], "-delkey") == 0) {
        if (trozos == 2) { // Mostrar solo bloques delkey
            printf("delkey necesita clave_valida");
        }else if (trozos == 3) { // Eliminar clave de memoria compartida del sistema
            key_t key = (key_t)strtoul(param1[2], NULL, 10);
            deallocate_delkey(key);
        } else {
            printf("Error: Comando invalido para -delkey\n");
        }
    } else if (strcmp(param1[1], "-addr") == 0){ // Liberar por dirección
        void *address = (void *)strtoull(param1[2], NULL, 16);
        deallocate_address(address, M);
    }
}

void command_memfill (void *p, size_t cont, unsigned char byte) {
    //void *p es el puntero a la direccion de memoria de inicio, donde comenzaremos a escribir
    //size_t cont es el numero de bytes a llenar con el valor especifico
    //unsigned char byte es el valor del byte con el que se llenara la memoria
    unsigned char *arr=(unsigned char *) p;
    size_t i;

    printf("Llenando %ld bytes de memoria con el byte %02x a partir de la direccion %p\n", cont, byte, p);

    for (i=0; i<cont;i++)
        arr[i]=byte;
}

void imprimir_char (char c){ // Muestra caracteres
    bool imprimir = false;
    if (c > 32 && c < 126)
        imprimir = true;
    else
        imprimir = false;

    if(imprimir)
        printf("%4c", c);
    else
        printf("%4c", ' ');

    imprimir = false;
}

void imprimir_hex (char c){ //Muestra los caracteres en código hexadecimal
    bool imprimir = false;
    if (c >= 32 && c <= 126)
        imprimir = true;
    else
        imprimir = false;

    if(imprimir)
        printf("%4x", c);
    else
        printf("%3c%c", '0','0');
    imprimir = false;
}

void imprimir_mem (char *addr, int nbytes){ //Función auxiliar para poder hacer el comando Mendump
    int i, j;
    int pos_char;
    int veces = 0, lineas = 0;
    if(addr == NULL){
        perror("Direccion de memoria invalida.\n");
    }

    for (i = 0; i < nbytes; i++){
        imprimir_char(addr[i]);
        veces++;

        if((veces == nbytes - 25 * lineas) || (veces == 25)){
            printf("\n");
            pos_char = i + 1 - veces;

            for (j = pos_char; j < pos_char + veces; ++j){
                imprimir_hex(addr[j]);
            }
            printf("\n");
            lineas++;
            veces = 0;
        }
    }
}

void command_memdump(void *addr, int nbytes) {
    printf("Volcando %d bytes desde la dirección %p\n", nbytes, addr);
    imprimir_mem(addr, nbytes); 
}

void *cadtop(char *string){
    //Convierte un string a puntero
    void *p;
    sscanf(string, "%p", &p);
    return p;
}

ssize_t readFile(char *f, void *p, size_t cont) {
    struct stat s;
    ssize_t n;
    int df, aux;

    if (stat(f, &s) == -1 || (df = open(f, O_RDONLY)) == -1)
        return -1;
    if (cont == -1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont = s.st_size;
    if ((n = read(df, p, cont)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return -1;
    }
    close(df);
    return n;
}

void command_readfile(char *args[], int trozos){
    // readfile file addr cont : Lee cont bytes desde file a la direccion addr

    void *p;
    size_t cont = -1;  // -1 -> leer el fichero completo
    ssize_t n;
    if (trozos < 3){
	printf ("faltan parametros\n");
	return;
   }
    p = cadtop(args[2]);  /*convertimos de cadena a puntero*/
    if (args[3]!=NULL)
	    cont=(size_t) atoll(args[3]);

    if ((n = readFile(args[1],p,cont))==-1)
	    perror ("Imposible leer fichero");
    else
	    printf ("leidos %lld bytes de %s en %p\n",(long long) n,args[1],p);
}

ssize_t writeFile(char *f, void *p, size_t cont,int overwrite){
    ssize_t  n;
    int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

    if (overwrite)
        flags=O_CREAT | O_WRONLY | O_TRUNC;
    if ((df = open(f, flags, 0777)) == -1)
        return -1;
    if ((n = write(df, p, cont)) == -1){
        aux = errno;
        close(df);
        errno = aux;
        return -1;
    }
    close (df);
    return n;
}

void command_writefile(char *args[], int trozos){
    //write [-o] file addr cont : Escribe cont bytes desde la direccion addr a file (-o sobreescribe)
    void *p;
    size_t cont;
    ssize_t n;

    if(trozos < 4){
        puts("Faltan parametros");
    } else{
        if (strcmp(args[1],"-o") == 0){ // -o (sobreescritura)
            if(trozos == 5){
                p = cadtop(args[3]); // transforma en puntero la direccion de memoria
                cont = (size_t) atoll(args[4]); // convierte cadena en numerico
                if ((n = writeFile(args[2], p, cont,1)) == -1){ // intenta escribir en fichero
                    perror("Imposible escribir fichero");
                } else{
                    printf("escritos %lld bytes de %s desde %p\n", (long long) n, args[2], p);
                }
            } else{
                puts("Faltan parametros");
            }
        } else{ // opcion escribir (sin -o)
            if(trozos == 4){
                p = cadtop(args[2]);
                cont = (size_t) atoll(args[3]);
                if ((n = writeFile(args[1], p, cont,0)) == -1){
                    perror("Imposible escribir fichero");
                } else{
                    printf("escritos %lld bytes de %s desde %p\n", (long long) n, args[1], p);
                }
            } else{
                puts("Faltan parametros");
            }
        }
    }
}

void execute_and_print(const char *command) {
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error al ejecutar el comando");
        return;
    }

    // Leer la salida del comando línea por línea
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    pclose(fp);
}


void command_memory(char *param[], int trozos, tListM *M) {

    int localVar1 = 30;
    int localVar2 = 40;
    int localVar3 = 50;

    if (trozos < 2 || strcmp(param[1], "-all") == 0) {
        pid_t pid = getpid();
        printf("Variables locales     %p, %p, %p\n", 
               (void*)&localVar1, (void*)&localVar2, (void*)&localVar3);
        printf("Variables globales    %p, %p, %p\n", 
               (void*)&globalVarUninit1, (void*)&globalVarUninit2, (void*)&globalVarUninit3);
        printf("Var (N.I.) globales   %p, %p, %p\n", 
               (void*)&globalVar1, (void*)&globalVar2, (void*)&globalVar3);
        printf("Variables estaticas   %p, %p, %p\n", 
               (void*)&staticVarUninit1, (void*)&staticVarUninit2, (void*)&staticVarUninit3);
        printf("Var (N.I.) estaticas  %p, %p, %p\n", 
               (void*)&staticVar1, (void*)&staticVar2, (void*)&staticVar3);
        
        printf("Funciones programa:   %p, %p, %p\n", (void *)command_authors,(void *)command_allocate,(void *)command_deallocate); 
        printf("Funciones libreria    %p, %p, %p\n", (void*)malloc, (void*)free, (void*)fopen);
        printf("******Lista de bloques asignados para el proceso %d\n", pid);
    } else if (strcmp(param[1], "-funcs") == 0) {
        // Imprimir direcciones de funciones del programa
        printf("Funciones programa:    %p, %p, %p\n", (void *)command_authors,(void *)command_allocate,(void *)command_deallocate); 
        printf("Funciones libreria     %p, %p, %p\n", (void*)malloc, (void*)free, (void*)fopen);
        return ;
    } else if (strcmp(param[1], "-vars") == 0) {
        printf("Variables locales     %p, %p, %p\n", 
               (void*)&localVar1, (void*)&localVar2, (void*)&localVar3);
        printf("Variables globales    %p, %p, %p\n", 
               (void*)&globalVarUninit1, (void*)&globalVarUninit2, (void*)&globalVarUninit3);
        printf("Var (N.I.) globales   %p, %p, %p\n", 
               (void*)&globalVar1, (void*)&globalVar2, (void*)&globalVar3);
        printf("Variables estaticas   %p, %p, %p\n", 
               (void*)&staticVarUninit1, (void*)&staticVarUninit2, (void*)&staticVarUninit3);
        printf("Var (N.I.) estaticas  %p, %p, %p\n", 
               (void*)&staticVar1, (void*)&staticVar2, (void*)&staticVar3);
    } else if (strcmp(param[1], "-blocks") == 0) {
        mostrarMemListCompleta(*M);
    } else if (strcmp(param[1], "-pmap") == 0)  {
        pid_t pid = getpid();
        #ifdef __APPLE__ 
        char command[256];
        snprintf(command, sizeof(command), "vmmap %d", pid); // Usar "vmmap" en macOS
        system(command);
        #else
        char command[128];
        snprintf(command, sizeof(command), "pmap %d", pid);
        system(command);
        #endif
    }
}

// Leer desde un descriptor de archivo abierto
ssize_t readFromDescriptor(int df, void *p, size_t cont) {
    ssize_t n;

    if ((n = read(df, p, cont)) == -1) {
        perror("Error al leer del descriptor");
        return -1;
    }
    return n;
}

void command_read(char *args[], int trozos){
    // read df addr cont : Lee cont bytes desde el descriptor df a la dirección addr
    void *p;
    size_t cont = -1;  // -1 -> leer todo lo que quede en el descriptor
    ssize_t n;
    int df;

    if (trozos < 3) {
        printf("Faltan parámetros\n");
        return;
    }

    // Convertimos el descriptor de archivo (df) de cadena a entero
    df = atoi(args[1]); 

    // Convertimos la dirección en cadena a puntero
    p = cadtop(args[2]);

    // Convertimos la cantidad de bytes a leer
    if (args[3] != NULL)
        cont = (size_t) atoll(args[3]);

    // Llamamos a la función para leer desde el descriptor
    if ((n = readFromDescriptor(df, p, cont)) == -1)
        perror("Imposible leer del descriptor");
    else
        printf("Leídos %lld bytes del descriptor %d en %p\n", (long long) n, df, p);
}

ssize_t writefd(int df, void *p, size_t cont){
    ssize_t n;
    int aux;

    // Intentamos escribir en el archivo usando el descriptor de archivo ya abierto
    if ((n = write(df, p, cont)) == -1){
        aux = errno;
        errno = aux;
        return -1;
    }

    return n;
}

void command_write(char *args[], int trozos){
    // writefd addr cont: Escribe cont bytes desde la direccion addr usando un descriptor de archivo ya abierto
    void *p;
    size_t cont;
    ssize_t n;
    int df;

    if(trozos < 4){
        puts("Faltan parametros");
    } else{
        df = atoi(args[1]); // Obtén el descriptor de archivo de los argumentos

        // Si df es inválido, muestra un error
        if (df < 0){
            puts("Descriptor de archivo no válido");
            return;
        }

        // Convertimos el argumento de la dirección a puntero y el argumento de tamaño a entero
        p = cadtop(args[2]);
        cont = (size_t) atoll(args[3]);

        // Intentamos escribir en el archivo con el descriptor ya abierto
        if ((n = writefd(df, p, cont)) == -1){
            perror("Imposible escribir en el archivo");
        } else {
            printf("Escritos %lld bytes desde %p al descriptor de archivo %d\n", (long long) n, p, df);
        }
    }
}

void command_recurse(char *param[], int trozos){
    if(trozos > 1)
        recursiva(atoi(param[1]));
}


//Practica 3

char * user(uid_t uid){
    /*
     * Obtiene el nombre de usuario correspondiente al uid
     */
    struct passwd *passwd;

    if ((passwd = getpwuid(uid)) == NULL) // obtiene los datos del usuario (si existe)
        return ("UKNOWN");
    return passwd->pw_name;
}

void command_getuid(){
    uid_t real = getuid(), efec = geteuid();
    printf ("Credencial real: %d, (%s)\n", real, user(real));
    printf ("Credencial efectiva: %d, (%s)\n", efec, user(efec));
}

uid_t obtenerUid(char *nombre){
    /*
     * Obtiene el uid correspondiente al nombre de usuario
     */
    struct passwd *passwd;

    if ((passwd = getpwnam(nombre)) == NULL) // obtiene informacion del usuario
        return (uid_t) -1;
    return passwd->pw_uid;
}

void command_setuid(char *param[], int trozos){
    uid_t uid;
    if(trozos > 1){
        if(strcmp(param[1],"-l") == 0){ // setuid -l
            if(trozos > 2){
                if ((uid = obtenerUid(param[2])) == (uid_t) (-1)) {
                    // busca el uid correspondiente al usuario
                    printf("Usuario no existente %s\n", param[2]);
                    return;
                }
                if (setuid(uid) == -1){ // cambia uid del proceso
                    perror("Imposible cambiar credencial");
                }
            } else command_getuid();
        } else {
            uid = strtol(param[1],NULL,10); // convierte string en long
            if(setuid(uid) == -1){ // cambia uid del proceso
                perror("Imposible cambiar credencial");
            }
        }
    } else {
        command_getuid();
    }
}

void imprimirVariable(char *env[], char *name){
    /*
     * Imprime información sobre una variable de entorno
     */
    int i;

    for(i = 0; env[i] != NULL; ++i){
        printf("%p->%s[%d]=(%p) %s\n", &env[i], name, i, env[i], env[i]);
    }
}

int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parÃ¡metro*/
{                                           /*devuelve la posicion de la variable en el entorno, -1 si no existe*/
  int pos=0;
  char aux[MAXVAR];
  
  strcpy (aux,var);
  strcat (aux,"=");
  
  while (e[pos]!=NULL)
    if (!strncmp(e[pos],aux,strlen(aux)))
      return (pos);
    else 
      pos++;
  errno=ENOENT;   /*no hay tal variable*/
  return(-1);
}

int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                        /*lo hace directamente, no usa putenv*/
  int pos;
  char *aux;
   
  if ((pos=BuscarVariable(var,e))==-1)
    return(-1);
 
  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
	return -1;
  strcpy(aux,var);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}
tItemLP actualizarEstado(tItemLP item, int opcion){
    int estado;

    opcion = WNOHANG | WUNTRACED | WCONTINUED;

    if(waitpid(item.pid, &estado, opcion) == item.pid){
        if(WIFEXITED(estado)){
            item.estado = FINISHED;
            item.signal = WEXITSTATUS(estado);
        } else if (WIFCONTINUED(estado)){
            item.estado = ACTIVE;
        } else if (WIFSTOPPED(estado)){
            item.estado = STOPPED;
            item.signal = WTERMSIG(estado);
        } else if (WIFSIGNALED(estado)){
            item.estado = SIGNALED;
            item.signal = WTERMSIG(estado);
        }
    }
    return item;
}
void borrarProcesos(status estado, tListP *procList){
    /*
     * Borra los procesos de la lista con el estado x
     */
    tItemLP item;
    int i;

    for (i = firstP(*procList); i <= lastP(*procList) && !isEmptyListP(*procList); ++i){
        item = getItemP(i,*procList);
        if(item.estado == estado){
            deleteItemP(i,procList);
            --i;
        }
    }

}
void imprimirProceso(tItemLP item){
    char *estado = malloc(20);

    switch (item.estado) {
        case FINISHED:
            strcpy(estado,"FINISHED");
            break;
        case STOPPED:
            strcpy(estado,"STOPPED");
            break;
        case ACTIVE:
            strcpy(estado,"ACTIVE");
            break;
        case SIGNALED:
            strcpy(estado,"SIGNALED");
            break;
    }
    printf("%d\t%s p=%d %s %s (%03d) %s\n", item.pid, item.user,
           getpriority(PRIO_PROCESS, item.pid), item.date, estado, item.signal, item.cmd);
}
void command_showvar(char *param[], int trozos, char * arg3[]){
    /*
     * showvar var:  Muestra el valor y las direcciones de la variable de entorno var
     */
    int i,j;
    char * valor;

    if(trozos == 1){
        imprimirVariable(arg3, "main arg3");
    } else if(trozos == 2){
        valor = getenv(param[1]); // obtiene valor variable de entorno
        if(valor != NULL){
            if ((i = BuscarVariable(param[1], arg3)) == -1) {
                return;
            }
            if ((j = BuscarVariable(param[1], environ)) == -1) { //accede a la variable global environ
                return;
            } else {
                printf("Con arg3 main:\t%s (%p) %p\n", arg3[i], arg3[i], &arg3[i]);
                printf("Con environ:\t%s (%p) %p\n", environ[j], environ[j], &environ[j]);
                printf("Con getenv:\t%s (%p)\n", valor, &valor);
            }
        }
    }
}

void command_changevar (char *param[], int trozos, char *arg3[], char *env[]){
    /*
     * changevar [-a|-e|-p] var valor  Cambia el valor de una variable de entorno
     *  -a: accede por el tercer arg de main
     *  -e: accede mediante environ
     *  -p: accede mediante putenv
     */
    int i;
    char * var;
    if(trozos < 4){
        puts("Uso: changevar [-a|-e|-p] var valor");
    } else{
        var = malloc(strlen(param[2]) + strlen(param[3]) + 4);
        strcpy(var, param[2]);
        strcat(var, "=");
        strcat(var, param[3]);
        if(strcmp(param[1],"-a") == 0){
            if ((i = BuscarVariable(param[2], arg3)) == -1) {
                perror("Imposible cambiar variable");
                free(var);
                return;
            }
            arg3[i] = var;
        } else if(strcmp(param[1],"-e") == 0){
            if ((i = BuscarVariable(param[2], env)) == -1) {
                perror("Imposible cambiar variable");
                free(var);
                return;
            }
            env[i] = var;
        } else if(strcmp(param[1],"-p") == 0){
            if (putenv(var) != 0) {
                free(var);
                return;
            }
        }
    }
}
void command_subsvar(char *param[], int trozos, char *arg3[], char *env[]){
    /*
     * subsvar [-a|-e] var1 var2 valor Sustituye la variable de entorno var1
     *  con var2=valor
     *  -a: accede por el tercer arg de main
     *  -e: accede mediante environ
     */

    if(trozos < 5){
        puts("Uso: subsvar [-a|-e] var valor");
    } else{
        if(strcmp(param[1],"-a") == 0){
            if(CambiarVariable(param[3],param[4],arg3) == (-1)){
                perror("Imposible cambiar variable");
                return;
            }
        } else if(strcmp(param[1],"-e") == 0){
            if(CambiarVariable(param[3],param[4],env) == (-1)){
                perror("Imposible cambiar variable");
                return;
            }
        }
    }
}

void command_environ(char *param[], int trozos, char *arg3[], char *env[]){
    /*
     * environ [-environ|-addr]: Muestra el entorno del proceso
     *  -environ: accede usando environ (en lugar del tercer arg de main)
     *  -addr: muestra el valor y donde se almacenan environ y el 3er arg main
     */

    if (trozos < 2){
        imprimirVariable(arg3, "main arg3");
    } else{
        if (strcmp(param[1],"-environ") == 0){
            imprimirVariable(environ, "environ");
        } else if (strcmp(param[1],"-addr") == 0){
            printf("environ: %p (almacenado en %p)\n",&env[0], &env[0]); //ns si esta bien
            printf("main arg3: %p (almacenado en %p)\n",&arg3[0], &arg3[0]);
        }
    }
}

void command_fork(tListP *procList){
    /*
     * fork : El shell hace fork y queda en espera a que su hijo termine
     */
    pid_t pid;

    if ((pid = fork()) == 0) {
        while(!isEmptyListP(*procList)){ // elimina el contenido de la lista
            deleteItemP(firstP(*procList),procList);
        }
        printf ("ejecutando proceso %d\n", getpid());
    } else if (pid != -1){
        waitpid (pid,NULL,0);
    } else{
        perror("Error con fork");
    }
}
void command_listjobs(tListP *procList){
    /*
     * jobs: Lista los procesos en segundo plano
     */
    int pos;
    tItemLP item;

    if(!isEmptyListP(*procList)){
        for(pos = firstP(*procList); pos <= lastP(*procList); ++pos){
            item = getItemP(pos,*procList);
            item = actualizarEstado(item,0);
            imprimirProceso(item);
        }
    }else return;
}
void command_deljobs(char *param[], int trozos, tListP *procList){
    /*
     * deljobs [-term][-sig]   Elimina los procesos de la lista procesos en sp
     *  -term: los terminados
     *  -sig: los terminados por senal
     */
    int i;

    if(trozos >= 2){
        for(i = 1; i < trozos; ++i){
            if(strcmp(param[i],"-term") == 0){ // borrar procesos terminados
                borrarProcesos(FINISHED,procList);
            } else if(strcmp(param[i],"-sig") == 0){ // borrar procesos terminados por señal
                borrarProcesos(SIGNALED,procList);
            }
        }

    } else{
        command_listjobs(procList);
    }
}
