#ifndef STATE_H
#define STATE_H

#define MAX_TRACKS 4096

typedef enum {
    PLAYER_IDLE,
    PLAYER_LOADING,
    PLAYER_PLAYING,
    PLAYER_PAUSED,
    PLAYER_ERROR
} PlayerState;

typedef enum {
    REPEAT_OFF,
    REPEAT_ALL,
    REPEAT_ONE
} RepeatMode;

typedef struct {
    char path[1024];
    char title[256];
    char artist[256];
    char album[256];
    int  duration;
    int  track_num;
} Track;

typedef struct {
    /* lifecycle */
    int         is_running;

    /* terminal size */
    int         rows;
    int         cols;

    /* playback */
    PlayerState playback_state;
    double      position;
    double      duration;
    double      volume;
    int         muted;

    /* current track info */
    char        current_path[1024];
    char        current_title[256];
    char        current_artist[256];
    char        current_album[256];

    /* library */
    Track       tracks[MAX_TRACKS];
    int         track_count;

    /* library pane */
    int         lib_selected;
    int         lib_scroll;

    /* playlist */
    int         current_index;
    int         shuffle;
    RepeatMode  repeat;

    /* status message */
    char        status_msg[256];
} AppState;

void state_init(AppState *s);

#endif
