#include "game_fixture.hpp"

game_fixture::game_fixture() : game(new struct game) { game_initialise(game); }

game_fixture::~game_fixture() { delete game; }
