#ifndef COLORS_H
#define COLORS_H

/* Color pair IDs — use these names everywhere, never raw numbers */
#define PAIR_NORMAL    1   /* white   on default — body text          */
#define PAIR_SELECTED  2   /* black   on white   — highlighted row    */
#define PAIR_HEADER    3   /* white   on blue    — header bar         */
#define PAIR_STATUS    4   /* black   on cyan    — status bar         */
#define PAIR_PROGRESS  5   /* green   on default — progress bar       */
#define PAIR_DIM       6   /* white   on default — secondary text     */
#define PAIR_ERROR     7   /* red     on default — errors             */
#define PAIR_HINTS     8   /* yellow  on default — key hints          */
#define PAIR_PLAYING   9   /* green   on default — playing badge      */
#define PAIR_PAUSED   10   /* yellow  on default — paused badge       */
#define PAIR_TITLE    11   /* cyan    on default — track title        */
#define PAIR_BORDER   12   /* white   on default — borders/dividers   */

void init_colors(void);

#endif
