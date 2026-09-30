#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "sprite.h"
#include "collider.h"
#include "linkedlist.h"
#include "scores.h"

#define SPACESHIP_BOOST     0.02
#define SPACESHIP_FRICTION  0.0009
#define BULLET_LIFETIME     25
#define BULLET_SPEED        10
#define DEFAULT_PTSIZE      24
#define NUMBER_OF_LIFES     5
//chemin de la musique de fond ajoutée
#define MUSIC_FILE "music/theme.mp3"
#include "level.h"

#define GDB()  __asm__("int $0x3")
//#define GDB() __builtin_trap();

bool gameover;
bool paused;
SDL_Surface *level_surface = NULL;
SDL_Surface *best_score_surface = NULL;
SDL_Surface *screen;
sprite_t sprite_ship;
list_ptr l_sprite_bullet;
list_ptr l_sprite_comet;
list_ptr l_sprite_explosion;
list_ptr l_sprite_text;
list_ptr l_sprite_life_counter;
list_ptr l_score_el = NULL;

bool shoot_again;
int score;
int level = LEVEL_MIN;
int best_score;
bool score_saved;

/* Declaration of few prototypes because there is no .h file with them */
int init_sdl(void);
static pid_t start_background_music(void);
static void stop_background_music(pid_t music_pid);
void afficher_scores(void);
void sauvegarder_score(const char *pseudo, int score);
void general_events(char *keys, pid_t music_pid);
void reset_game(TTF_Font *font_score, TTF_Font *font_level);
void game_events(char *keys);
void draw_explosion(int i,int j);
bool show_game_over(TTF_Font *title_font, TTF_Font *prompt_font);
void draw_fire(void);
void draw_life_counter(void);
void draw_score(TTF_Font *font);
void update_level_display(TTF_Font *font);
void update_best_score_display(TTF_Font *font);
void next_level(TTF_Font *font);
void draw_sprites(list_ptr *l_sprite);
void split(sprite_t old_comet, list_ptr **l_sprite_comet, enum sprite_type new_type);
void split_and_score(list_ptr element, list_ptr *l_sprite_comet, bool update_score);

/* SDL Initialisation. Create windows and so on
 *  return 0 if everything is ok, otherwise 1.
 * */
int init_sdl(void) {
  /* initialize SDL */
  SDL_Init(SDL_INIT_VIDEO);
  /* set the title bar */
  SDL_WM_SetCaption("Comet buster", "w00t!");
  /* create window */
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
  if (!screen)
    return 1;
  /* Initialize the TTF library a nd open the font */
  if ( TTF_Init() < 0 ) {
    return 1;
  }

  /* set keyboard repeat */
  SDL_EnableKeyRepeat(50, 50);
  return 0;
}

/* Use ffplay to play the supplied MP3 repeatedly while the game runs. */
static pid_t start_background_music(void) {
  pid_t music_pid = fork();

  if (music_pid == 0) {
    const char *music_file = getenv("COMET_BUSTER_MUSIC");
    if (!music_file || !music_file[0])
      music_file = MUSIC_FILE;
    execlp("ffplay", "ffplay", "-nodisp", "-loglevel", "error", "-loop", "0", music_file, (char *)NULL);
    perror("Impossible de lancer ffplay");
    _exit(127);
  }
  if (music_pid < 0)
    perror("Impossible de démarrer la musique");

  return music_pid;
}

static void stop_background_music(pid_t music_pid) {
  if (music_pid > 0) {
    kill(music_pid, SIGCONT);
    kill(music_pid, SIGTERM);
    waitpid(music_pid, NULL, 0);
  }
}


/* general SDL events hanlder
 * */
void general_events(char* keys, pid_t music_pid) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        gameover = true;
        break;
      case SDL_KEYUP:
        keys[event.key.keysym.sym] = 0;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
          case SDLK_q:
            gameover = true;
            break;
          case SDLK_p:
            if (!keys[SDLK_p]) {
              paused = !paused;
              if (music_pid > 0)
                kill(music_pid, paused ? SIGSTOP : SIGCONT);
            }
            break;
          case SDLK_d:
            GDB();
            break;
        }
        keys[event.key.keysym.sym] = 1;
        break;
    }
  }
}


/* In game event handler
 * */
void game_events(char* key) {
  SDLKey tabkey[] = {SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT, SDLK_SPACE};
  int i;
  if (key[tabkey[0]]) { //UP
    sprite_boost(sprite_ship, SPACESHIP_BOOST);
  }

  if (key[tabkey[1]]) { //DOWN
    sprite_boost(sprite_ship, -SPACESHIP_BOOST);
  }
  if (key[tabkey[2]]) { // LEFT
    sprite_turn_left(sprite_ship);
  }
  if (key[tabkey[3]]) { // RIGHT
    sprite_turn_right(sprite_ship);
  }
  if (key[tabkey[4]]) { // SPACE
    if (shoot_again) {
      draw_fire();
      shoot_again = false;
    }
  } else {
    shoot_again = true;
  }
}

/* Add a new explosion to the list
 * */
void draw_explosion(int i,int j) {
  sprite_t sprite;
  int colorkey = SDL_MapRGB(screen->format, 255, 0, 255);
  sprite = sprite_new(EXPLOSION, "sprites/explosion17.bmp", colorkey, 64, 25, 0, i-32, j-32, 0., 0., 0.);
  sprite->lifetime = 25;
  l_sprite_explosion = list_add(sprite, l_sprite_explosion);
}
bool show_game_over(TTF_Font *title_font, TTF_Font *prompt_font) {
  SDL_Color title_color = {255, 0, 0, 0};
  SDL_Color prompt_color = {255, 255, 255, 0};
  SDL_Surface *title = TTF_RenderText_Solid(title_font, "GAME OVER", title_color);
  SDL_Surface *prompt = TTF_RenderText_Solid(prompt_font,
      "R: recommencer     Q: quitter", prompt_color);
  bool restart = false;
  bool waiting = true;
  SDL_Event event;

  if (title && prompt) {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_Rect title_position = {(SCREEN_WIDTH - title->w) / 2, SCREEN_HEIGHT / 3, 0, 0};
    SDL_Rect prompt_position = {(SCREEN_WIDTH - prompt->w) / 2,
                                title_position.y + title->h + 20, 0, 0};
    SDL_BlitSurface(title, NULL, screen, &title_position);
    SDL_BlitSurface(prompt, NULL, screen, &prompt_position);
    SDL_Flip(screen);
  }

  while (waiting) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        waiting = false;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_r) {
          restart = true;
          waiting = false;
        } else if (event.key.keysym.sym == SDLK_q ||
                   event.key.keysym.sym == SDLK_ESCAPE) {
          waiting = false;
        }
      }
    }
    if (waiting)
      SDL_Delay(20);
  }

  if (title)
    SDL_FreeSurface(title);
  if (prompt)
    SDL_FreeSurface(prompt);
  return restart;
}


/* Launch a bullet ;)
 *  Add the bullet to the list
 * */
void draw_fire(void) {
  int colorkey;
  float dir_angle;
  sprite_t sprite;
  colorkey = SDL_MapRGB(screen->format, 255, 125, 0);
  dir_angle = 2.*PI*sprite_ship->anim_sprite_num/sprite_ship->anim_sprite_num_max;
  sprite = sprite_new(BULLET, "sprites/bullet02.bmp", colorkey, 4, 1, 0, sprite_ship->rc_screen_xy.x+sprite_ship->size/2*(1+cos(dir_angle)), sprite_ship->rc_screen_xy.y+sprite_ship->size/2*(1-sin(dir_angle)), BULLET_SPEED*cos(dir_angle), BULLET_SPEED*(-sin(dir_angle)), 0.);
  sprite->lifetime = BULLET_LIFETIME;
  l_sprite_bullet = list_add(sprite, l_sprite_bullet);
}

/* Draw the score sprite
 *  Add it to the list
 * */
void draw_score(TTF_Font * font) {

  SDL_Surface *score_surf;
  SDL_Color score_color = {255, 255, 255, 0};
  char score_text[1024];
  sprite_t sprite;

  sprintf(score_text, "%08d", score);
  if (score > best_score)
    best_score = score;
  update_best_score_display(font);
  score_surf = TTF_RenderText_Solid(font, score_text, score_color);
  sprite = sprite_new_text(score_surf, 5, 5);
  if (l_score_el)
    list_remove(l_score_el, &l_sprite_text);
  l_score_el = l_sprite_text = list_add(sprite, l_sprite_text);
}

/* Draw the all-time high score in the HUD. */
void update_best_score_display(TTF_Font *font) {
  SDL_Color color = {255, 255, 255, 0};
  char text[48];

  if (best_score_surface)
    SDL_FreeSurface(best_score_surface);

  if (!font) {
    best_score_surface = NULL;
    return;
  }

  sprintf(text, "BEST %08d", best_score);
  best_score_surface = TTF_RenderText_Solid(font, text, color);
}

/* Refresh the persistent HUD text showing the current level. */
void update_level_display(TTF_Font *font) {
  SDL_Color color = {255, 255, 0, 0};
  char text[32];

  if (level_surface)
    SDL_FreeSurface(level_surface);

  if (!font) {
    level_surface = NULL;
    return;
  }

  sprintf(text, "LEVEL %d", level);
  level_surface = TTF_RenderText_Solid(font, text, color);
}

/* Draw the life counter sprites
 *  Add sprites to the list
 * */
void draw_life_counter(void) {
  int x = 5;
  int i;
  sprite_t sprite;
  int colorkey = SDL_MapRGB(screen->format, 255, 0, 255);

  /* create and initialize the life counter sprite */
  l_sprite_life_counter = list_new();
  for (i = 0; i < NUMBER_OF_LIFES; i++) {
    sprite = sprite_new(LIFE_COUNTER, "sprites/grayship_16x16.bmp", colorkey, 16, 1, 0, x, 35, 0., 0., 0.);
    l_sprite_life_counter = list_add(sprite, l_sprite_life_counter);
    x += 15;
  }
}

/* Clear the previous round and build a fresh one without resetting the record. */
void reset_game(TTF_Font *font_score, TTF_Font *font_level) {
  int colorkey = SDL_MapRGB(screen->format, 255, 0, 255);

  if (sprite_ship)
    sprite_free(sprite_ship);
  list_free(l_sprite_bullet);
  list_free(l_sprite_comet);
  list_free(l_sprite_explosion);
  list_free(l_sprite_text);
  list_free(l_sprite_life_counter);

  sprite_ship = NULL;
  l_sprite_bullet = list_new();
  l_sprite_comet = list_new();
  l_sprite_explosion = list_new();
  l_sprite_text = list_new();
  l_sprite_life_counter = list_new();
  l_score_el = NULL;

  score = 0;
  level = LEVEL_MIN;
  score_saved = false;
  gameover = false;
  paused = false;
  shoot_again = true;

  update_level_display(font_level);
  draw_score(font_score);
  draw_life_counter();
  sprite_ship = sprite_new(SHIP, "sprites/greenship-v1.bmp", colorkey, 32, 36,
      9, SPACESHIP_INIT_X, SPACESHIP_INIT_Y, 0., 0., SPACESHIP_FRICTION);
  gen_level(level, &l_sprite_comet, screen);
}

/* Draw the new level ban,
 *  and level up
 * */
void next_level( TTF_Font * font) {
  SDL_Color text_color = {255, 255, 0, 0};//R,G,B,A
  char text[1024];
  sprite_t sprite_text;

  printf("DEBUG: Congratulation! You won the level %d (score=%d)\n",level,score);
  fflush(stdout);
  level++;
  update_level_display(font);
  printf("DEBUG: Start level %d, good luck!\n",level);
  fflush(stdout);
  // Add the "next level" text sprite to the text sprite list
  sprintf(text, "LEVEL %d", level);
  SDL_Surface *text_surf = TTF_RenderText_Solid(font, text, text_color);
  sprite_text = sprite_new_text(text_surf, SCREEN_WIDTH/2-60, SCREEN_HEIGHT/3);
  sprite_text->lifetime = 150;
  l_sprite_text = list_add(sprite_text, l_sprite_text);
}


/* Blit a sprites list.
 *  Depending on the sprite type, we had physics & effects
 * */
void draw_sprites(list_ptr *l_sprite) {
  sprite_t sprite;
  list_ptr l_ptr = *(l_sprite);

  // nothing to draw
  if (list_is_empty(l_ptr))
    return;

  // loop throught the list and draw
  while (!list_is_empty(l_ptr)) {
    sprite = list_head_sprite(l_ptr);
    list_ptr list_el = l_ptr;

    l_ptr = list_next(l_ptr);

    // let's take some age if needed
    if (sprite_can_die(sprite)) {
      sprite_get_older(sprite);

      if (sprite_is_dead(sprite)) {
        //printf("DEBUG sprite_is_dead %d\n",sprite->type);
        list_remove(list_el, l_sprite);
        continue;
      }
    }

    if (sprite) {
      if (sprite_is_ennemy(sprite)||sprite->type==BULLET) {
        sprite_play_physics(sprite);
      }

      // comet are dangerous, they rotate on themself to maximize damages
      if (sprite_is_comet(sprite)) {
        sprite_turn_right(sprite);
      }

      // explosions effect is simple, simply add a rotation :)
      if (sprite->type == EXPLOSION) {
        sprite_turn_left(sprite);
      }

      // TXT sprite don't provide animation files, so provide a special blit
      if (sprite->type == TXT)
        SDL_BlitSurface(sprite->sprite, NULL, screen, &sprite->rc_screen_xy);
      else
        SDL_BlitSurface(sprite->sprite, &sprite->rc_anim_xy, screen, &sprite->rc_screen_xy);
    }
  }
}


/* Handle consequences of a collision:
 *   increase score depending on the sprite->type
 *   split object if needed
 * */
void split_and_score(list_ptr element, list_ptr *l_sprite_comet, bool update_score) {
  //score + split
  int diff = 0;
  sprite_t data = list_head_sprite(element);
  switch (data->type) {
    case L_COMET:
      diff = 20;
      split(data, &l_sprite_comet, M_COMET);
      break;
    case M_COMET:
      diff = 50;
      split(data, &l_sprite_comet, S_COMET);
      break;
    case S_COMET:
      diff = 100;
      break;
    case ET:
      diff = 60;
      break;
  }
  score += (update_score)?diff:0;
}

/* Split an asteroid into two new objects which get most of parameters from
 * their parent (old_comet). They are also added to the list of comets.
 * */
void split(sprite_t old_comet, list_ptr **l_sprite_comet, enum sprite_type new_type) {
  float speed = get_base_speed(level, new_type);
  float angle = (float)(rand()%360)/360*2*PI;
  int colorkey = old_comet->colorkey;
  int x = old_comet->x;
  int y = old_comet->y;
  int new_sprite_size = old_comet->size/2; //64 32 26
  const char * fname = get_comet_sprite(level, new_type);
  sprite_t first = sprite_new(new_type, fname, colorkey, new_sprite_size, 32, 0, x, y, speed*cos(angle), speed*sin(angle), 0.);
  //change angle for the second asteroid
  angle = (float)(rand()%360)/360*2*PI;
  sprite_t second = sprite_new(new_type, fname, colorkey, new_sprite_size, 32, 0, x, y, speed*cos(angle), speed*sin(angle), 0.);
  **l_sprite_comet = list_add(first, **l_sprite_comet);
  **l_sprite_comet = list_add(second, **l_sprite_comet);
}


void sauvegarder_score(const char *pseudo, int score){
  entry_t table[MAX_SCORES];
  int count = scores_load("scores.txt", table);
  scores_sort(table, count);
  int previous_best = count > 0 ? table[0].score : 0;

  if (score <= 0)
    return;

  if (count < MAX_SCORES) {
    count = scores_add(table, count, pseudo, score);
  } else if (score > table[count - 1].score) {
    strncpy(table[count - 1].name, pseudo, NAME_LEN - 1);
    table[count - 1].name[NAME_LEN - 1] = '\0';
    table[count - 1].score = score;
  }

  scores_sort(table, count);
  scores_save("scores.txt", table, count);
  best_score = count > 0 ? table[0].score : 0;

  if (score > previous_best)
    printf("Nouveau record ! %d points\n", score);
  scores_print_top(table, count, 5);
}

void afficher_scores(void){
  entry_t table[MAX_SCORES];
  int count = scores_load("scores.txt", table);
  scores_sort(table, count);
  scores_print_top(table, count, 5);
}


int main(int argc, char* argv[]) {
  char pseudo[NAME_LEN];
  SDL_Surface *temp, *bg, *pause_surface;
  SDL_Rect rcBg;
  int colorkey;
  bool collide;
  bool running = true;
  bool lost_game;
  int cu, cv;
  int timer = 0;
  sprite_t sprite_life_counter;
  sprite_t current_sprite;
  int ret;
  pid_t music_pid;
  TTF_Font * font_score;
  TTF_Font * font_next_level;
  TTF_Font *font_game_over;

  printf("Entrez votre pseudo (max %d caractères): ", NAME_LEN - 1);
  fflush(stdout);
  if (!fgets(pseudo, sizeof(pseudo), stdin))
    strcpy(pseudo, "Joueur");
  pseudo[strcspn(pseudo, "\r\n")] = '\0';
  if (pseudo[0] == '\0')
    strcpy(pseudo, "Joueur");

  ret = init_sdl();
  if (ret) {
    perror("Error init_sdl: ");
    SDL_Quit();
    return(ret);
  }

  music_pid = start_background_music();

  // default colorkey
  colorkey = SDL_MapRGB(screen->format, 255, 0, 255);

  // fonts
  font_score = TTF_OpenFont("fonts/LinLibertine_DR.ttf", 24);
  font_next_level = TTF_OpenFont("fonts/LinLibertine_DR.ttf", 36);;
  font_game_over = TTF_OpenFont("fonts/LinLibertine_DR.ttf", 72);
  pause_surface = TTF_RenderText_Solid(font_score,
      "PAUSE - P pour reprendre", (SDL_Color){255, 255, 0, 0});
  update_level_display(font_next_level);

  {
    entry_t table[MAX_SCORES];
    int count = scores_load("scores.txt", table);
    scores_sort(table, count);
    best_score = count > 0 ? table[0].score : 0;
  }
  update_best_score_display(font_score);

  // create the text sprites list
  l_sprite_text = list_new();

  // initialize score and score sprite
  score = 0;
  draw_score(font_score);
  draw_life_counter();

  //create and initialize the spaceship sprite
  sprite_ship = sprite_new(SHIP, "sprites/greenship-v1.bmp", colorkey, 32, 36, 9, SPACESHIP_INIT_X, SPACESHIP_INIT_Y, 0., 0., SPACESHIP_FRICTION);

  /* create and initialize a comet list (only for tests, comets will be generated in level.c) */
  l_sprite_comet = list_new();
  gen_level(level, &l_sprite_comet, screen);

  //create the bullets list
  l_sprite_bullet = list_new();

  //load background sprite
  temp = SDL_LoadBMP("sprites/backgroundlvl1.bmp");
  bg = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  rcBg.x = 0;
  rcBg.y = 0;

  gameover = false;
  shoot_again = true;

  char key[SDLK_LAST] = {0};
  unsigned int lasttime = 0;
  while (running) {
    gameover = false;
    lost_game = false;

    /* message pump for the current round */
    while (!gameover) {
    lasttime = SDL_GetTicks();
    SDL_Event event;
    list_ptr l_ptr;
    int counter;

    general_events(key, music_pid);
    if (gameover)
      break;

    if (paused) {
      if (pause_surface) {
        SDL_Rect pause_position = {(SCREEN_WIDTH - pause_surface->w) / 2,
                                   (SCREEN_HEIGHT - pause_surface->h) / 2, 0, 0};
        SDL_BlitSurface(pause_surface, NULL, screen, &pause_position);
      }
      SDL_Flip(screen);
      SDL_Delay(50);
      continue;
    }

    game_events(key);

    /* draw the background */
    SDL_BlitSurface(bg, NULL, screen, &rcBg);

    /* Keep the current level visible throughout the game. */
    if (level_surface) {
      SDL_Rect level_position = {SCREEN_WIDTH - level_surface->w - 10, 10, 0, 0};
      SDL_BlitSurface(level_surface, NULL, screen, &level_position);
    }
    if (best_score_surface) {
      SDL_Rect best_score_position = {5, 65, 0, 0};
      SDL_BlitSurface(best_score_surface, NULL, screen, &best_score_position);
    }

    // draw the text sprites
    draw_sprites(&l_sprite_text);

    // draw the life counter sprites
    draw_sprites(&l_sprite_life_counter);

    /* play & draw the spaceship sprite */
    sprite_play_physics(sprite_ship);
    SDL_BlitSurface(sprite_ship->sprite, &sprite_ship->rc_anim_xy, screen, &sprite_ship->rc_screen_xy);
    
    if (sprite_ship->x <= 0 || //x pour le bord gauche
    sprite_ship->x + sprite_ship->size >= SCREEN_WIDTH ||  //x+ longueur de l'écran= bord du droit
    sprite_ship->y <= 0 ||  //y pour le bord haut
    sprite_ship->y + sprite_ship->size >= SCREEN_HEIGHT) {  //y+ longueur de l'écran= bord du bas

  draw_explosion(sprite_ship->x + sprite_ship->size / 2,
                 sprite_ship->y + sprite_ship->size / 2);

  sprite_ship->x = sprite_ship->rc_screen_xy.x = 304;
  sprite_ship->y = sprite_ship->rc_screen_xy.y = 224;
  sprite_ship->vx = 0.;
  sprite_ship->vy = 0.;

  if (!list_is_empty(l_sprite_life_counter)) {
    sprite_t dead_sprite = list_pop_sprite(&l_sprite_life_counter);
    if (dead_sprite)
      sprite_free(dead_sprite);
  } else {
    printf(" ============ Game Over ============= \n");
    printf("Score: you reached level %d with %d points\n", level, score);
    fflush(stdout);
    if (!score_saved) {
      sauvegarder_score(pseudo, score);
      score_saved = true;
    }
    lost_game = true;
    gameover = true;
  }
}
    // draw comets & nyancats
    draw_sprites(&l_sprite_comet);
    // draw bullets
    draw_sprites(&l_sprite_bullet);

    /* collide tests ship <-> comets */
    l_ptr = l_sprite_comet;
    while (!list_is_empty(l_ptr)) {
      current_sprite = list_head_sprite(l_ptr);
      list_ptr list_el = l_ptr;
      l_ptr = list_next(l_ptr);

      collide = collide_test(sprite_ship, current_sprite, screen->format, &cu, &cv);
      if (collide) {
        // draw the explosion
        draw_explosion(cu,cv);
        //no additional score if the ship is destroyed?
	split_and_score(list_el, &l_sprite_comet, false);
	list_remove(list_el, &l_sprite_comet);
	sprite_ship->x = sprite_ship->rc_screen_xy.x = 304;
	sprite_ship->y = sprite_ship->rc_screen_xy.y = 224;
	sprite_ship->vx = 0.;
	sprite_ship->vy = 0.;
	if (!list_is_empty(l_sprite_life_counter)) {
	  sprite_t dead_sprite = list_pop_sprite(&l_sprite_life_counter);
          if (dead_sprite)
            sprite_free(dead_sprite);
	} else {
          printf(" ============ Game Over ============= \n");
          printf("Score: you reached level %d with %d points\n",level,score);
          fflush(stdout);
          if (!score_saved) {
            sauvegarder_score(pseudo, score);
            score_saved = true;
          }
          lost_game = true;
          gameover = true;
        }
      }
    }

    /* collide tests bullets <-> comets */
    l_ptr = l_sprite_bullet;
    while (!list_is_empty(l_ptr)) {
      current_sprite = list_head_sprite(l_ptr);
      list_ptr list_el = l_ptr;
      list_ptr l_ptr_c = l_sprite_comet;

      l_ptr = list_next(l_ptr);
      while (!list_is_empty(l_ptr_c)) {
        sprite_t sprite_comet = list_head_sprite(l_ptr_c);
        list_ptr list_el_c = l_ptr_c;

        l_ptr_c = list_next(l_ptr_c);
        collide = collide_test(current_sprite, sprite_comet, screen->format, &cu, &cv);
        if (collide) {
          // draw the explosion
          draw_explosion(cu,cv);
          split_and_score(list_el_c, &l_sprite_comet, true);//addtional points
          list_remove(list_el_c, &l_sprite_comet);
          list_remove(list_el, &l_sprite_bullet);
          // update the score display
          draw_score(font_score);
          break;
        }
      }
    }

    draw_sprites(&l_sprite_explosion);

    //when enemies are all destroyed, the level is passed
    if (list_is_empty(l_sprite_comet)) {
      // draw the new level
      next_level(font_next_level);
      //clean comets & nyancats list from old level
      list_free(l_sprite_comet);
      l_sprite_comet = list_new();
      gen_level(level, &l_sprite_comet, screen);
    }

    /* update the screen */
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    timer++;
    //FIXME?
    while(SDL_Flip(screen)!=0) { /* if GC not ready to blit */
      SDL_Delay(1); /* wait and keep vertical sync*/
    }
    while(SDL_GetTicks()-lasttime<20) { /* minimal frame time: 20ms */
      SDL_Delay(1);
    }

    }//loop !gameover

    /* Save the round once, whether it ended in a loss or a normal quit. */
    if (!score_saved && score > 0) {
    sauvegarder_score(pseudo, score);
    score_saved = true;
    update_best_score_display(font_score);
    }

    if (lost_game && show_game_over(font_game_over, font_score)) {
      reset_game(font_score, font_next_level);
      memset(key, 0, sizeof(key));
      continue;
    }
    running = false;
  }

  printf("Bye bye.\n");
  /* free the space_ship sprite */
  sprite_free(sprite_ship);
  /* free the background surface */
  SDL_FreeSurface(bg);
  // Shutdown the TTF library
  TTF_CloseFont(font_next_level);
  TTF_CloseFont(font_score);
  TTF_CloseFont(font_game_over);
  if (pause_surface)
    SDL_FreeSurface(pause_surface);
  if (level_surface)
    SDL_FreeSurface(level_surface);
  if (best_score_surface)
    SDL_FreeSurface(best_score_surface);
  TTF_Quit();
  stop_background_music(music_pid);
  /* cleanup SDL */
  SDL_Quit();
  return 0;
}//main

