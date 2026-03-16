#include "scanner.h"
#include "metadata.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

static const char *AUDIO_EXTS[] = {
    ".mp3", ".flac", ".ogg", ".opus",
    ".m4a", ".aac",  ".wav", ".aiff",
    ".wv",  ".ape",  ".mpc", NULL
};

static int has_audio_ext(const char *name)
{
    const char *ext = strrchr(name, '.');
    if (!ext) return 0;
    for (int i = 0; AUDIO_EXTS[i]; i++) {
        if (strcasecmp(ext, AUDIO_EXTS[i]) == 0) return 1;
    }
    return 0;
}

static void scan_dir(AppState *s, const char *path, int depth)
{
    if (depth > 8) return;

    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.') continue;
        if (s->track_count >= MAX_TRACKS) break;

        char full[1024];
        snprintf(full, sizeof(full), "%s/%s", path, ent->d_name);

        struct stat st;
        if (stat(full, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            scan_dir(s, full, depth + 1);
        } else if (S_ISREG(st.st_mode) && has_audio_ext(ent->d_name)) {
            Track *t = &s->tracks[s->track_count];
            snprintf(t->path, sizeof(t->path), "%s", full);
            metadata_read(t);
            s->track_count++;
        }
    }
    closedir(dir);
}

int scan_library(AppState *s, const char *path)
{
    s->track_count = 0;
    scan_dir(s, path, 0);
    return s->track_count;
}
