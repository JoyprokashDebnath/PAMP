#ifndef METADATA_H
#define METADATA_H

#include "../state/state.h"

/* Read tags for track t — fills title, artist, album, duration.
 Fal*ls back to filename if tags are missing. */
void metadata_read(Track *t);

#endif
