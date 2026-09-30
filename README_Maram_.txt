AJOUTS AU JEU
============

Vaisseau et limites de l'ecran
------------------------------
- La vitesse du vaisseau est ajustable avec SPACESHIP_BOOST et SPACESHIP_FRICTION
	dans main.c.
- Le vaisseau ne traverse plus les bords : les toucher fait perdre une vie.

Niveaux et fin de partie
------------------------
- Le niveau actuel reste affiche en haut a droite et change a chaque nouveau
	niveau.
- Quand il n'y a plus de vies, un ecran GAME OVER propose R pour recommencer
	ou Q/Echap pour quitter. Une nouvelle partie reinitialise le score, les vies
	et le niveau, mais conserve le record.

Scores et records
-----------------
- Le jeu demande un pseudo et conserve les scores dans scores.txt entre les
	parties.
- Le meilleur score reste affiche a l'ecran. A la fin d'une partie, le
	terminal annonce un nouveau record si le meilleur score est battu et
	affiche les cinq meilleurs scores.

Pause
-----
- Appuyer sur P met le jeu en pause ou le reprend; la musique est egalement
	suspendue pendant la pause.

Musique
-------
- Une musique de fond est lue en boucle par ffplay.
- Par defaut, le jeu cherche music/theme.mp3. Un autre fichier peut etre
	choisi avec la variable d'environnement COMET_BUSTER_MUSIC.
- Le fichier audio n'est pas fourni avec le depot; il faut avoir ffplay
	installe et fournir un fichier MP3 local.
