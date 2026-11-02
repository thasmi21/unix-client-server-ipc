#include "CL_include"

/**********************************************
*	     LIT UNE DONNÉE ENVOYÉE PAR LE SERVEUR
*
* ENTREE: *tampon associé à la mémoire partagée avec le serveur, contient les données à récupérer
*         voie sur laquelle lire les données
*         semid identifiant du sémaphore concerné par la tentative de lecture de donnée
*         pipefd descripteur d'eriture de la voie 1 ou 2
* SORTIE: la donnée envoyée par le serveur est recopiée sur le pipe à destination du Rédacteur
* de la voie correspondante
* RETOUR: 0 Si pas d'erreur
*         -101 si erreur sur la mémoire partagée
*         -103 si erreur sur un sémaphore
*         -104 si erreur sur le pipe   
*/
int Lecture(BUF *tampon, int voie, int semid, int pipefd){

    //Vérification de la conformité des paramètres
    if (tampon->n < 0 || tampon->n > BUF_SZ -1){ //L'indice du dernier élément modifié sur le buffer circulaire est incorrecte
        printf("Error in lecteur.c/Lecture : tampon->n out of range.\n");
        return SHMerr;
    }

    if (voie < 1 || voie > 2){ //La voie indiquée n'est ni la 1 ni la 2
        printf("Error in lecteur.c/Lecture : voie must be in [1, 2].\n");
        return SEMerr;
    }

    if (semid < 0){ //Problème lors de la récupération de l'identifiant du sémaphore
        printf("Error in lecteur.c/Lecture : semid < 0.\n");
        return SEMerr;
    }

    if (pipefd < 0){ //Problème lors de la récupération du descripteur d'ecriture
        printf("Error in lecteur.c/Lecture : pipefd < 0.\n");
        return PIPerr;
    }

    //Vérifier l'accès à la voie avec l'opération P
    if (P(semid, voie) < 0){
        printf("Error in lecteur.c/Lecture : the semaphore (P) operation failed.\n");
        return SHMerr;
        //TO DO : un peu violent d'arreter le programme si un client est deja en train de lire les données non ?
    }

    //Récupérer la donnée sur la voie spécifiée depuis le dernier emplacement utilisé
    int n = (tampon + voie)->n;              
    int data = (tampon + voie)->tampon[n];
    printf("Donnée lue sur la voie %d : %d.\n", voie, data);

    // Envoyer la donnée au Rédacteur via le pipe
    if (write(pipefd, &data, sizeof(int)) < 0){ //Problème pour ecrire la donnée sur le pipe
        printf("Error in lecteur.c/Lecture : Lecteur %d failed to write in the pipe.\n", voie);
        return PIPerr;
    }

    //Libérer l'accès avec l'opération V
    if (V(semid, voie) < 0){ //La voie n'est pas libérable
        printf("Error in lecteur.c/Lecture : the semaphore (V) operation failed.\n");
        return SHMerr; 
    }

    return NOERR_err; 
}