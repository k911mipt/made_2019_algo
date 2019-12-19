#include "position.hpp"


std::function<bool(uint64)> Position::move_allowers[4] = { &Position::AllowedUp , &Position::AllowedDown , &Position::AllowedLeft, &Position::AllowedRight };
std::function<uint64(uint64)> Position::movers[4] = { &Position::MoveUp , &Position::MoveDown , &Position::MoveLeft, &Position::MoveRight };
