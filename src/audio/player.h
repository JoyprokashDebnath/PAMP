#ifndef PLAYER_H
#define PLAYER_H

/* Opaque handle — callers never touch mpv_handle directly */
typedef struct Player Player;

/* Lifecycle */
Player *player_create(void);
void    player_destroy(Player *p);

/* Playback control */
void    player_load(Player *p, const char *path);
void    player_toggle_pause(Player *p);
void    player_seek(Player *p, double seconds_relative);
void    player_seek_abs(Player *p, double position);
void    player_set_volume(Player *p, double vol); /* 0 – 100 */

/* State queries */
double  player_get_position(Player *p);
double  player_get_duration(Player *p);
double  player_get_volume(Player *p);
int     player_is_paused(Player *p);
int     player_is_idle(Player *p);

/* Call once per frame from the main loop (non-blocking) */
/* Returns 1 if the current track ended, 0 otherwise      */
int     player_poll(Player *p);

#endif /* PLAYER_H */
