===================
README Victoria RUF
===================

==========================================
Bilan Comet Busters
Autrices : Maram MAAROUFI et Victoria RUF
==========================================

Lors de ce projet j'ai appris à m'approprier un code qui n'est pas le mien, à corriger des erreurs et à implémenter des fonctions.
Ce projet a été particulièrement difficile car il m'a demandé une réelle réflexion pour comprendre le code.
Il m'a également appris à me coordonner et à travailler avec une camarade, ce qui n'est pas toujours évident lorsqu'on code.

Nous avons donc réparti le travail :

Victoria :
- Ajout des fonctions dans le linkedlist.c
- Ajout des 2 fonctions dans le main.c (afficher_scores, sauvegarder_score) pour répondre à la demande du fichier texte.
- Corrections de quelques erreurs comme mettre des float à la place des int pour les angles.

Maram :
- Gestion de la vitesse du vaisseau : déplacement du vaisseau en mettant à jour sa position à chaque frame selon une vitesse donnée. Cela permet d'obtenir un mouvement fluide et continu.
- Mise en place du GameOver : vérification des vies du joueur et un état de fin de partie lorsque celles-ci tombent à zéro.
- Affichage du niveau : ajout d'un système pour afficher le niveau actuel et le faire évoluer selon la progression du joueur.
- Musique de fond : musique en boucle pour améliorer l'ambiance du jeu.
- Ajout des options de pause avec P et restart avec R.


J'ai rapidement compris qu'il fallait implémenter des fonctions dans le fichier "linkedlist.c".
Pour les fonctions list_new, list_add, list_is_empty, list_next, list_head_sprite, list_free, list_length, list_clone je me suis aidée des exercices du TD et j'ai repris la même forme.
Toutes ces fonctions sont reliées à list_ptr, list_ptr est une liste qui pointe vers un noeud qui lui-même est composé de 2 parties : le next (pour passer au noeud d'après) et le data qui contient un sprite.

J'ai eu beaucoup de mal avec la fonction list_pop_sprite, j'avais "l'exosquette" de la fonction mais elle ne faisait pas ce que je voulais.
Je comprenais ce qu'il fallait faire mais je n'arrivais pas à le retranscrire en code. Là où je bloquais c'était pour définir le dernier noeud de la liste : j'y arrivais quand la liste ne contient qu'un seul élément mais pas quand elle en contenait plusieurs.
Je me suis alors aidée de forums et de l'IA car je n'y arrivais pas et que je ne voulais pas mettre en retard notre projet.
Ce que je n'ai pas réussi à retranscrire c'était : "avant_dernier = avant_dernier -> next;"

Une fois débloquée, j'ai compris comment retranscrire la fonction en code. Ce qui m'a permis de faire la fonction "list_remove" sans utiliser l'IA car elle suivait la même logique.

Pour la fonction "list_reverse" j'ai utilisé la fonction "list_add" utilisée précédemment.
L'objectif de cette fonction est de retourner l'inverse d'une liste, OR on sait que la fonction "list_add" ajoute en en-tête.
Donc il suffit de créer une nouvelle liste, de parcourir l'ancienne et pour chaque élément de l'ancienne, ajouter en-tête l'élément dans la nouvelle liste.

Ce schéma de pensée m'a demandé beaucoup de réflexion et d'analyse du code. 
Au début j'avais pensé à faire du REGEX mais même problème qu'avant : je manque de compétences en programmation, je n'arrivais pas à le retranscrire en code. 
J'ai alors bien pris le temps d'analyser tout mon programme et j'ai trouvé cette solution. 
Cela montre bien l'importance de comprendre et de s'approprier un code qui ne nous appartiens pas.


Puis j'ai mis en place 2 fonctions principales dans le main.c :
-> afficher_scores
-> sauvegarder_score

Ces fonctions ont pour objectif que le joueur saisisse un pseudo et qu'à la fin son pseudo soit associé à son score dans un fichier texte.

Je me suis servi de cette documentation pour réaliser ces 2 fonctions ainsi que les exercices du TD :
http://manpagesfr.free.fr/man/man3/fopen.3.html
