#include "CL_include"

/**********************************************
*	ENVOIE AU SERVEUR UNE DEMANDE DE CONNEXION
*
* ENTREE: *ptr = PID du client (entier naturel)
* SORTIE: Si il reste de la place sur le serveur, le client reçoit la clé d'accès privée et se connecte au serveur
*         Sinon, il se déconnecte
* RETOUR: 0 Si pas d'erreur
*         1 Si le client a du se déconnecter
*         -100 Si erreur     
*/
int Connexion(int* ptr, int* shmid, int* msqid){

    //Vérification de la conformité de *ptr
    if (*ptr < 0){
        printf("Error in moniteur.c/DemandeConnexion : PID < 0.\n");
        return CLEerr;
    }
    
    //Calcul le nombre de chiffres du PID du client
    int ptr_sz = 0;
    int temp = *ptr;
    if (*ptr == 0){
        ptr_sz = 1;
    }
    else {
        while (temp > 0){
            ptr_sz += 1;
            temp/= 10;
        }
    }
    if (ptr_sz > L_MSG){ //Le PID du client est trop grand pour pouvoir être écrit en 1 message
        printf("Error in moniteur.c/DemandeConnexion : PID too big, L_MSG = 200.\n");
        return CLEerr; 
    }

    // Génère la clé d'accès unique propre à la messagerie du serveur
    key_t key = ftok(CleServeur, 'M'); 
    if (key < 0){ //Problème d'accès au chemin/fichier fourni
        printf("Error in moniteur.c/DemandeConnexion : impossible to generate the public key\n");
        return CLEerr; 
    }
    //Accède à la file de messages du serveur
    *msqid = msgget(key, IPC_CREAT); 
    if (*msqid < 0){ //Problème de récupération de l'identifiant de messagerie
        printf("Error in moniteur.c/DemandeConnexion : msqid < 0\n");
        return CLEerr;
    }

    //Création du message de demande de connnexion 
    printf("\n* * Demande de Connexion au serveur * *\n");
    dmsgbuf message;
    message.type = CONNECT; //Message de type CONNECT
    snprintf(message.txt, L_MSG, "%d", *ptr); //Convertit le PID du client en une chaine de caractère
    if (msgsnd(*msqid, &message, ptr_sz, 0) == -1){ //Problème lors de l'envoie du message
        printf("Error in moniteur.c/DemandeConnexion : Impossible to send the message\n");
        return MSGerr;
    }

    //Attente de la réception du message de type PID envoyé par le serveur
    dmsgbuf reponse;
    reponse.type = *ptr;
    if ((msgrcv(*msqid, &reponse, L_MSG, reponse.type, 0)) < 0){ //Problème lors de la réception de la réponse
        printf("Error in moniteur.c/DemandeConnexion : Impossible to receive a response from the server\n");
        return MSGerr;
    }
    if (reponse.type != *ptr){ //Le message reçue par le client n'est pas du type (PID) espéré
        printf("Error in moniteur.c/DemandeConnexion : Client received the wrong type of answer (%ld).\n", reponse.type);
        return MSGerr;
    }

    //Traitement de la réponse 
    message.type = 4; //Message de type ACK
    if (msgsnd(*msqid, &message, ptr_sz, 0) == -1){ //Problème lors de l'envoie du message
        printf("Error in moniteur.c/DemandeConnexion : Impossible to confirm the reception of the server answer\n");
        return MSGerr;
    }
    if (strlen(reponse.txt) == 0){ //Trop de clients sont déjà connectés au serveur ou la clé reçue est vide
        printf("Error in moniteur.c/DemandeConnexion : Too many clients already connected or key empty\n");
        return CLEerr;
    }
    else { //Le client a bien recu la clé d'accès au serveur pour s'y connecter

        printf("* * Clé d'accès reçue * *\n");
        // Génère la clé d'accès unique propre au serveur
        key_t key2 = ftok(reponse.txt, 'T'); 
        if (key2 == -1){ //Problème d'accès au chemin/fichier fourni
            printf("Error in moniteur.c/DemandeConnexion : impossible to generate the private key\n");
            return CLEerr; 
        }
        //Accède au serveur
        int ServeurId = msgget(key2, IPC_CREAT | 0666); 
        if (ServeurId < 0){ //Problème de récupération de l'identifiant du serveur
            printf("Error in moniteur.c/DemandeConnexion : ServeurId < 0\n");
            return CLEerr;
        }
        printf("* * Connexion au serveur réussie * *\n");

        //Accède à la mémoire partagée
        if ((*shmid = shmget(key2, 0, IPC_CREAT | 0666)) == -1){ //Problème lors de la génération de l'identifiant de la mémoire partagée
            printf("Error in moniteur.c/DemandeConnexion : Impossible to create the memory ID.\n");
            return SHMerr;
        }
        printf("* * Accès à la mémoire partagée réussi * *\n");

        printf("\n* * * * * Connexion réussie * * * * *\n\n");
        return NOERR_err;
    }    
}

/**********************************************
*	ENVOIE AU SERVEUR D'UNE DEMANDE DE DECONNEXION
*
* ENTREE: *ptr = PID du client (entier naturel)
* SORTIE: le serveur retire le client de la liste
* RETOUR: 1 Si pas d'erreur
*         -100 Si erreur     
*/
int Deconnexion(int *ptr, int msqid){

    //Vérification de la conformité de *ptr
    if (*ptr < 0){
        printf("Error in moniteur.c/Deconnexion : PID < \n");
        return CLEerr;
    }

    //Calcul le nombre de chiffres du PID du client
    int ptr_sz = 0;
    int temp = *ptr;
    if (*ptr == 0){
        ptr_sz = 1;
    }
    else {
        while (temp > 0){
            ptr_sz += 1;
            temp/= 10;
        }
    }
    if (ptr_sz > L_MSG){ //Le PID du client est trop grand pour pouvoir être écrit en 1 message
        printf("Error in moniteur.c/Deconnexion : PID too big\n");
        return CLEerr; 
    }

    //Création du message de demande de deconnnexion 
    dmsgbuf message;
    message.type = 2; //Message de type DECONNECT
    snprintf(message.txt, L_MSG,  "%d", *ptr); //Convertit le PID du client en une chaine de caractère
    if (msgsnd(msqid, &message, ptr_sz, 0) == -1){ //Problème lors de l'envoie du message
        printf("Error in moniteur.c/Deconnexion : Impossible to send the deconnexion message\n");
        return MSGerr;
    }

    return 1; //La deconnexion est confirmée puisqu'aucun problème n'est survenu
}