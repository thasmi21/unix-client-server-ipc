#include "CL_include"

void Redaction(int pipefd_LR, int pipefd_RD, int semid2, int voie){
    int data[5];
    int count = 0;

    while (1){ //Boucle infinie tant que le processus père n'est pas fermé
        //Extrait une donnée du pipe dès qu'elle est transmise
        if (read(pipefd_LR, &data[count], sizeof(int)) > 0){
            count += 1;
        }
        else {
            printf("Error in redacteur.c/Redaction : Redacteur %d failed to read in the pipe.\n", voie);
            return SHMerr;
        }

        // Si 5 données sont prêtes, les transmettre au Driver
        if (count == 5){
            //Demande l'accès au Driver
            //Vérifier l'accès à la voie avec l'opération P
            if (P(semid2, voie) < 0){
                printf("Error in redacteur.c/Redaction : the semaphore (P) operation failed.\n");
                return SHMerr;
                //TO DO : un peu violent d'arreter le programme si un client est deja en train de lire les données non ?
            }

            // Envoyer la donnée au Driver via le pipe
            if (write(pipefd_RD, &data, sizeof(int)) < 0){ //Problème pour ecrire la donnée sur le pipe
                printf("Error in redacteur.c/Redaction : Redacteur %d failed to write in the pipe.\n", voie);
                return PIPerr;
            }

            //Libérer l'accès avec l'opération V
            if (V(semid2, voie) < 0){ //La voie n'est pas libérable
                printf("Error in redacteur.c/Redaction : the semaphore (V) operation failed.\n");
                return SHMerr; 
            }
            
            printf("Les 5 données à envoyer au Driver sont :\n");
                for (int i=0; i<5; i++){
                    printf("donnée %d : %d.\n", i, data[i]);
                }                
            }
            count = 0;
        }
}