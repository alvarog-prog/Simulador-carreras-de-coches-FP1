#include <iostream>
#include <string>
#include<fstream>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;


//Posiciones de la tabla ASCII.
char const CHAR_LINEA_HORIZONTAL = 205;
char const CHAR_ESQUINA_SUPERIOR_IZQUIERDA = 201;
char const CHAR_ESQUINA_SUPERIOR_DERECHA = 187; // ??
char const CHAR_ESQUINA_INFERIOR_IZQUIERDA = 200; // ??
char const CHAR_ESQUINA_INFERIOR_DERECHA = 188; // ??
char const CHAR_LINEA_VERTICAL = 186; // ?
char const CHAR_COCHE = 254; // ?
char const CHAR_CLAVO = 193; // ?
char const CHAR_SORPRESA = 63; // ?
char const CHAR_NORMAL = 32;

typedef enum { NORMAL, CLAVO, SORPRESA } tTipoPosicion; //Array de posiciones
const int MAX_PASOS = 3, DEBUG = true, TIEMPO_PARADO = 2, LONG_CARRETERA = 10, NUM_CARRILES = 3, MAX_PODIUMS = 10; //Constantes

struct tCoche { 
	int pos;
	int tiempoParado;
};
typedef tTipoPosicion tArrayCarril[LONG_CARRETERA];
struct tCarril {
	tCoche coche;
	tArrayCarril carretera;
};
typedef tCarril tCarretera[NUM_CARRILES];
typedef int tPodium[NUM_CARRILES]; 
struct tGanadores {
	string idCarrera;
	tPodium ganadores;
};
typedef tGanadores tArrayPodiums[MAX_PODIUMS];
struct tListaPodiums { 
	int cont;
	tArrayPodiums listaPodiums;
};
bool esClavo(const tCarril carril, int x); //Cuando la posición en la que se encunetra es un Clavo.
bool esSorpresa(const tCarril carril, int x); //Cuando la posición en la que se encunetra es una Sorpresa.
void dibujaCarriles(const tCarretera carriles); //for int = 0; i numcarriles; i++ llamamos a dibujacarril(carriles[i], i)
void dibujaCarril(const tCarril& carril, int n); //Aquí pintamos las lineas de separación de los carriles
void dibujaCarretera(const tCarretera carriles); //Dibujamos la carretera completa
void dibujaLineaHorizontalSuperior();//Línea superior.
void dibujaLineaHorizontalInferior();//Línea inferior.
void iniciaCarril(tCarril& carril); //Inicializa el carril (y su coche) con todas las posicíones a tipo NORMAL
bool cargaCarriles(tCarretera carriles); //Para iniclalizar todos los carriles (pide el nombre del archivo)
void iniciaCoche(tCoche& coche); //Inicializa el coche con los valores de su posición y de su tiempo parado a 0.
void leeCarriles(ifstream& archivo, tCarretera carriles); //Realiza la carga de los carriles
void leeCarril(ifstream& archivo, tArrayCarril carril); //Carga im carril
bool enCarretera(int n); //Comprueba que el coche se encuentre en la carretera
int buscaPosicionSorpresa(const tCarril& carril, int posIni, int incr); //Busca la sorpresa a donde debe pasar el coche tras caer en una sorpresa
bool calculaPosicion(tCarril& carril); //Calcula la siguiente posición del coche teniendo en cuenta que tipo de posición sea
int avanza(int posCoche, int y); //Hace avanzar al coche según el modo en el que estemos jugando
bool haLlegado(const tCoche& coche); //Indica si ha llegado el coche a la meta
void avanzaCarriles(tCarretera carretera, int& cochesEnMeta, tPodium podium); //Indica el orden en el que avanzan los coches
tTipoPosicion stringToEnum(string s); //Para dibujar las sorpresas y los clavos
void simulaCarrera(tCarretera carretera, tPodium podium); //Responsable de que se ejecute o no la carrera
void muestraPodium(const tPodium podium); //Para mostrar en qué posición llega a la meta cada coche
bool guardaGanadores(const tListaPodiums& podiums); //Genera un archivo que guarda y posteriormente muestra los ganadores
void crearIdentificador(tListaPodiums& podiums, const tGanadores& ganadores); //Para generar el identificador de cada carrera

int main() {
	tCarretera carretera;
	tGanadores ganadores;
	tCarretera carriles;
	tListaPodiums listaPodiums;
	char respuesta;

	srand(time(NULL)); //Genera los números aleatorios
	if (LONG_CARRETERA % 2 == 0) { //Comprobamos que la carretera tiene una longitud par.
		do {
			cargaCarriles(carretera);
			simulaCarrera(carretera, ganadores.ganadores);
			guardaGanadores(listaPodiums);
			cout << "DESEA REALIZAR OTRA SIMULACION? (S / N)?" << endl; //Una vez que termina la carrera nos pregunta esto.
			cin >> respuesta;
		} while (respuesta != 'N'); //Si la respuesta a la pregunta es SI (S) se repetirá toda la carretera. El while es nuestra condición de finalización.
		if (respuesta == 'N') { //Si la respuesta es NO (N), aparecerá esto.
			cout << "GAME OVER" << endl;
			cout << "EL JUEGO HA TERMINADO.";
		}
	}
	else {//Longitud de carretera impar.
		cout << "ERROR" << endl;
	}
	return 0;
}
void iniciaCoche(tCoche& coche) { //Inicializamos el coche y el tiempo parado a 0.
	coche.pos = 0;
	coche.tiempoParado = 0;
}
void iniciaCarril(tCarril& carril) {
	iniciaCoche(carril.coche);
	for (int i = 0; i < LONG_CARRETERA; i++) {
		carril.carretera[i] = NORMAL; //Se asignan todas las posiciones del array como NORMAL
	}
}
void leeCarriles(ifstream& archivo, tCarretera carriles) { //Contiene un bucle que hace que se cargen todos los carriles
	for (int i = 0; i < NUM_CARRILES; i++) {
		iniciaCarril(carriles[i]);
		leeCarril(archivo, carriles[i].carretera);
	}
}
void leeCarril(ifstream& archivo, tArrayCarril carril) {
	int num, numeros;
	string elemento;
	archivo >> elemento;
	while (elemento != "XX") {//Mientras sea distinto del centinela.
		archivo >> num; //Leemos el número que indica la cantidad de clavos y sorpresas.
		for (int i = 0; i < num; i++) {
			archivo >> numeros; //Leemos los números de las posiciones de los clavos y las sorpresas
			carril[numeros] = stringToEnum(elemento); //Igualamos nuestro array de carretera a stringtoEnum, donde hacemos el cambio de los elementos a enumerado.				
		}
		archivo >> elemento;
	}
}
tTipoPosicion stringToEnum(string s) { //Nos permite dibujar los clavos y las sorpresas e indica de qué tipo es cada posición
	tTipoPosicion elemento;
	if (s == "CLAVO") {
		elemento = CLAVO;
	}
	else if (s == "SORPRESA") {
		elemento = SORPRESA;
	}
	else {
		elemento = NORMAL;
	}
	return elemento;
}
bool cargaCarriles(tCarretera carriles) { //Carga todos los carriles.
	string NombreArchivo;
	ifstream archivo; //Para abrir el archivo
	do {
		cout << "Dame el nombre del archivo: "; cin >> NombreArchivo; //Pedimos el nombre del archivo
		archivo.open(NombreArchivo);
	} while (!archivo.is_open());
	leeCarriles(archivo, carriles);//En el caso de que el archivo se pueda abrir sin problemas, llamamos a leeCarriles.
	archivo.close(); //Cerramos el archivo
	return(archivo.is_open());
}
bool enCarretera(int n) {
	return(0 <= n < LONG_CARRETERA);//Si n está dentro de la carretera devuelve true.
}
int buscaPosicionSorpresa(const tCarril& carril, int posIni, int incr) { //Cuando el coche cae en una posición sorpresa calcula a cuál otra sorpresa va
	bool encontrado = false;
	posIni += incr; //Sumamos a la posición inicial el incremento
	while (!encontrado && posIni > 0 && posIni < LONG_CARRETERA) {
		if (esSorpresa(carril, posIni)) {
			encontrado = true;
		}
		else
			posIni += incr;
	}
	if (!encontrado)
		posIni = 0;
	return posIni;
}
bool calculaPosicion(tCarril& carril) { //Calcula la siguiente posición del coche
	int incr, posIni = carril.coche.pos;
	bool es = false;
	if (carril.coche.tiempoParado != 0) { //La posición del coche está en un clavo
		cout << "EL SIGUIENTE COCHE SIGUE PINCHADO." << endl;
		cout << "PULSE UNA TECLA PARA CONTINUAR" << endl;
		char boton;
		boton = getchar();
		cout << "EL COCHE ESTA PINCHADO. LE QUEDAN " << carril.coche.tiempoParado << " PASOS PARA MOVERSE." << endl;
		carril.coche.tiempoParado--; //Le quede un paso menos para moverse.
	}
	else if (esClavo(carril, posIni)) {
		carril.coche.tiempoParado = TIEMPO_PARADO;
		cout << "EL COCHE HA PINCHADO. ESTARA INMOVILIZADO " << carril.coche.tiempoParado << " PASOS." << endl;
	}
	else if (esSorpresa(carril, posIni)) { //Si el coche está en una sorpresa
		cout << "PULSA UNA TECLA PARA CONTINUAR" << endl;
		char boton;
		boton = getchar();
		cout << "EL COCHE HA CAIDO EN UNA POSICION SORPRESA." << endl;
		incr = rand() % 2; // Se genera un aleatorio
		if (incr == 0) { // Si el aleatorio es un 0, lo cambiamos por un -1, y asi nos llevará a la anterior sorpresa.
			incr = -1;
			posIni += incr;
		}
		buscaPosicionSorpresa(carril, posIni, incr); //Invocamos la función y la igualamos a la posición del coche
		carril.coche.pos = buscaPosicionSorpresa(carril, posIni, incr);
		if (carril.coche.pos == 0) {
			cout << "REGRESAS AL PRINCIPIO." << endl;
			carril.coche.pos = 0;
		}
		else if (incr == 1) { //Si el incremento es 1, vas a la siguiente sorpresa
			cout << "VAS A LA PROXIMA POSICION SORPRESA. DE LA POSICION " << posIni << " A LA POSICION  " << carril.coche.pos << endl;
		}
		else {
			cout << "VAS A LA ANTERIOR POSICION SORPRESA. DE LA POSICION " << posIni << " A LA POSICION " << carril.coche.pos << endl;
		}
		es = true;
	}
	return es;
}
int avanza(int posCoche, int y) { //Para que el coche avance
	int pasos;
	if (DEBUG) { //Modo depuración.
		cout << "Introduce el numero de pasos que avanza el coche: "; //Le pregunta al usuario cuánto quiere avanzar.
		cin >> pasos;
		cout << "Avanzando en el carril " << y + 1 << " ..." << endl;
		if (pasos > MAX_PASOS) { //Cuando el número de pasos que el usuario quiere avanzar es mayor al máximo de pasos, el programa lo prohibe, el coche no avanza y te vuelve a preguntar cuánto quieres avanzar.
			cout << "NO SE PUEDEN AVANZAR TANTOS PASOS." << endl;
			cout << "Vuelve a introducir el numero de pasos que quieres avanzar: "; //Repite la pregunta.
			cin >> pasos;
			cout << "PULSE UNA TECLA PARA CONTINUAR" << endl; //Pulsar "enter" para continuar con la simulación.
			char boton;
			boton = getchar();
		}
		else
			cout << "PULSE UNA TECLA PARA CONTINUAR" << endl;
		char boton;
		boton = getchar();
		boton = getchar();
		cout << "EL COCHE AVANZA " << pasos << " PASOS." << endl; //Indica cuántos pasos va a avanzar el coche.
		if (posCoche < 0)
			posCoche = 0;
		else
			posCoche = posCoche + pasos;
	}
	else { //Modo normal (números aleatorios).
		cout << "PULSE UNA TECLA PARA CONTINUAR" << endl; //Pulsa "enter" para continuar el juego.
		char boton;
		boton = getchar();
		boton = getchar();
		pasos = 1 + rand() % MAX_PASOS; //Generación de un número aleatorio
		posCoche = posCoche + pasos;
		cout << "Avanzando en el carril " << y + 1 << "..." << endl;//Indicamos el coche que va a avanzar.
		cout << "EL COCHE AVANZA " << pasos << " PASOS." << endl;
	}
	return posCoche;
}
bool haLlegado(const tCoche& coche) { //Cuando el coche llega a la meta.
	return (coche.pos >= LONG_CARRETERA);
}
void dibujaLineaHorizontalSuperior() {
	cout << char(CHAR_ESQUINA_SUPERIOR_IZQUIERDA); //Esquina superior izquierda
	for (int i = 0; i < LONG_CARRETERA; i++) {
		cout << char(CHAR_LINEA_HORIZONTAL);
	}
	cout << char(CHAR_ESQUINA_SUPERIOR_DERECHA) << endl; //Esquina superior derecha
}
void dibujaLineaHorizontalInferior() {
	cout << char(CHAR_ESQUINA_INFERIOR_IZQUIERDA); //Esquina inferior izquierda
	for (int i = 0; i < LONG_CARRETERA; i++) {
		cout << char(CHAR_LINEA_HORIZONTAL);
	}
	cout << char(CHAR_ESQUINA_INFERIOR_DERECHA) << endl; //Esquina inferior derecha
}
void dibujaCarriles(const tCarretera carriles) { //Dibuja todos los carriles
	for (int i = 0; i < NUM_CARRILES; i++) {
		dibujaCarril(carriles[i], i);
	}
}
void dibujaCarril(const tCarril& carril, int n) { //Dibuja un carril
	int k;
	cout << char(CHAR_LINEA_VERTICAL); //Dibuja la línea vertical
	for (int i = 0; i < LONG_CARRETERA; i++) {
		if (carril.carretera[i] == CLAVO) {
			cout << char(CHAR_CLAVO); //Dibuja los clavos
		}
		else if (carril.carretera[i] == SORPRESA) {
			cout << char(CHAR_SORPRESA); //Dibuja las sorpresas
		}
		else {
			cout << char(CHAR_NORMAL);
		}
	}
	cout << char(CHAR_LINEA_VERTICAL) << endl;
	cout << char(CHAR_LINEA_VERTICAL);
	for (int k = 0; k < carril.coche.pos; k++) {
		cout << char(CHAR_NORMAL);
	}
	cout << char(CHAR_COCHE);//Dibujamos el coche.
	for (int k = 0; k < (LONG_CARRETERA - carril.coche.pos) - 1; k++) {
		cout << char(CHAR_NORMAL);
	}
	cout << char(CHAR_LINEA_VERTICAL) << endl;
	if (n < NUM_CARRILES - 1) {//Dibujamos la separación entre carriles.
		cout << char(CHAR_LINEA_VERTICAL);
		for (int i = 0; i < LONG_CARRETERA / 2; i++) {
			cout << char(CHAR_LINEA_HORIZONTAL);
			cout << " ";
		}
		cout << char(CHAR_LINEA_VERTICAL) << endl;
	}
}
void dibujaCarretera(const tCarretera carriles) { //Hace que se dibuje la carretera al completo
	dibujaLineaHorizontalSuperior();
	dibujaCarriles(carriles);
	dibujaLineaHorizontalInferior();
}
bool esClavo(const tCarril carril, int x) { //Si el coche cae en un clavo
	return (carril.carretera[x] == CLAVO);
}
bool esSorpresa(const tCarril carril, int x) { //Si el coche cae en una sorpresa
	return (carril.carretera[x] == SORPRESA);
}
void avanzaCarriles(tCarretera carretera, int& cochesEnMeta, tPodium podium) { //Indica si el coche de cada carril ha llegado o no a la meta (para que se siga moviendo o no)
	for (int i = 0; i < NUM_CARRILES; i++) {
		if (!haLlegado(carretera[i].coche) && carretera[i].coche.tiempoParado == 0) {
			dibujaCarretera(carretera);
			carretera[i].coche.pos = avanza(carretera[i].coche.pos, i);
			if (haLlegado(carretera[i].coche)) {
				carretera[i].coche.pos = LONG_CARRETERA;
				podium[cochesEnMeta] = i + 1;
				cochesEnMeta++;
				cout << "El carril " << i + 1 << " ha completado la carrera." << endl; //Para cuando un coche llega a la meta
				if (cochesEnMeta == NUM_CARRILES) {
					dibujaCarretera(carretera);
				}
			}
			calculaPosicion(carretera[i]);
		}
		else if (carretera[i].coche.tiempoParado != 0) { //Si el coche está pinchado, nos invoca calculaPosicion.
			calculaPosicion(carretera[i]);
		}
	}
}
void simulaCarrera(tCarretera carretera, tPodium podium) {//Para que el juego se ejecute.
	int cochesEnMeta = 0;
	for (int i = 0; i < NUM_CARRILES; i++)
		podium[i] = 0;
	while (cochesEnMeta < 3) {
		avanzaCarriles(carretera, cochesEnMeta, podium);
	}
	if (cochesEnMeta == 3) { //Cuando han llegado todos los coches a la meta.
		cout << "LA CARRERA HA TERMINADO." << endl;
		cout << "LOS TRES COCHES HAN LLEGADO A LA META"<<endl;
		cout << "PULSA UNA TECLA PARA VER LA CLASIFICACION" << endl;
		char boton;
		boton = getchar();
		muestraPodium(podium);
	}

}
void muestraPodium(const tPodium podium) { //Muestra el orden de llegada de los coches.
	cout << "CLASIFICACION DE LA CARRERA: " << endl;
	for (int i = 0; i < NUM_CARRILES; i++) {
		cout << "PUESTO NUMERO " << i + 1 << ": Coche en el carril " << podium[i] << endl;
	}
}
void crearIdentificador(tListaPodiums& podiums, const tGanadores& ganadores) { //Para que podamos crear el identificador de cada carrera.
	int identificador;//No funciona.
	cout << "Dame un identificador para la carrera: " << endl;
	cin >> identificador;
}
bool guardaGanadores(const tListaPodiums& podiums) { //Almacena los podiums de las distintas carreras. No hemos sabido hacer que funcione.
	bool es = false;
	ofstream fichero("ganadores.txt");
	if (fichero.is_open()) {
		for (int i = 0; i <podiums.cont ; i++) {
			fichero << podiums.listaPodiums[i].idCarrera << endl;
			for (int k = 0; k < NUM_CARRILES; k++) {
				fichero <<"Puesto " << k + 1 << ": coche en el carril "  << podiums.listaPodiums[i].ganadores[k] + 1;
			}
		}
	}
	else {
		fichero << "ERROR AL CREAR EL ARCHIVO DE GANADORES" << endl;
		es = true;
	}
	fichero.close();
	return es;
}
