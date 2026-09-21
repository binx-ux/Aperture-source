# Aperture

internal dll for Fortnite and FiveM. menu, aim (mouse move), esp, local key gate that looks like a loader screen. thats it.

i went through this code a lot before leaving it like this. build is clean on vs (release x64).

---

## important (read this)

**this repo is only the cheat code / dll.**

there is:
- no injector
- no external loader exe that maps the dll
- no eac / anticheat bypass
- no driver

if you need those, find them yourself online or write your own. dont open issues asking for an inject method or a bypass. im not putting that here on purpose.

fortnite live with eac will reject normal loadlibrary inject. that is expected. this project does not solve that.

---

## offsets

**Fortnite:** i (kyn) will update `Offsets.hpp` after FN patches so you dont have to. pull latest when the game updates. if something still breaks open an issue with the build / version.

**FiveM:** those are on you. builds change a lot and i am not chasing every artifact. update patterns / build offsets in `FiveM.hpp` yourself when your client moves.

---

## build

open `FortniteExternalCheat.sln` in visual studio, or:

```
MSBuild FortniteExternalCheat.sln /p:Configuration=Release /p:Platform=x64
```

needs:
- vs with c++ desktop workload
- imgui + minhook already under `third_party\`

output:
`bin\Release\Aperture.dll`

more detail in [docs/build.md](docs/build.md).

---

## how it attaches

dll checks the host process:
- FortniteClient-Win64-Shipping -> fortnite path
- FiveM_* GameProcess / GTAProcess -> fivem path

if its neither, it does nothing.

after present hook is up you get a small license ui first. local keys only (no remote panel). then insert toggles the real menu.

defaults:
- insert = menu
- right mouse = aim when enabled

---

## docs and community

- [docs/](docs/) - build, offsets, faq
- [CONTRIBUTING.md](CONTRIBUTING.md) - how to PR / report stuff
- [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)
- [SECURITY.md](SECURITY.md)
- [SUPPORT.md](SUPPORT.md)
- use **Issues** for bugs and questions (templates are there)
- Discussions are on if you just want to talk

---

## selling / redistributing

you can use this, fork it, even sell a product based on it.

**you must credit:**
- kyn
- Dola17
- vvar1thes
- and the other people listed in `NOTICE` / Credits below

put the names somewhere users can actually see (readme, site, product ui). stripping credits or claiming you made the whole thing alone is not ok. full rules are in `LICENSE`.

---

## credits

- **kyn** - this build / maintainer (FN offsets)
- **Dola17** - project
- **vvar1thes** - original base and a lot of the early files
- **Dear ImGui** (ocornut)
- **MinHook** (TsudaKageyu)

see `LICENSE` and `NOTICE`.

---

## disclaimer

as-is. no warranty. bans, crashes, broken offsets after updates, legal stuff, all on you. this is for learning / research on your own machines. respect game tos and local law.
