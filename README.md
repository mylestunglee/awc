# awc

`awc` is a minimal _Advanced Wars 2_ clone designed with the following  in mind:

- Highly structured C program
- Highly interfaceable text IO
	+ Easy to replay actions and edit maps
- Open source
	+ Highly configurable constants and technical constraints can be remodelled
- Small feature set for high bot compatibility

```html
aaa<b>a</b>
```

## Features

`awc` adds and removes features compared to _Advanced Wars 2_:

- Allow merging and attacking in same unit operation
- different units type do not block movement
- new bridge tile: that allow ships to pass but not block bridges
- world wraps in 256x256 grid
- 255 shared unit capacity
- different unit types (land/air/sea) cannot block each over
- bots can play against each other

lost features over Advanced Wars 2:
- no characters
- no ammo for units/different damages for a unit
- no transports
- no fog/submarine diving

configurable:

features:
- extracted maps from Advanced Wars 2

technical limitations:
- maximum of 4 players (bot and non-bot)
- these limitations can be modified by changing constant


## Usage

### Installation

These installation steps have been tested for _Ubuntu 22.04 LTS_. If you are running _Windows_, consider installing Linux on _WSL_.

1. Download this repository via `git` or _GitHub_.
2. Run `./quick_start.sh`. The script may prompt for permissions to install packages.

### Development tools

| Command | Description |
| --- | --- |
| `make` | Builds the game binary. |
| `make doc` | Generates _Doxgygen_ documentation visualising call graphs and dependency graphs. |
| `make coverage_tests` | Builds all tests and generates code coverage statistics for each test.` |
| `make format` | Lints all source and test code. |
| `make test` | Builds and runs all unit tests. |
| `make clean` | Deletes built files. |

### Game rules

Build and command units to defeat your opponent! Direct units can move and attack in one turn. Ranged units cannot move and attack in one turn. An opponent is defeated when the opponent's head-quarters is captured or all units are destroyed and all buildings are captured.

### Controls

Controls are rebindable in `src/parse.c`.

| Control | Action |
| --- | --- |
| `w` | Pan up |
| `a` | Pan left |
| `s` | Pan right |
| `d` | Pan down |
| `SPACE` | Interact |
| `n` | Next turn |
| `m` | Hover next unit |
| `k` | Self-destruct selected unit |
| `K` | Surrender |
| `q` | Quit |
| `1` | Build option 1 |
| `2` | Build option 2 |
| `3` | Build option 3 |
| `4` | Build option 4 |
| `5` | Build option 5 |
| `6` | Build option 6 |
| `7` | Build option 7 |
| `8` | Build option 8 |
| `9` | Build option 9 |
| `0` | Build option 10 |
| `!` | Load `state1.txt` |
| `"` | Load `state2.txt` |
| `#` | Load `state3.txt` |
| `$` | Load `state4.txt` |
| `%` | Load `state5.txt` |
| `^` | Save `state1.txt` |
| `&` | Save `state2.txt` |
| `*` | Save `state3.txt` |
| `(` | Save `state4.txt` |
| `)` | Save `state5.txt` |

## File structure

Game states can be loaded and saved as ASCII, LF line-ended files. A file is composed of lines that construct the game state. See examples of files at `awc/maps`. A line is one of the following formats:
| Line Format | Description |
| --- | --- |
| `turn t` | start game at turn `t` |
| `map t...` | next row in the map are tiles `t...`. See tile table for format of `t`. |
| `territory p x y` | building at point `(x, y)` is owned player `p` |
| `bot p` | player `p` is a bot |
| `money p m` | player `p` has `m` money |
| `team p ...` | players `p ...` are in the same team |
| `n p x y [h [e [c]]]` | unit of model name `n` is owned by player `p` is at point `(x, y)` has health `h`, has enabled status `e` and has capture process `c`. `e` is either `"enabled"` or `"disabled"`. See unit table for format of `n`.

Square brackets denotes an optional parameter.

## Bot strategy

The bot is an implementation of a naive AI. A bot performs an action on each unit and then tries to builds units.

### Action strategy

For a given unit, the bot scans any targets in the current turn. If enemy units are found, the bot finds the best attack, by comparing damage, counter-damage and unit costs. If the given unit is a capturable unit, finds the closest building. If there are no targets in the current turn, the bot will scan many turns ahead and if a target is found, the bot moves the unit towards the target.

### Build strategy

At the end of a bot turn, the bot tries to find which unit models should be built.
The problem is formalised as a mixed-integer programming problem.
The objective is to maximise the potential damage.
If the potential damage cannot be maximised, then then the maximum number of infantry is built.

Let:
| Variable | Description |
| --- | --- |
| **`A`** | attack matrix, where **`A`**`_i_j` is the damage when model `i` attacks model `j` |
| **`B`**  | build matrix, where **`B`**`_i_k` is the number of model `i` built at building `k` |
| **`a`** | number of buildings, where **`a`**`_k` is the number of building `k` |
| `b` | budget |
| **`c`** | model cost, where **`c`**`_i` is the cost of building model `i` |
| **`d`** | distribution of friendly units, where **`d`**`_i` is the sum of all friendly units' health of model `i` |
| **`d'`** | distribution of enemy units, where **`d'`**`_i` is the sum of all enemy units' health of model `i` |
| `z` | objective variable |

The objective is formalised as:
`z = max_B min_j (∑_i A_i_j (d_i + ∑_i B_i_k)) / d'_j`

with the following constaints:

| Constraint | Description |
| --- | --- |
| `∀k ∑_i B_i_k ≤ a_k` | the build matrix is limited by the number of buildings |
| `∑_i ∑_k c_i B_i_k ≤ b` | the build matrix is limited by budget |

## Game Tables

### Tile Table
	
| Tile Name | Tile Format | Maximum Defense | Building |
| --- | --- | --- | --- |
| Void | `.` | 0 | No |
| Plains | `"` | 1 | No |
| Forest | `Y` | 2 | No |
| Mountain | `^` | 4 | No |
| Sand | `:` | 0 | No |
| Sea | `~` | 0 | No |
| Coral | `*` | 1 | No |
| River | `'` | 0 | No |
| Road | `-` | 0 | No |
| Bridge | `=` | 0 |No |
| City | `C` | 3 | Yes |
| Factory | `F` | 3 | Yes |
| Airport | `A` | 3 | Yes |
| Seaport | `S` | 3 | Yes |
| Headquarters | `Q` | 4 | Yes |

### Unit table

| Unit name | Visual | Cost | Attack Type | Movement Type | Movement Range | Capturable |
| --- | --- | --- | --- | --- | --- | --- |
| infantry | `ooo`<br>`^^^` | 1000 | Direct | Infantry | 3 | Yes |
| mech | `===`<br>`^^^` | 3000 | Direct | Mech | 2 | Yes |
| recon | `/--`<br>`ooo` | 4000 | Direct | Wheel | 8 | No |
| tank | `/[ ]-`<br>`o===o` | 7000 | Direct | Tread | 6 | No |
| battletank | `[  ]=`<br>`o===o` | 16000 | Direct | Tread | 5 | No |
| artillery | `^^^`<br>`o=o` | 6000 | Ranged (2 → 3) | Tread | 5 | No |
| rockets | `^^^^^`<br>`ooooo` | 15000 | Ranged (3 → 6) | Wheel | 5 | No |
| antiair | `///`<br>`o=o` | 8000 | Direct | Tread | 6 | No |
| missles | `//////`<br>`ooooo` | 12000 | Ranged (2 → 5) | Wheel | 5 | No |
| battlecopter | `=x=`<br>` [ ]=` | 9000 | Direct | Air | 6 | No |
| fighter | `\    `<br>`<\ ]=` | 22000 | Direct | Air | 9 | No |
| bomber | `\    `<br>`<vvv>` | 20000 | Direct | Air | 7 | No |
| submarine | `_____`<br>`\__]=` | 20000 | Direct | Sea | 5 | No |
| crusier | `_///_`<br>`\__]=` | 18000 | Direct | Sea | 6 | No |
| battleship | `^^^^^`<br>`\___/` | 28000 | Ranged (2 → 7) | Sea | 5 | No |
