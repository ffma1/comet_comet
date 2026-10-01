==========================================
Bilan Comet Busters
Autrice : Maram MAAROUFI et Victoria RUF
==========================================

Lors de ce projet j'ai appris à gérer les aspects dynamiques et logiques d'un jeu, à coordonner les mécaniques de gameplay et à mettre en place une expérience utilisateur cohérente.
Ce projet a été particulièrement enrichissant car il m'a demandé de réfléchir à la structure globale du jeu et comment les différentes fonctionnalités s'articulent entre elles.
Il m'a également appris à me coordonner et à travailler avec une camarade, ce qui n'est pas toujours évident lorsqu'on code.

Nous avons donc réparti le travail :

Maram :
- Gestion de la vitesse du vaisseau et de la délimitation des bordures du jeu.
- Mise en place d'un "GameOver" pour stopper la partie une fois que nous n'avons plus de vie.
- Mise en place d'une musique de fond pour le jeu.

Victoria :
- Ajout des fonctions dans le linkedlist.c
- Ajout des 2 fonctions dans le main.c (afficher_scores, sauvegarder_score) pour répondre à la demande du fichier texte.
- Corrections de quelques erreurs comme mettre des float à la place des int pour les angles.


## Gestion de la vitesse du vaisseau et des bordures du jeu

J'ai dû comprendre comment modifier les coordonnées du vaisseau pour implémenter la vitesse. 
Le vaisseau devait se déplacer progressivement et de manière fluide, pas seulement sauter d'une position à l'autre.
J'ai travaillé sur les variables de positionnement et les boucles de jeu pour que la vitesse soit cohérente avec la logique générale.

Pour les bordures, j'ai mis en place une vérification à chaque frame pour que le vaisseau ne puisse pas sortir de l'écran.
C'était important pour la jouabilité : le vaisseau doit rester visible et contrôlable à tout moment.


## Mise en place du GameOver

La gestion du GameOver a nécessité de comprendre le système de vies et comment arrêter la boucle principale du jeu.
J'ai dû :
- Suivre le nombre de vies restantes
- Vérifier à chaque frame si les vies sont épuisées
- Arrêter les animations et les inputs du joueur
- Afficher un écran de fin de partie

C'était un défi logique car il fallait s'assurer que tous les éléments du jeu s'arrêtaient proprement sans crash.


## Mise en place de la musique de fond

Pour intégrer la musique, j'ai dû :
- Charger un fichier audio approprié
- L'initialiser au démarrage du jeu
- S'assurer qu'elle boucle correctement sans interruption
- Gérer les ressources audio pour éviter les fuites mémoire

La musique améliore considérablement l'atmosphère du jeu et crée une meilleure immersion pour le joueur.


## Apprentissages et défis

Ce projet m'a montré l'importance de la coordination dans un projet de groupe. 
Victoria et moi avons dû nous assurer que nos modifications ne conflictaient pas et que le jeu restait cohérent.

J'ai également appris à :
- Penser à la expérience utilisateur globale
- Gérer les états du jeu (en cours, pause, fin)
- Mettre en place des mécaniques fluides et réactives
- Travailler avec des structures et données complexes en C

Ce projet a renforcé ma compréhension de la programmation événementielle et de la boucle principale d'un jeu.
