# Bugs
* HBLANK handler `callax` may crash if hblank is delayed past v rendering
* Slopes are currently awful
* corners handled very poorly in general
* ppubuf can be overrun and glitch the game? should just be pausing the update loop until next frame

# Features
* Entities deleted on old sections as map streams
* bumpers
* smashers