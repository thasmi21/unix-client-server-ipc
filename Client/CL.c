#include "Client/CL_include"

void litbuf1();
void litbuf2();

int voie = 0;
int Cptr, msqid, shmid;	         /*variable globale (numero du canal a lire)*/


int main(int argc, char *argv[])
{

	if (argc < 2){ //Vérifie que 2 paramètres ont bien été renseignés dans l'appel du main()
		printf("Le nombre de paramètres doit être de 2.\n");
		exit(EXIT_FAILURE);
	}

	int nb_iteration = atoi(argv[1]);
	if (nb_iteration < 1){ //Le client doit être appelé à s'exécuter pendant au moins une seconde
		printf("Le nombre d'itérations doit être positif.\n");
		return -1;
	}

	printf("\n* * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("Projet CLIENT - SERVEUR\tTemps R�el E2i - Mathis Hurstel, Novembre 2024\n");
	printf("* * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("\nLancement du client pendant %d secondes\n", nb_iteration);

	//Indique le PID du client et lance la tentative de connexion au serveur
	Cptr = getpid();
	printf("\nLe PID du client est %d\n", Cptr);
	int err = Connexion(&Cptr, &shmid, &msqid);
	if (err < 0){ //Un return autre que 0 indique une erreur incompatible à la connexion du client
		printf("* *Tentative de connexion echouée.* *\n");
		return -1;
	}

	//Création d'un tampon qu'on relie à la mémoire partagée du serveur
    BUF *tampon = (BUF *)shmat(shmid, NULL, SHM_RDONLY);
    if (tampon == (void *)-1) { //Echec de la liaison entre le tampon et la mémoire partagée
        printf("Error in CL.C/main : the buffer could not be linked to the shared memory.\n");
        return SHMerr;
    }
	printf("Tampon relié à la mémoire partagée du serveur.\n");

    //Connexion au sémaphore relié à ce tampon
    int semid = AccessMutex();
    if (semid < 0){ //Le sémaphore n'a pas pu être crée
        printf("Error in CL.C/main : the mutex to the shared memory could not be created.\n");
        return SEMerr;
    }
	printf("Connexion au sémaphore du serveur réussie, semid = %d.\n", semid);

	printf("\n* * Création des pipes * *\n");

	//Création des pipes entre les Lecteur et Redacteur respectifs 
	int pipefd_LR1[2];
	if (pipe(pipefd_LR1) == -1){ //Le pipe n'a pas pu être crée
        perror("Error in CL.c/main : Impossible to create the pipe LR1.\n");
        return PIPerr; 
    }

	int pipefd_LR2[2];
	if (pipe(pipefd_LR2) == -1){ //Le pipe n'a pas pu être crée
        perror("Error in CL.c/main : Impossible to create the pipe LR2.\n");
        return PIPerr; 
    }
	printf("Création des pipes réussie : P1 = [%d, %d] et P2 = [%d, %d].\n", pipefd_LR1[0], pipefd_LR1[1], pipefd_LR2[0], pipefd_LR2[1]);

	printf("\n* * * * * Début de réception des données * * * * *\n");

	//Armement des gestionnaires de signaux
	signal(SIGUSR1, litbuf1);
	signal(SIGUSR2, litbuf2);
	printf("Armement des gestionnaires de signaux.\n");

	//Création des processus Lecteur1 et Lecteur2
	int l1 = fork();
	if (l1 == -1){ //Le lecteur n'a pas pu être crée
		printf("Error in CL.C/main : Lecteur1 could not be forked.\n");
		return -1;
	}
	printf("Lecteur 1 crée : pid = %d.\n", l1);
	if (l1 == 0){ //On est dans le processus fils qui boucle à l'infini (jusqu'à ce que le processus père se ferme)
		close(pipefd_LR1[0]); //On ne garde que le mode écriture des pipes pour les 2 processus Lecteur
		printf("fermeture du mode lecture de l1.\n");

		while (1){
			if (voie == 1 && wait == 1){
				int err = Lecture(tampon, 1, semid, pipefd_LR1[1]); //Lecture des données sur la voie 1
				voie = 0; //Réinitialisation de la voie
				if (err < 0) { //Une erreur quelconque a empêché la bonne lecture des données
					printf("Error in CL.c/main : Imposible to read the data on track 1.\n");
					// sigprocmask(SIG_UNBLOCK, &block, NULL);
					return -1;
				}
			}
			// sigprocmask(SIG_UNBLOCK, &block, NULL); //Déblocage des futurs signaux car traitement fini

		}
	}

	int l2 = fork();
	if (l2 == -1){ //Le lecteur n'a pas pu être crée
		printf("Error in CL.C/main : Lecteur2 could not be forked.\n");
		return -1;
	}
	printf("Lecteur 2 crée : pid = %d.\n", l2);
	if (l2 == 0){ //On est dans le processus fils qui boucle à l'infini (jusqu'à ce que le processus père se ferme)
		close(pipefd_LR2[0]); //On ne garde que le mode écriture des pipes pour les 2 processus Lecteur
		printf("fermeture du mode lecture de l2.\n");

		// //Création d'un masque pour bloquer les potentiels prochains signaux le temps de traiter celui actuel
		// sigset_t block;
		// sigemptyset(&block);
		// sigaddset(&block, SIGUSR1);
		// sigaddset(&block, SIGUSR2);
		// printf("Création des masques pour bloquer les signaux (l2).\n");
		while (1){
			// pause(); //Attente du signal SIGUSR2
			sleep(1);
			// sigprocmask(SIG_BLOCK, &block, NULL); //Réception d'un signal donc blocage des prochains
			if (voie == 2){
				int err = Lecture(tampon, 2, semid, pipefd_LR1[2]); //Lecture des données sur la voie 2
				voie = 0; //Réinitialisation de la voie
				if (err < 0) { //Une erreur quelconque a empêché la bonne lecture des données
					printf("Error in CL.c/main : Imposible to read the data on track 2.\n");
					// sigprocmask(SIG_UNBLOCK, &block, NULL);
					return -1;
				}
			}
			// sigprocmask(SIG_UNBLOCK, &block, NULL); //Déblocage des futurs signaux car traitement fini
		}
	}

	// //Création du sémaphore d'accès au Driver
    // int semid2 = AccessMutex();
    // if (semid2 < 0){ //Le sémaphore n'a pas pu être crée
    //     printf("Error in CL.C/main : the mutex to the Driver could not be created.\n");
    //     return SEMerr;
    // }
	////Initialisation de ce sémaphore : il est disponible
	// Init_Mutex(semid2, 1)
	// printf("Création du sémaphore d'accès au Driver : semid = %d.\n", semid2);

	// //Création des processus Redacteur1 et Redacteur 2
	// int r1 = fork();
	// if (r1 == -1){ //Le rédacteur n'a pas pu être crée
	// 	printf("Error in CL.C/main : Redacteur1 could not be forked.\n");
	// 	return -1;
	// }
	// if (r1 == 0){ //On est dans le processus fils
	// 	close(pipefd_LR1[1]); //On ne garde que le mode lecture des pipes pour les 2 processus Lecteur

	// //Création des pipes entre le Redacteur1 et le Driver 
	// int pipefd_RD1[2];
	// if (pipe(pipefd_RD1) == -1){ //Le pipe n'a pas pu être crée
	// 	perror("Error in CL.c/main : Impossible to create the pipe RD1.\n");
	// 	return PIPerr; 
	// }
	// close(pipefd_RD1[0]);
	// Redaction(pipefd_LR[0], pipefd_RD[1] semid2);
	// }

	// int r2 = fork();
	// if (r2 == -1){ //Le rédacteur n'a pas pu être crée
	// 	printf("Error in CL.C/main : Redacteur2 could not be forked.\n");
	// 	return -1;
	// }
	// if (r2 == 0){ //On est dans le processus fils
	// 	close(pipefd_LR2[1]); //On ne garde que le mode lecture des pipes pour les 2 processus Lecteur

	//Création des pipes entre le Redacteur2 et le Driver
	// int pipefd_RD2[2];
	// if (pipe(pipefd_RD2) == -1){ //Le pipe n'a pas pu être crée
	// 	perror("Error in CL.c/main : Impossible to create the pipe RD2.\n");
	// 	return PIPerr; 
	// }
	// close(pipefd_RD2[0]);
	// Redaction(pipefd_LR2[0], pipefd_RD[1] semid2);
	// }

	// time_t fin = time(NULL) + nb_iterations;
	// while (time(NULL) < fin){
	//  pause();
	// }

	// kill(l1, SIGKILL);
	// kill(l2, SIGKILL);
	// kill(r1, SIGKILL);
	// kill(r2, SIGKILL);
	// Deconnection(Cptr, *msqid);

	// return 0;
}	   	   

	   	   	   
			   
/*/////////////////////////////////////////////////////////////////////////*/
	   
void litbuf1() /* semaphore 1 */
{
	voie=1;
}


/*/////////////////////////////////////////////////////////////////////////*/

void litbuf2() /* semaphore 2 */
{
	voie=2;
}
