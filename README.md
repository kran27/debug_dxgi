# Kran's DebugDXGI

dxgi (or dinput8) shim dll to help debugging and patch crashes/other things, mainly in debug versions of games

comes with patches for a few games (/patches directory):

| Game                   | Patch                                                            |
|------------------------|------------------------------------------------------------------|
| All Games              | Logs debug strings and exceptions                                |
| Fallout 4              | Fixes crash with `tcd` command and infinite loop on modded saves |
| Dishonored 2           | Prevents crash due to expired Havok license                      |
| Immortals Fenyx Rising | Handles int3 exception to prevent Bloomberg exiting the game     |
| Starfield              | Fixes add credits & fuel menu options, and popup when starting   |
| Hunter: The Reckoning  | Fixes various crashes experienced in the world                   |