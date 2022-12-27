#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#define max 100
#define Dmax 100

struct parametri {

	int passi, D;

} p;

void assegna(struct parametri*, char*[]);

int scelta();

double dist();

struct particella{

	int ix[Dmax];

} part;

typedef struct memoria {

	int coord[Dmax], controllo[Dmax];

} campo;


int check();

void walk();

int genInt();

void stampa();

int fine();

void inizializza();

double genDouble();

void generacontrollore();

void aggiorna();

int main(int argc, char*argv[]) {

	if (argc != 3) {

		printf("ERROR! Syntax: %s passi dimensioni\n", argv[0]);

		exit(EXIT_FAILURE);
		
	}

	int i, tempo=0, distanza, cicli, s1, s2, s3, s4, s5, f;

	double c=0, sqmedio;

	char stringa[max], word[max];

	FILE*fin;

	FILE*fout;

	FILE*nebul;
	
	FILE*v;

	srand48(time(0));

	assegna(&p, argv);
	
	cicli = scelta("Inserire numero di cicli:", 1, 100000);

	s1 = scelta("Creare file passi prob distmedia?\n1.Si' 2.No:", 1, 2);

	if (cicli==1) s2 = scelta("Creare file percorso?\n1.Si' 2.No:", 1, 2);

	s3 = scelta("Creare file con posizioni finali dei camminatori?\n1.Si 2.No", 1, 2);

	s4 = scelta("Creare file con camminatori vivi in funzione dei passi disponibili?\n1.Si 2.No", 1, 2);

	s5 = scelta("Far compiere tutti i passi possibili ai camminatori?\n1.Si 2.No", 1, 2);/*Questa opzione e' necessaria per poter calcolare
												in modo efficiente lo spostamento quadratico 													medio.*/

	if (s1 == 1 ) {

		sprintf(stringa, "saw_%dD_%d", p.passi, p.D);

		fin = fopen(stringa, "w");

	}

	if (s2 == 1 ) {

		sprintf(word, "percorso_%d_%d", p.passi, p.D);

		fout = fopen(word, "w");

	}

	if (s3 == 1) {

		nebul = fopen("EndPos", "w");

	}

	if (s4 == 1) {

		v = fopen("Vivi", "w");

	}

	campo*state;

	state = (campo*) calloc(p.passi, sizeof(campo));

	generacontrollore(state);

	tempo = p.passi;

	p.passi = 0;

	while (p.passi<tempo+1) {

		c = 0;

		distanza = 0;

		sqmedio = 0;

		for (i=0;i<cicli;i++) {

			f = 0;

			inizializza(state);

			walk(state, &c, fout, &distanza, s2, &tempo, s3, nebul, &f, cicli);

			if (s5==1) {/*Se l'utente vuole che ogni camminatore percorra tutti i passi e' necessario far retrocedere il for 
					di un passo ogni volta che il camminatore si perde (f controlla la buona riuscita del cammino)*/

				if (f==0) sqmedio += dist();

				if (f!=0) { 

					i--;

					c--;
	
				}

			}

			sqmedio += dist();

		}

		if (s1 == 1 ) fprintf(fin, "%d %lf %lf %lf\n", p.passi, c/cicli, (double)distanza/cicli, sqmedio/(cicli));

		printf("passo:%d\n", p.passi);

		if ( s4 == 1 ) fprintf(v, "%d %lf\n", p.passi, cicli-c);

		p.passi += 1;

	}

	return 0;

}

void assegna(struct parametri*p, char *argv[]) {

	p->passi = (int) atoi(argv[1]);

	p->D = (int) atoi(argv[2]);

}

int check(campo*state, int N, int passo, int asse) {

	/*La funzione check prende in argomento l'asse lungo il quale ci si vuole spostare e controlla che la posizione verso cui si e'
	diretti non sia gia' stata precedentemente occupata*/

	int i, j, a, b=0;

	for (j=0;j<N;j++) {

		a = 0;

		for (i=0;i<p.D;i++) {

			if ( i != asse ) {

				if ((state[N-1].coord[i]) == state[j].coord[i]) {

					a++;

				}
	
			} else {

				if ((state[N-1].coord[i] + passo) == state[j].coord[i]) {

					a++;

				}

			}

		}

		if ( a == p.D ) b = 1;

	}

	return b;

}

int genInt() {

	return lrand48()%p.D;

}

double genDouble() {

	return (double) lrand48()/RAND_MAX;

}

void walk(campo*state, double*c, FILE*fout, int *distanza, int s2, int *tempo, int s3, FILE*nebul, int *f, int cicli) {

	int i=0, asse, successo, libero=0, gioco=0, t, last_i=0;

	double direzione;

	/*In questo while si controlla semplicemente che non si superi il numero di passi e che il gioco (ossia la possibilita' di muoversi) 
	non sia finito*/

	while ( gioco == 0 && i<p.passi ) {

		successo = 0;

		if (s2 == 1 && p.passi == *tempo && cicli == 1) stampa(state, i, fout);
		
		i++;

		last_i = i;

		t = 0;

		*distanza += 1;/*Se si e' arrivati fin qui ci si muovera' sicuramente quindi si aumenta la distanza percorsa.*/

		/*In questo while si generano possibili direzioni lungo cui muoversi finche' non ci si muove, ossia finche' successo==1.*/

		while (successo==0) {

			asse = genInt();/*Scegliamo una dimensione lungo cui provare a spostarci*/

			direzione = genDouble();/*Scegliamo se muoverci verso il positivo o verso il negativo*/

			/*Nelle seguenti righe si valuta semplicemente se il nuovo stato che si vuole assumere non fosse gia' stato
			assunto in precedenza (quindi il controllo va fatto solo per i>1) e, se possibile, si aggiornano le coordinate
			della particella nel nuovo stato.*/ 

			if ( direzione < 0.5 ) {

				if (i>1) libero = check(state, i, 1, asse);

				if ( libero == 0 ) {

					part.ix[asse]++;

					state[i].coord[asse] = part.ix[asse];

					successo = 1;

					aggiorna(state, i, asse);

				}

			} else  {

				if (i>1) libero = check(state, i, -1, asse);

				if ( libero == 0 ) {
					
					part.ix[asse]--;

					state[i].coord[asse] = part.ix[asse];

					successo = 1;

					aggiorna(state, i, asse);

				}

			}

			t++;/*t conta quante volte si e' generata una possibile direzione lungo cui muoversi*/

			/*Se t diventa troppo grande allora si controlla che il gioco non sia finito. Nel caso in cui lo fosse (gioco!=0) 
			allora si impone successo=1 per uscire dal while piu' interno e tornare nel while piu' esterno e chiudere la 				funzione.*/

			if (t>5*pow(p.D,p.D)) {

				gioco = fine(state, i);

				if ( gioco != 0 ) { 	

					successo = 1;

					*c += 1;

					*f = 1;

				}

			}

		}
	
		/*Questo contatore tiene conto del numero delle volte in cui la particella si e' intrappolata.*/

	}

	if (s3==1 && p.passi == *tempo) { 

		for (i=0;i<p.D;i++) {

			fprintf(nebul, "%d\t", state[last_i-1].coord[i]);

		}

		fprintf(nebul, "\n");

	}

}

int fine(campo*state, int N) {

	int i, a, b=0, j, k;

	/*Accedendo a questo for si controllano tutti gli stati che la particella ha assunto fino a questo istante*/

	for (j=0;j<N;j++) {

		/*Nel for successivo si controllano tutte le possibili direzioni (es: se siamo in due dimensioni possiamo muoverci lungo 
		x e y con verso positivo e negativo per un totale di 2*dimensioni possibilita').
		Oltretutto si setta un contatore a=0*/

		for (i=0;i<2*p.D;i++) {

			a = 0;

			/*Il for successivo e' il cuore del controllo. Usando i%2 ci si assicura che se i e' pari allora ci si sposta 
			in direzione positiva mentre se i e' dispari ci si muove in direzione negativa. Usando i controllori (ossia i
			vettori della base canonica) si permette di effettuare il controllo lungo tutte le possibili direzioni e lungo
			tutti i possibili assi. Se al termine di questo for a==p.D, ossia muovendosi lungo una data direzione si e' 
			incontrato uno stato gia' assunto in precedenza il contatore b aumenta di 1.*/

			for (k=0;k<p.D;k++) {

				if (i%2 == 0) {

					if ((state[N-1].coord[k] + state[i/2].controllo[k] == state[j].coord[k])) {

						a++;

					}

				} else {

					if ((state[N-1].coord[k] - state[i/2].controllo[k] == state[j].coord[k])) {

						a++;

					}

				}
			
			}

			if ( a == p.D ) b += 1;

		}				

	}

	/*Per assicurarsi che la particella sia veramente intrappolata e' sufficiente controllare che b sia uguale a tutte le possibili 
	direzioni in cui essa potrebbe muoversi, ossia 2*dimensioni; se cosi' non dovesse essere allora si ritorna 0 e la ricerca di nuove
	posizioni da occupare continua*/

	if (b==2*p.D) {

		b = 1; 

	} else {

		b = 0;

	}

	return b;

}

void inizializza(campo*state) {

	int i, j;

	/*In questa funzione si azzerano tutti i possibili stati che la 
	particella potra' assumere e si impongono le condizioni iniziali 		
	(particella nell'origine)*/

	for (i=0;i<p.passi;i++) {

		for (j=0;j<p.D;j++) {

			state[i].coord[j] = 0;

		}

	}

	for (i=0;i<p.D;i++) {

		part.ix[i] = 0;

	}

}

void stampa(campo*state, int N, FILE*fout) {

	int j;

	/*Viene stampato su file la sequenza di posizioni espresse in coordinate cartesiane che la particella assume*/

	for (j=0;j<p.D;j++) {

		fprintf(fout, "%d  ", state[N].coord[j]);

	}

	fprintf(fout, "\n");

}

void aggiorna(campo*state, int N, int asse) {

	int j;

	/*In questa funzione viene passata la dimensione (asse) che la particella non deve aggiornare 
	e si aggiornano le altre prendendo le posizioni dello stato precedente*/

	for (j=0;j<p.D;j++) {

		if ( j != asse) state[N].coord[j] = state[N-1].coord[j];

	}

}

void generacontrollore(campo*state) {

	int i, j;

	/*In questa funzione vengono generati dei "controllori", ossia dei vettori della base canonica (l'utilizzo e' spiegato nella
	funzione "fine"*/

	for (i=0;i<p.D;i++) {

		for (j=0;j<p.D;j++) {

			if (j==i) {

				state[i].controllo[j] = 1;

			} else {
			
				state[i].controllo[j] = 0;

			}

		}

	}

}

int scelta(char stringa[max], int a, int b) {

	int c;

	printf("%s\n", stringa);

	do {

		scanf("%d", &c);

		if ( c < a || c > b)  printf("Valore errato! Riprovare:\n");

	} while ( c < a || c > b );

	return c;

}

double dist() {

	int i;

	double c=0;

	for (i=0;i<p.D;i++) {

		c += (part.ix[i]*part.ix[i]);

	}

	return c;

}
