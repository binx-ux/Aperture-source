# Offsets

## Fortnite

file: `Offsets.hpp`

kyn keeps these updated after Fortnite patches. when the game updates:

1. pull latest `main`
2. rebuild
3. if it still fails, open a bug issue with your FN version / what fails (GWorld, actors, etc.)

you can still send a PR with new offsets if you want. include the version number.

## FiveM

file: `FiveM.hpp` (patterns + per-build fields)

FiveM is on you. different artifacts (`bXXXX`) need different tables. when your client updates:

1. fix patterns / offsets for that build
2. test on that build
3. PR is welcome if it helps others on the same artifact

do not open issues that are only "FiveM updated, fix it for me" with no build info and no attempt.
