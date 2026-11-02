#include "CL_include"

/****************	CREATION DU MUTEX
 *  ENTREE: Neant
 *  SORTIE: le sémaphore est crée/rejoint
 *  RETOUR: semid = identifiant de la famille de sémaphores
 *          -103 si l'opération echoue
 */
int AccessMutex(){

    //Création d'une clé d'accès liée au chemin d'accès et de son nom
    key_t CleSEM;
    if ((CleSEM = ftok(CleServeur, 'S')) < 0){ //Problème lors de la création de la clé d'accès au sémaphore
        printf("Error in sem.c/CreationMutex : Impossible to create the CleSEM.\n");
        return SEMerr;
    }
    //Création de son identifiant unique
    int semid = semget(CleSEM, NVOIES, IPC_CREAT);
    if (semid < 0){ //Problème lors de la récupération de l'identifiant du sémaphore
        printf("Error in sem.c/CreationMutex : Impossible to access the mutex ID.\n");
        return SEMerr;
    }
    printf("Accès au sémaphore d'identifiant semid=%d confirmé.\n", semid);
    return semid;
}

/*****************	DESTRUCTION DU MUTEX
 *  ENTREE: semid = identificateur de la famille de sémaphores
 *  SORTIE: néant
 *  RETOUR: code erreur -1 ou 0
 */
int DestructionMutex(int semid){
    if (semid < 0){ //Problème lors de la récupération de l'identifiant du sémaphore
        printf("Error in sem.c/DestructionMutex : semid < 0.\n");
        return SEMerr;
    }
    printf("Destruction du sémaphore d'identifiant semid=%d confirmée.\n", semid);
    return (semctl(semid, 0, IPC_RMID, 0));
}

/*********************	Opération P
 *  ENTREE: semid = identificateur de la famille de sémaphores
 *  SORTIE: néant
 *  RETOUR: 0 si la voie est accessible ou -103 sinon
 */
int P(int semid, int voie){

    if (semid < 0){ //Problème lors de la récupération de l'identifiant du sémaphore
        printf("Error in sem.c/P : semid < 0.\n");
        return SEMerr;
    }

    struct sembuf semoper;
    semoper.sem_num = voie;
    semoper.sem_op = -1;
    semoper.sem_flg = 0;
  
    if (semop(semid, &semoper, 1) < 0){ //Un autre processus est déjà en train de lire/écrire sur cette voie
        printf("Error in sem.c/P : P error on the mutex.\n");
        return SEMerr;
    }

    if (voie < 1 || voie > 2){
        printf("Error in sem.c/P : voie must be in [1, 2].\n");
        return SEMerr;
    }
    
    return 0; //Le lecteur peut lire la donnée sur la voie renseignée
}

/*********************	Opération V
 *  ENTREE: semid = identificateur de la famille de sémaphores
 *  SORTIE: néant
 *  RETOUR: 0 si la voie est libérable ou -103 sinon
 */
int V(int semid, int voie){

    if (semid < 0){ //Problème lors de la récupération de l'identifiant du sémaphore
        printf("Error in sem.c/V : semid < 0.\n");
        return SEMerr;
    }

    if (voie < 1 || voie > 2){
        printf("Error in sem.c/V : voie must be in [1, 2].\n");
        return SEMerr;
    }

    struct sembuf semoper;
    semoper.sem_num = voie;
    semoper.sem_op = 1;
    semoper.sem_flg = 0;
    
    if (semop(semid, &semoper, 1) < 0){ // La voie était déjà libre
        printf("Error in sem.c/V : V error on the mutex.\n");
        return SEMerr;
    }
    
    return 0; //Cette voie est de nouveau accessible à la lecture par un processus
}


/*********************	Initialisation du sémaphore
 *  ENTREE: semid = identifiant de la famille de sémaphores
 *  SORTIE: néant
 *  RETOUR: 0 si l'opération est réussie ou -103 sinon
 */
int Init_Mutex(int semid, int nb){

    if (semid < 0){ //Problème lors de la récupération de l'identifiant du sémaphore
        printf("Error in sem.c/Init_Mutex : semid < 0.\n");
        return SEMerr;
    }

    if (nb != -1 || nb != 1){
        printf("Error in sem.c/Init_Mutex : nb must be in {-1, 1}.\n");
        return SEMerr;
    }

    struct sembuf semoper;
    int i;
    for(i = 0; i < NVOIES; i++){
        semoper.sem_num = i;
        semoper.sem_op = nb;
        semoper.sem_flg = 0;
        
        if (semop(semid, &semoper, 1) < 0){ //Le sémaphore ne peut pas être initialisé
            printf("Error in sem.c/Init_Mutex : the mutex could not be initialized.\n");
            return SEMerr;
        }
    }
    printf("Sémaphore initialisé à la valeur %d.\n", nb);
    return 0;
}