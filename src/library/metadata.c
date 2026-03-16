#include "metadata.h"
#include <string.h>
#include <libgen.h>  /* basename() */

#ifdef HAVE_TAGLIB
#include <tag_c.h>

void metadata_read(Track *t)
{
    TagLib_File *f = taglib_file_new(t->path);

    if (!f || !taglib_file_is_valid(f)) {
        /* fallback — use filename without extension as title */
        char tmp[1024];
        strncpy(tmp, t->path, sizeof(tmp) - 1);
        char *base = basename(tmp);
        char *dot  = strrchr(base, '.');
        if (dot) *dot = '\0';
        strncpy(t->title,  base,      sizeof(t->title)  - 1);
        strncpy(t->artist, "Unknown", sizeof(t->artist) - 1);
        strncpy(t->album,  "Unknown", sizeof(t->album)  - 1);
        if (f) taglib_file_free(f);
        return;
    }

    TagLib_Tag *tag = taglib_file_tag(f);

    const char *title  = taglib_tag_title(tag);
    const char *artist = taglib_tag_artist(tag);
    const char *album  = taglib_tag_album(tag);

    /* fallback each field individually */
    if (title && *title)
        strncpy(t->title, title, sizeof(t->title) - 1);
    else {
        char tmp[1024];
        strncpy(tmp, t->path, sizeof(tmp) - 1);
        char *base = basename(tmp);
        char *dot  = strrchr(base, '.');
        if (dot) *dot = '\0';
        strncpy(t->title, base, sizeof(t->title) - 1);
    }

    strncpy(t->artist,
            (artist && *artist) ? artist : "Unknown",
            sizeof(t->artist) - 1);

    strncpy(t->album,
            (album  && *album)  ? album  : "Unknown",
            sizeof(t->album)  - 1);

    t->track_num = (int)taglib_tag_track(tag);

    const TagLib_AudioProperties *ap =
    taglib_file_audioproperties(f);
    if (ap)
        t->duration = taglib_audioproperties_length(ap);

    taglib_file_free(f);
    taglib_tag_free_strings();
}

#else   /* no TagLib — filename-only fallback */

void metadata_read(Track *t)
{
    char tmp[1024];
    strncpy(tmp, t->path, sizeof(tmp) - 1);
    char *base = basename(tmp);
    char *dot  = strrchr(base, '.');
    if (dot) *dot = '\0';
    strncpy(t->title,  base,      sizeof(t->title)  - 1);
    strncpy(t->artist, "",        sizeof(t->artist) - 1);
    strncpy(t->album,  "",        sizeof(t->album)  - 1);
    t->duration  = 0;
    t->track_num = 0;
}

#endif
