/*
Simple network library Copyright (C) 2009 Jesús Hernández Gormaz

Fecha de creacion: 22 de marzo del 2009 (Siglo XXI)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Este programa es software libre. Puede redistribuirlo y/o
modificarlo bajo los términos de la Licencia Pública General
de GNU según es publicada por la Free Software Foundation,
bien de la versión 3 de dicha Licencia o bien (según su
elección) de cualquier versión posterior.
Este programa se distribuye con la esperanza de que sea
útil, pero SIN NINGUNA GARANTÍA, incluso sin la garantía
MERCANTIL implícita o sin garantizar la CONVENIENCIA PARA UN
PROPÓSITO PARTICULAR. Para más detalles, véase la Licencia
Pública General de GNU.

Debería haber recibido una copia de la Licencia Pública
General junto con este programa. En caso contrario, escriba
a la Free Software Foundation, Inc., en 675 Mass Ave,
Cambridge, MA 02139, EEUU.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef unix
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2udpip.h>
#endif

/*macros con informacion sobre la biblioteca*/
#define NOMBRE_BIBLIOTECA "Simple network library"
#define SIGLAS_BIBLIOTECA "SNL"
#define VERSION_BIBLIOTECA "0.0.1"
#define FECHA_CREACION_BIBLIOTECA "Domingo 22 de Marzo del 2009 (22/3/2009) siglo XXI D.C."
#define AUTOR_BIBLIOTECA "Jesús Hernández Gormaz"

/*estructura de grupo de sockets*/
struct grupo_sockets{
	char id;
	int servidor;
	int maximo;
	int minimo;
	fd_set conectados;
	fd_set envios_remotos;
	struct timeval tiempo_espera;
};

/*macros para usar la funcion para el tratamiento de errores*/
#define REINICIAR_ERRORES SNL_informar_errores( 0, 0)
#define NERROR 1
#define UERROR 2
#define CERROR 3

/*macros para usar la funcion para el control del nivel de ejecucion
 de la biblioteca*/
#define NNIVEL 1
#define ANIVEL 0
#define NIVEL_EJECUCION SNL_ejecucion(0, 0)

/*macros para definir los niveles de ejecucion de la biblioteca*/
#define E_ANADA 0
#define E_NADA 1
#define E_EBIBLIOTECA 2
#define E_LMEMORIA 3
#define E_IBIBLIOTECA 4
#define E_TRABAJARDATOS 5
#define E_RMEMORIA 6
#define E_TODO 10

#include <SNL.h>


/*Funciones de informacion interna de la libreria y de mantenimiento y
 recuperacion y notificacion de errores de la misma*/

/*Informa sobre la capacidad de ejecucion de la biblioteca para restringir
 el uso de algunas funciones o todas ante errores para evitar otros
 errores igual o más graves*/
unsigned int SNL_ejecucion(char accion, unsigned int nivel_ejecucion);

unsigned int SNL_ejecucion(char accion, unsigned int nivel_ejecucion){
register unsigned int r=0;
static unsigned int ejecucion=E_TODO;
if(accion == ANIVEL){
	r= ejecucion;
}else if(accion == NNIVEL){
	ejecucion= nivel_ejecucion;
	r= nivel_ejecucion;
}
return r;
}

/*Informa de los errores de las funciones de la libreria*/
unsigned int SNL_informar_errores(unsigned int accion, unsigned int codigo_error);

unsigned int SNL_informar_errores(unsigned int accion, unsigned int codigo_error){
if(NIVEL_EJECUCION < E_EBIBLIOTECA){
	return 0;
}
register unsigned int r=0;
static unsigned int error=0;
static char comprobado=0;
if(accion == 0 && codigo_error == 0){
	error= 0;
	comprobado= 0;
	r= 0;
}else if(accion == NERROR){
	/*Tratando el error 1 que es un error grave inrrecuperable que
	 impide la ejecución de todo el programa y provoca la inmediata
	 finalización del mismo llamando a exit(-1)*/
	if(codigo_error == 1){
		exit(-1);
	}else if(codigo_error == 2){
		/*Tratando el erro 2 que impide la ejecucion de cualquier
		 funcion de la biblioteca*/
		SNL_ejecucion(NNIVEL, E_NADA);
	}
	error= codigo_error;
	comprobado= 0;
	r= 0;
}else if(accion == UERROR && codigo_error == 0){
	r= error;
	comprobado= 1;
}else if(accion == UERROR && codigo_error != 0){
	r= error;
	error= codigo_error;
	comprobado= 0;
}else if(accion == CERROR && codigo_error == 0){
	if(comprobado == 1){
		r= 0;
	}else{
		r= 1;
	}
}
return r;
}

extern int SNL_emergencia(void){
if(NIVEL_EJECUCION < E_NADA){
	return -2;
}
register int r=-1;
if(SNL_ejecucion(NNIVEL, E_TODO) == E_TODO){
	r= 0;
	SNL_informar_errores(NERROR, 4);
}else{
	r= -1;
	SNL_ejecucion(NNIVEL, E_ANADA);
}
return r;
}

extern unsigned int SNL_ultimo_error(void){
if(NIVEL_EJECUCION < E_EBIBLIOTECA){
	return 0;
}
register unsigned int error;
error= SNL_informar_errores(UERROR, 0);
return error;
}

extern int SNL_comprobado_ultimo_error(void){
if(NIVEL_EJECUCION < E_EBIBLIOTECA){
	return -2;
}
register int r=0;
r= SNL_informar_errores(CERROR, 0);
if(r != 0){
	r= -1;
}
return r;
}

extern unsigned long int informacion_biblioteca_SNL(char *informacion, unsigned long int longitud){
if(NIVEL_EJECUCION < E_IBIBLIOTECA){
	SNL_informar_errores(NERROR, 6);
	return 0;
}
register unsigned long int r=0;
char texto[512]="Nombre de la biblioteca: \0";
sprintf(&texto[0], "%s%s - Siglas: %s - Version: %s", &texto[0], NOMBRE_BIBLIOTECA, SIGLAS_BIBLIOTECA, VERSION_BIBLIOTECA);
sprintf(&texto[0], "%s - Fecha de creacion: %s - Creador de la biblioteca: %s", &texto[0], FECHA_CREACION_BIBLIOTECA, AUTOR_BIBLIOTECA);
sprintf(&texto[0], "%s - Fecha de compilacion: %s - Hora de compilacion: %s", &texto[0], __DATE__, __TIME__);
strcat(&texto[0], " - Licencia de la biblioteca: GNU General Public License (GPL)");
r= strlen(&texto[0]);
if(r+1 <= longitud){
	strcpy(informacion, &texto[0]);
	r= 0;
}
return r;
}

/*Funciones criptograficas*/

extern int EncriptarTexto_CifradoBitsIguales(unsigned char *texto, unsigned char *clave){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
/*comprobamos que los valores pasados no sean nulos*/
if((texto == NULL || clave == NULL) || (*texto == '\0' || *clave == '\0')){
	return -1;
}
unsigned char *caracter_t, *caracter_c;
/*asignando a cada puntero la direccion de memoria del primer byte de cada
 cadena*/
caracter_t= texto;
caracter_c= clave;
/*ejecutando el bucle mientras no estemos en el ultimo byte de la cadena de
 texto identificado como nulo, 0*/
while(*caracter_t != 0){
	/*asignamos el resultado de un xor entre los bits de texto y clave
	 y realizamos un not a dicho resultado*/
	*caracter_t= ~(*caracter_t ^ *caracter_c);
	/*apuntamos cada puntero al siguiente byte en memoria*/
	caracter_t++;
	caracter_c++;
	/*comprobamos si estamos al final de la cadena clave comprobando si
	 el byte en el que estamos situados es el caracter nulo, 0*/
	if(*caracter_c == 0){
		/*volvemos a apuntar al primer byte de clave*/
		caracter_c= clave;
	}
}
return 0;
}

/*Funciones de suma de verificacion y digitos de verificación*/

extern unsigned int checksum(SNL_datos datos, unsigned long int longitud){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return 0;
}
unsigned long int acumulacion=0;
register unsigned long int i;
unsigned int r, *cd;
cd=(unsigned int *) datos;
for(i=0; i<longitud; i+=2){
	acumulacion+= (*(cd++));
}
r= ~(((acumulacion>>16) + (acumulacion & 0xFFFF)) & 0xFFFF);
return r;
}

/*Funciones para trabajar con grupos de conexiones*/

extern grupo_conexiones *SNL_nuevo_grupo_conexiones(int conexion_escucha){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return NULL;
}
grupo_conexiones *nuevo;
nuevo=(grupo_conexiones *) malloc(sizeof(grupo_conexiones));
if(nuevo != NULL){
	nuevo->id= 0;
	nuevo->servidor= conexion_escucha;
	FD_ZERO(&(nuevo->conectados));
	FD_ZERO(&(nuevo->envios_remotos));
	if(conexion_escucha != -1){
		FD_SET(conexion_escucha, &(nuevo->conectados));
	}
	nuevo->maximo= conexion_escucha;
	nuevo->minimo= conexion_escucha;
	nuevo->tiempo_espera.tv_sec= 0;
	nuevo->tiempo_espera.tv_usec= 500000;
}else{
	nuevo= NULL;
	SNL_informar_errores(NERROR, 100);
}
return nuevo;
}

extern int SNL_nueva_conexion_grupo(int conexion, grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
if(grupo == NULL || conexion == SIN_CONEXION){
	r= -1;
}else{
	FD_SET(conexion, &(grupo->conectados));
	if(grupo->maximo < conexion){
		grupo->maximo= conexion;
	}
	if(grupo->minimo > conexion || grupo->minimo == SIN_CONEXION){
		grupo->minimo= conexion;
	}
}
return r;
}

extern int SNL_quitar_conexion_grupo(int conexion, grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_LMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int i, maximo, minimo;
if(conexion != SIN_CONEXION){
	FD_CLR(conexion, &(grupo->conectados));
}
if(conexion == grupo->maximo){
	maximo= grupo->maximo;
	minimo= grupo->minimo;
	for(i=maximo; i>=minimo; i--){
		if(FD_ISSET(i, &(grupo->conectados))){
			grupo->maximo=i;
			break;
		}
	}
}else if(conexion == grupo->minimo){
	maximo= grupo->maximo;
	minimo= grupo->minimo;
	for(i=minimo; i<=maximo; i++){
		if(FD_ISSET(i, &(grupo->conectados))){
			grupo->minimo=i;
			break;
		}
	}
}
if(conexion == grupo->servidor){
	grupo->servidor= SIN_CONEXION;
}
return 0;
}

extern int SNL_desconectar_conexion_grupo(int conexion, grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_LMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
SNL_quitar_conexion_grupo(conexion, grupo);
SNL_desconectar(conexion);
return 0;
}

extern int SNL_tiempo_espera_grupo(grupo_conexiones *grupo, int segundos, int microsegundos){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=-1;
if(grupo != NULL){
	grupo->tiempo_espera.tv_sec= segundos;
	grupo->tiempo_espera.tv_usec= microsegundos;
	r= 0;
}
return r;
}

extern int SNL_cambiar_id_grupo(grupo_conexiones *grupo, char id){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=-1;
if(grupo != NULL){
	grupo->id= id;
	r= 0;
}
return r;
}

char SNL_id_grupo(grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register char r=0;
if(grupo != NULL){
	r= grupo->id;
}
return r;
}

extern int SNL_pertenece_a_grupo_conexiones(int conexion, grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=-1;
if(conexion != SIN_CONEXION && grupo != NULL){
	if(conexion < grupo->maximo && conexion > grupo->minimo){
		if(FD_ISSET(conexion, &(grupo->conectados))){
			r= 0;
		}else{
			r= 1;
		}
	}else{
		if(conexion == grupo->maximo || conexion == grupo->minimo){
			r= 0;
		}else{
			r= 1;
		}
	}
}else{
	r= -1;
}
return r;
}

extern int SNL_enviar_grupo_TCP(grupo_conexiones *grupo, SNL_datos datos, int bytes, int flags){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0, i, maximo;
/*Comprovamos que ni el grupo ni los datos sean nulos y que los bytes sean
 mayores que 0*/
if(grupo == NULL || datos == NULL || bytes <= 0){
	/*Preparamos el valor de retorno para devolver el error*/
	r= -1;
}else{
	/*Copiamos el socket maximo a una variable para que el codigo se
	 ejecute con mayor rapidez que si tuviera que acceder en cada
	 ciclo de for a la estructura*/
	maximo= grupo->maximo;
	/*Recorremos todos los sockets posibles*/
	for(i=grupo->minimo; i<=maximo; i++){
		/*Comprobamos si el socket i forma parte del grupo y no es la
		conexion de escucha*/
		if(SNL_pertenece_a_grupo_conexiones(i, grupo) == 0 && i != grupo->servidor){
			/*Enviamos los datos y recogemos el retorno*/
			r+= SNL_enviar_TCP(i, datos, bytes, flags);
		}
		/*Evitamos que en r se produzca un desbordamiento y
		 pase a ser negativo impidiendo que pase de 10000*/
		if(r > 10000){
			r= 1;
		}
	}
}
/*Comprobamos si hubo algun error en alguno de los envios de*/
if(r > 0){
	r= -1;
}
return r;
}

extern int SNL_comprobar_grupo_conexiones(grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
/*Vaciamos el conjunto de sockets con el que trabajaremos para que no se vea
 afectado el conjunto de sockets con todos los sockets del grupo*/
FD_ZERO(&(grupo->envios_remotos));
/*Copiamos el conjunto con todos los sockets al conjunto con el que
 trabajaremos*/
grupo->envios_remotos= grupo->conectados;
/*Hacemos la llamada al sistema select para obtener en el conjunto con el que
 estamos trabajando todos los sockets en los que tengamos datos que leer o
 alguna conexion haiga finalizado por el otro ordenador*/
if(select((grupo->maximo)+1, &(grupo->envios_remotos), NULL, NULL, &(grupo->tiempo_espera)) == -1){
	/*Cambiamos r a -1 para usarlo como valor de retorno indicando que no
	 hay sockets con datos enviados por el ordenador remoto ni conexiones
	 entrantes en cola*/
	r= -1;
}
return r;
}

extern int SNL_conexion_entrante_grupo(grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
r= SNL_comprobar_grupo_conexiones(grupo);
if(r != -1 && grupo->servidor != SIN_CONEXION){
	r= 0;
	if(FD_ISSET(grupo->servidor, &(grupo->envios_remotos))){
		r= 1;
		FD_CLR(grupo->servidor, &(grupo->envios_remotos));
	}
}else{
	if(grupo->servidor == SIN_CONEXION){
		r= -1;
	}else{
		r= 0;
	}
}
return r;
}

extern int SNL_conexion_activa_grupo_conexiones(grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int i, r, maximo;
r= SIN_CONEXION;
maximo= grupo->maximo;
/*En caso de que el grupo tenga una conexion de escucha la quitamos del grupo
 para evitar devolverla como conexion activa*/
if(grupo->servidor != SIN_CONEXION){
	if(FD_ISSET(grupo->servidor, &(grupo->envios_remotos))){
		FD_CLR(grupo->servidor, &(grupo->envios_remotos));
	}
}
for(i=grupo->minimo; i<=maximo; i++){
	if(FD_ISSET(i, &(grupo->envios_remotos))){
		r= i;
		FD_CLR(i, &(grupo->envios_remotos));
		break;
	}
}
return r;
}

extern int SNL_conexion_escucha_grupo(grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
int r=SIN_CONEXION;
if(grupo != NULL){
	r= grupo->servidor;
}
return r;
}

extern int SNL_cambiar_escucha_grupo(grupo_conexiones *grupo, int conexion){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
if(grupo != NULL){
	r= 0;
	grupo->servidor= conexion;
}else{
	r= -1;
}
return r;
}

extern int SNL_liberar_grupo_conexiones(grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_LMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
free(grupo);
return 0;
}

extern int SNL_cerrar_grupo_conexiones(grupo_conexiones *grupo){
if(NIVEL_EJECUCION < E_LMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int i, maximo;
if(grupo->minimo == -1 || grupo->maximo == -1){
	free(grupo);
}else{
	maximo= grupo->maximo;
	for(i=grupo->minimo; i<=maximo; i++){
		if(FD_ISSET(i, &(grupo->conectados))){
			SNL_desconectar(i);
		}
	}
	FD_ZERO(&(grupo->conectados));
	FD_ZERO(&(grupo->envios_remotos));
	free(grupo);
}
return 0;
}


/*Funciones para IPv4*/

extern int SNL_conectar_TCP_IPv4(char *direccion, unsigned short int puerto){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
int descriptor;
struct in_addr direccion_internet;
struct sockaddr_in direccion_socket;
/*pasando la cadena con la IP con los bytes separados por puntos a un numero de 32 bits*/
if(direccion == NULL){
	return -1;
}
direccion_internet.s_addr= inet_addr(direccion);
if(direccion_internet.s_addr == -1){
	r= 1;
	SNL_informar_errores(NERROR, 300);
}else{
	/*rellenando la estructura de la direccion de socket*/
	direccion_socket.sin_family = AF_INET;
	direccion_socket.sin_port = htons(puerto);
	direccion_socket.sin_addr.s_addr= direccion_internet.s_addr;
	/*obteniendo el descriptor de fichero*/
	descriptor= socket(AF_INET, SOCK_STREAM, 0);
	if(descriptor == -1){
		r= 2;
		SNL_informar_errores(NERROR, 300);
	}else{
		if(connect(descriptor, (struct sockaddr *)&direccion_socket, sizeof(struct sockaddr)) == -1){
			r= 3;
			SNL_informar_errores(NERROR, 300);
		}
	}
}
if(r != 0){
	return -1;
}else{
	return descriptor;
}
}

extern int SNL_conectar_UDP_IPv4(unsigned short int puerto){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
int descriptor;
struct sockaddr_in direccion_socket;
direccion_socket.sin_family = AF_INET;
direccion_socket.sin_port = puerto;
direccion_socket.sin_addr.s_addr = INADDR_ANY;
descriptor= socket(AF_INET, SOCK_DGRAM, 0);
if(bind(descriptor, (struct sockaddr *)&direccion_socket, sizeof(struct sockaddr)) == -1){
	r= 1;
	SNL_informar_errores(NERROR, 300);
}
if(r != 0){
	return -1;
}else{
	return descriptor;
}
}

extern int SNL_recibir_UDP_IPv4(int conexion, SNL_datos datos, int bytes, int flags, char *direccion_remota, unsigned short int *puerto_remoto, unsigned long int longitud_d){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
struct sockaddr_in socket_remoto;
int longitud_remoto= sizeof(socket_remoto);
if(conexion != SIN_CONEXION && datos != NULL && direccion_remota != NULL && puerto_remoto != NULL){
	r= recvfrom(conexion, datos, bytes, 0, (struct sockaddr *)&socket_remoto, &longitud_remoto);
	if(r == 0){
		*puerto_remoto= socket_remoto.sin_port;
		inet_ntop(AF_INET, &(socket_remoto.sin_addr), direccion_remota, longitud_d);
	}
}
return r;
}

extern int SNL_enviar_UDP_IPv4(int conexion, SNL_datos datos, int bytes, int flags, char *direccion_remota, unsigned short int puerto_remoto){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
struct sockaddr_in socket_remoto;
int longitud_remoto= sizeof(socket_remoto);
int bytes_enviar, bytes_enviados;
socket_remoto.sin_port= puerto_remoto;
socket_remoto.sin_addr.s_addr= inet_addr(direccion_remota);
socket_remoto.sin_family = AF_INET;
bytes_enviar= bytes;
bytes_enviados= 0;
while(bytes_enviados < bytes){
	r= sendto(conexion, datos, bytes, 0, (struct sockaddr *)&socket_remoto, longitud_remoto);
	if(r == -1){
		break;
	}
	bytes_enviados+= r;
	bytes_enviar-= r;
}
if(bytes_enviados != bytes){
	r= -1;
}
return r;
}

extern int SNL_escuchar_TCP_IPv4(char *direccion, unsigned short int puerto, int cola){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
int descriptor;
struct in_addr direccion_internet;
struct sockaddr_in direccion_socket;
/*pasando la cadena con la IP con los bytes separados por puntos a un numero de 32 bits*/
if(direccion == NULL){
	direccion_internet.s_addr= htonl(INADDR_ANY);
}else{
	direccion_internet.s_addr= inet_addr(direccion);
}
if(direccion_internet.s_addr == -1){
	r= 1;
	SNL_informar_errores(NERROR, 300);
}else{
	/*rellenando la estructura de la direccion de socket*/
	direccion_socket.sin_family = AF_INET;
	direccion_socket.sin_port = htons(puerto);
	direccion_socket.sin_addr.s_addr= direccion_internet.s_addr;
	/*obteniendo el descriptor de fichero*/
	descriptor= socket(AF_INET, SOCK_STREAM, 0);
	if(descriptor == -1){
		r= 2;
		SNL_informar_errores(NERROR, 300);
	}else{
		if(bind(descriptor, (struct sockaddr *)&direccion_socket, sizeof(struct sockaddr)) == -1){
			r= 3;
			SNL_informar_errores(NERROR, 300);
		}else{
			r= listen(descriptor, cola);
			if(r == -1){
				r= 4;
				SNL_informar_errores(NERROR, 300);
			}
		}
	}
}
if(r != 0){
	return -1;
}else{
	return descriptor;
}
}

extern int SNL_aceptar_conexion_IPv4(int conexion_escucha, char *direccion_remota, int *puerto_remoto){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
int r;
int descriptor;
struct sockaddr_in direccion_socket;
r= sizeof(struct sockaddr_in);
if(conexion_escucha == SIN_CONEXION){
	return SIN_CONEXION;
}
descriptor= accept(conexion_escucha, (struct sockaddr *)&direccion_socket, &r);
if(descriptor != -1){
	if(direccion_remota != NULL){
		strcpy(direccion_remota, inet_ntoa(direccion_socket.sin_addr));
	}
	if(puerto_remoto != NULL){
		*puerto_remoto= ntohs(direccion_socket.sin_port);
	}
}else{
	SNL_informar_errores(NERROR, 300);
}
return descriptor;
}

extern int SNL_DNS_IPv4(char *direccion, char *host){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
struct hostent *h;
if(host == NULL || direccion == NULL){
	r= -1;
}else{
	h=gethostbyname(host);
	if(h == NULL){
		r= -1;
	}else{
		strcpy(direccion, inet_ntoa(*((struct in_addr *)h->h_addr)));
		r= 0;
	}
}
return r;
}


/*Funciones para IPv6*/

/*evitando que esto se compile para windows por no soportar este sistema
 el protocolo IPv6*/
#ifndef WIN32
extern int SNL_conectar_TCP(char *direccion, unsigned short int puerto){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
int descriptor;
struct in6_addr direccion_internet;
struct sockaddr_in6 direccion_socket;
/*pasando la cadena con la IP con los bytes separados por puntos a un numero de 32 bits*/
if(inet_pton(AF_INET6, direccion, &direccion_internet) != 1){
	r= 1;
	SNL_informar_errores(NERROR, 300);
}else{
	/*rellenando la estructura de la direccion de socket*/
	direccion_socket.sin6_family = AF_INET6;
	direccion_socket.sin6_port = htons(puerto);
	direccion_socket.sin6_addr= direccion_internet;
	/*obteniendo el descriptor de fichero*/
	descriptor= socket(AF_INET6, SOCK_STREAM, 0);
	if(descriptor == -1){
		r= 2;
		SNL_informar_errores(NERROR, 300);
	}else{
		if(connect(descriptor, (struct sockaddr *)&direccion_socket, sizeof(struct sockaddr)) == -1){
			r= 3;
			SNL_informar_errores(NERROR, 300);
		}
	}
}
if(r != 0){
	return -1;
}else{
	return descriptor;
}
}

extern int SNL_conectar_UDP(unsigned short int puerto){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
int descriptor;
struct sockaddr_in6 direccion_socket;
direccion_socket.sin6_family= AF_INET6;
direccion_socket.sin6_port= puerto;
direccion_socket.sin6_addr= in6addr_any;
descriptor= socket(AF_INET6, SOCK_DGRAM, 0);
if(bind(descriptor, (struct sockaddr *)&direccion_socket, sizeof(struct sockaddr_in6)) == -1){
	r= 1;
	SNL_informar_errores(NERROR, 300);
}
if(r != 0){
	return -1;
}else{
	return descriptor;
}
}

extern int SNL_recibir_UDP(int conexion, SNL_datos datos, int bytes, int flags, char *direccion_remota, unsigned short int *puerto_remoto, unsigned long int longitud_d){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
struct sockaddr_in6 socket_remoto;
int longitud_remoto= sizeof(socket_remoto);
if(conexion != SIN_CONEXION && datos != NULL && direccion_remota != NULL && puerto_remoto != NULL){
	r= recvfrom(conexion, datos, bytes, 0, (struct sockaddr *)&socket_remoto, &longitud_remoto);
	if(r == 0){
		*puerto_remoto= socket_remoto.sin6_port;
		inet_ntop(AF_INET6, &(socket_remoto.sin6_addr), direccion_remota, longitud_d);
	}
}
return r;
}

extern int SNL_enviar_UDP(int conexion, SNL_datos datos, int bytes, int flags, char *direccion_remota, unsigned short int puerto_remoto){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
struct sockaddr_in6 socket_remoto;
struct in6_addr direccion_internet;
int longitud_remoto= sizeof(socket_remoto);
int bytes_enviar, bytes_enviados;
inet_pton(AF_INET6, direccion_remota, &direccion_internet);
socket_remoto.sin6_port= puerto_remoto;
socket_remoto.sin6_addr= direccion_internet;
socket_remoto.sin6_family = AF_INET6;
bytes_enviar= bytes;
bytes_enviados= 0;
while(bytes_enviados < bytes){
	r= sendto(conexion, datos, bytes, 0, (struct sockaddr *)&socket_remoto, longitud_remoto);
	if(r == -1){
		break;
	}
	bytes_enviados+= r;
	bytes_enviar-= r;
}
if(bytes_enviados != bytes){
	r= -1;
}
return r;
}

extern int SNL_escuchar_TCP(char *direccion, unsigned short int puerto, int cola){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
int descriptor;
struct in6_addr direccion_internet;
struct sockaddr_in6 direccion_socket;
/*pasando la cadena con la IP con los bytes separados por puntos a un numero de 32 bits*/
if(inet_pton(AF_INET6, direccion, &direccion_internet) != 1){
	r= 1;
	SNL_informar_errores(NERROR, 300);
}else{
	/*rellenando la estructura de la direccion de socket*/
	direccion_socket.sin6_family = AF_INET6;
	direccion_socket.sin6_port = htons(puerto);
	direccion_socket.sin6_addr= direccion_internet;
	/*obteniendo el descriptor de fichero*/
	descriptor= socket(AF_INET6, SOCK_STREAM, 0);
	if(descriptor == -1){
		r= 2;
		SNL_informar_errores(NERROR, 300);
	}else{
		if(bind(descriptor, (struct sockaddr *)&direccion_socket, sizeof(struct sockaddr_in6)) == -1){
			r= 3;
			SNL_informar_errores(NERROR, 300);
		}else{
			r= listen(descriptor, cola);
			if(r == -1){
				r= 4;
				SNL_informar_errores(NERROR, 300);
			}
		}
	}
}
if(r != 0){
	return -1;
}else{
	return descriptor;
}
}

extern int SNL_aceptar_conexion(int conexion_escucha, char *direccion_remota, int *puerto_remoto, unsigned long int longitud_d){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
int r;
int descriptor;
struct sockaddr_in6 direccion_socket;
r= sizeof(struct sockaddr_in6);
descriptor= accept(conexion_escucha, (struct sockaddr *)&direccion_socket, &r);
if(descriptor != -1){
	if(direccion_remota != NULL){
		inet_ntop(AF_INET6, &direccion_socket, direccion_remota, longitud_d);
	}
	if(puerto_remoto != NULL){
		*puerto_remoto= ntohs(direccion_socket.sin6_port);
	}
}else{
	SNL_informar_errores(NERROR, 300);
}
return descriptor;
}
#endif

extern int SNL_desconectar(int conexion){
if(NIVEL_EJECUCION < E_LMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
if(conexion != SIN_CONEXION){
/*compatibilidad con windows*/
#ifdef unix
	r= close(conexion);
#endif
#ifdef WIN32
	r= closesocket(conexion);
#endif
	if(r != 0){
		r= -1;
		SNL_informar_errores(NERROR, 400);
	}
}else{
	r=0;
}
return r;
}

extern int SNL_enviar_TCP(int conexion, const SNL_datos datos, int bytes, int flags){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0, n;
int bytes_enviar, bytes_enviados;
bytes_enviar= bytes;
bytes_enviados= 0;
while(bytes_enviados < bytes){
	n= send(conexion, (datos+bytes_enviados), bytes_enviar, flags);
	if(n == -1){
		break;
	}
	bytes_enviados+= n;
	bytes_enviar-= n;
}
if(bytes_enviados != bytes && bytes_enviar > 0){
	r= bytes_enviar;
}
return r;
}

extern int SNL_recibir_TCP(int conexion, SNL_datos datos, int bytes, int flags){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
int bytes_leidos;
bytes_leidos= recv(conexion, datos, bytes, flags);
if(bytes_leidos == 0){
	close(conexion);
}
return bytes_leidos;
}

/*evitando que esto se compile para windows por no soportar este sistema
 el protocolo IPv6*/
#ifndef WIN32
extern int SNL_DNS(char *direccion, char *host, char *servicio, unsigned long \
int longitud_d, unsigned long int longitud_s){
if(NIVEL_EJECUCION < E_TRABAJARDATOS){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
int n;
struct sockaddr_in6 direccion_socket;
struct in6_addr direccion_internet;
r= getnameinfo((struct sockaddr *)&direccion_socket, sizeof(struct sockaddr_in6)\
, host, strlen(host), servicio, longitud_s, NI_NUMERICHOST);
direccion_internet= direccion_socket.sin6_addr;
if(r == 0){
	inet_ntop(AF_INET6, &direccion_internet, direccion, longitud_d);
}
return r;
}
#endif


/*Funciones para la compatibilidad con windows*/

/*Nota para compilar la biblioteca para windows:
 Hay que decirle al compilador que enlace la biblioteca  Winsock 2,
 que normalmente se llama Ws2_32.lib*/

#ifdef WIN32
extern int SNL_iniciar_W(void){
if(NIVEL_EJECUCION < E_RMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
register int r=0;
WSADATA wsaData;
if(WSAStartup(MAKEWORD(2, 0), &wsaData) != 0){
	SNL_informar_errores(NERROR, 2);
	r= -1;
}else{
	if(atexit(SNL_acabar_W) == 0){
		r= 0;
	}else{
		r= 1;
	}
}
return r;
}

extern int SNL_acabar_W(void){
if(NIVEL_EJECUCION < E_LMEMORIA){
	SNL_informar_errores(NERROR, 6);
	return -2;
}
WSACleanup();
return 0;
}
#endif

/*Nota para compilar labiblioteca para solaris:
 Si compilas para Solaris o SunOS necesitas indicar algunos conmutadores
 adicionales en la línea de comandos para enlacer las bibliotecas
 adecuadas. Para conseguirlo simplemente añade " -lnsl -lsocket -lresolv"
 al final del comando de compilación. Si todavía tienes errores puedes
 probar a añadir "  -lxnet" al final de la línea de comandos.*/
