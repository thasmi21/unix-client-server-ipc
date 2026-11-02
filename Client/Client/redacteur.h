#ifndef _REDACTEUR_H_
#define _REDACTEUR_H_
#include "CL_include"

//Permet de lire les données en sortie du pipe commun avec le lecteur et de les envoyer au driver par paquets de 5
void Redaction(int pipefd_LR, int pipefd_RD, int semid2, int voie);

#endif 