#ifndef _SEM_H_
#define _SEM_H_

//Crée un sémaphore ou en rejoint un si il est déjà existant
//et renvoie son identifiant (ou SEMerr)
int AccessMutex(void);

//Détruit le sémaphore dont l'identifiant est renseigné en paramètre
int DestructionMutex(int semid);

//Initialise l'état (LIBRE ou OCCUPÉ) du sémaphore rentré en paramètre
int Init_Mutex(int semid, int nb);

//Test si la voie "voie"  du sémaphore est bien libre à la lecture 
int P(int semid, int voie);

//Test si il est bien possible de libérer la voie "voie" du sémaphore 
int V(int semid, int voie);

#endif