// //Création d'un masque pour bloquer les potentiels prochains signaux le temps de traiter celui actuel
		// sigset_t block;
		// sigemptyset(&block);
		// sigaddset(&block, SIGUSR1);
		// sigaddset(&block, SIGUSR2);
		// printf("Création des masques pour bloquer les signaux (l1).\n");
		while (1){
			// pause(); //Attente du signal SIGUSR1
			sleep(1);
			// sigprocmask(SIG_BLOCK, &block, NULL); //Réception d'un signal donc blocage des prochains
			if (voie == 1){
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