#ifndef _MONITEUR_H_
#define _MONITEUR_H_
#include "CL_include"

//Permet à un client de demander la clé d'accès du serveur pour s'y connecter
int Connexion(int* ptr, int* shmid, int* msqid);

//Permet à un client de demander au serveur de le déconnecter de la messagerie
int Deconnexion(int ptr, int msqid);

#endif