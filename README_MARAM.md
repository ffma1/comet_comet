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

Pour implémenter la vitesse du vaisseau, j'ai travaillé sur les variables de position et de déplacement. 
Le principe est simple : à chaque frame, on met à jour la position du vaisseau en ajoutant une valeur de vitesse :

```c
ship_x += velocity_x * delta_time;
ship_y += velocity_y * delta_time;
```

Cela permet au vaisseau de se déplacer progressivement et de manière fluide, plutôt que de sauter d'une position à l'autre.

Pour les bordures, j'ai mis en place des vérifications avant d'afficher le vaisseau. Le principe est de vérifier si la position du vaisseau dépasse les limites de l'écran et, si c'est le cas, de la ramener à la limite :

```c
if (ship_x < 0) ship_x = 0;
if (ship_x > SCREEN_WIDTH - SHIP_WIDTH) ship_x = SCREEN_WIDTH - SHIP_WIDTH;
if (ship_y < 0) ship_y = 0;
if (ship_y > SCREEN_HEIGHT - SHIP_HEIGHT) ship_y = SCREEN_HEIGHT - SHIP_HEIGHT;
```

C'était un élément crucial pour la jouabilité : le vaisseau doit rester visible et contrôlable à tout moment, sans pouvoir s'échapper de l'écran.


## Mise en place du GameOver

La gestion du GameOver a nécessité de suivre l'état du jeu et d'arrêter la boucle principale lorsque le joueur n'a plus de vies.

J'ai implémenté une variable `game_state` qui indique si le jeu est en cours, en pause ou terminé :

```c
enum GameState { RUNNING, PAUSED, GAME_OVER };
enum GameState state = RUNNING;
```

À chaque frame, avant de mettre à jour le jeu, je vérifie si le joueur a encore des vies :

```c
if (lives <= 0) {
    state = GAME_OVER;
}
```

Quand l'état passe à `GAME_OVER`, la boucle principale s'arrête, les inputs du joueur ne sont plus traités, et un écran de fin de partie s'affiche. C'était un défi logique car il fallait s'assurer que tous les éléments du jeu s'arrêtaient proprement sans crash et que la transition se faisait correctement.


## Mise en place de la musique de fond

Pour intégrer la musique au jeu, j'ai utilisé une bibliothèque audio pour charger et jouer les sons. Le principe général est le suivant :

1. **Initialisation** : Au démarrage du jeu, on charge le fichier audio :
```c
Mix_Music *background_music = Mix_LoadMUS("assets/music/background.wav");
Mix_PlayMusic(background_music, -1); // -1 pour boucler indéfiniment
```

2. **Gestion de la ressource** : On vérifie que le fichier a bien été chargé avant de le jouer :
```c
if (background_music == NULL) {
    printf("Erreur: impossible de charger la musique\n");
}
```

3. **Nettoyage** : À la fin du jeu, on libère la ressource pour éviter les fuites mémoire :
```c
Mix_FreeMusic(background_music);
```

La musique améliore considérablement l'atmosphère du jeu et crée une meilleure immersion pour le joueur. Elle boucle sans interruption pour une expérience fluide.


## Apprentissages et défis

Ce projet m'a montré l'importance de la coordination dans un projet de groupe. 
Victoria et moi avons dû nous assurer que nos modifications ne conflictaient pas et que le jeu restait cohérent.

J'ai également appris à :
- Penser à la expérience utilisateur globale et à comment chaque mécanique s'articule
- Gérer les états du jeu (en cours, pause, fin)
- Mettre en place des mécaniques fluides et réactives
- Travailler avec des structures et données complexes en C
- Utiliser les bibliothèques externes comme SDL2 pour la gestion audio

Ce projet a renforcé ma compréhension de la programmation événementielle et de la boucle principale d'un jeu. C'était enrichissant de voir comment tous les éléments d'un jeu s'assemblent pour créer une expérience cohérente.
