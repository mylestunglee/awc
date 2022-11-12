#ifndef CONTROLLER_H
#define CONTROLLER_H

struct game;

void controller_run(struct game* const game, char (*getch)(void));

#endif
