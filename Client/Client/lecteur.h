#ifndef _LECTEUR_H_
#define _LECTEUR_H_
#include "CL_include"

//Récupère une donnée envoyée par le serveur sur un des deux tampons de la mémoire partagée et l'envoie au rédacteur
//via un pipe privé
int Lecture(BUF *tampon, int voie, int semid, int pipefd);

#endif
