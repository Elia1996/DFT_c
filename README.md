La funzione da utilizzare è presente nel file src/HeartBeat.c e si chiama
HeartBeat, il suo prototipo è il seguente:
	uint8_t HeartBeat(int16_t value, uint16_t* BPM, uint16_t n_of_sample, uint16_t fs, uint8_t reset)

Questa funzione riceve:
   value -> valore corrente di corrente del sensore di battito
   BPM   -> variabile in cui verrà scritto il valore di BPM nel caso in cui sia calcolato
   n_of_sample -> quando la funzione viene chiamata la prima volta crea un vettore di NSAMPLE elementi
                     e vi salva il primo valore nella prima posizione, ogni volta che la funzione viene chiamata
                     essa salva il nuovo valore passato nel vettore nelle posizioni successive.
                     Quando il numero di valori passati arriva a SAMPLEUSED+n_of_sample il vettore viene processato
                     ricavando il valore del battito corrente.
          punto 1)   Successivamente ad ogni chiamata viene cancellato l'elemento più vecchio del vettore
                     e viene salvato quello nuovo passato alla funzione, quando il numero di valori scritti 
					 nel vettore(dopo all'ultima elaborazione) arriva a n_of_sample viene fatto un nuovo calcolo del BPM.
                     Successivamente si ripete dal punto 1
					
					 Non è stata testata la funzione nel caso in cui questo
					 parametro cambi durante il funzionamento, percui se non si
					 resetta prima di cambiarlo, il programma potrebbe o meno
					 continuare a funzionare.
			
	fs -> frequenza di campionamento del dato di corrente nel sensore
	reset -> se resettato resetta la funzione

 Se Tc è il periodo con cui si chiama la funzione allora si avrà inizialmente un BPM valido dopo
 Tc*SAMPLEUSED sec dalla prima chiamata e poi ogni Tc*n_of_sample sec, questo è stato fatto per aumentare il numero
 di campioni utilizzati e aumentare la precisione, allo stesso tempo viene data la possibilità di scegliere ogni
 quanto calcolare il battito.
 La costante SAMPLEUSED è definita nel file CommonInclude.h, può essere
 modificata ma deve rimanere in questo range:
 	0 < SAMPLEUSED < 2048 - n_of_sample
Poichè non sono stati fatti controlli su questa condizione se non viene
rispettata il programma non funziona :)

Il ritorno della funzione è:
	1 -> dato valido
	0 -> dato non valido
