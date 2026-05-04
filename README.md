# 🐭 MazeRunner

A visually rich, interactive maze game built in **C++ with SFML 3.0**, featuring procedurally generated mazes, BFS pathfinding hints, and a backtracking auto-solver.

---

## 📸 Features

- 🎮 **Interactive gameplay** — navigate the maze using keyboard or watch it solve itself
- 🧠 **BFS Hint system** — shows the shortest path to the goal in real time
- 🔁 **Backtracking Auto-Solver** — watch the algorithm explore and solve the maze live
- 🎯 **3 Difficulty levels** — Easy, Medium, Hard with different maze sizes and time limits
- 🏆 **3-Level progression** — beat all 3 levels to win
- ✨ **Particle effects, screen shake, and animated overlays**
- ⏱️ **Timer-based scoring** system

---

## 🧩 Algorithms Used

| Algorithm | Purpose |
|---|---|
| **Recursive Backtracking (DFS)** | Maze generation — carves paths through the grid |
| **Breadth-First Search (BFS)** | Hint system — finds the shortest path from player to goal |
| **Backtracking** | Auto-solver — explores all paths, backtracks on dead ends |

---

## 🛠️ Tech Stack

| Component | Technology |
|---|---|
| Language | C++ 17 |
| Graphics | SFML 3.0 |
| Compiler | MinGW-w64 GCC 15.x (Windows) |
| IDE | Visual Studio Code |

---

## 📁 Project Structure

```
MazeRunner/
├── maze_runner.cpp       ← Main source code
├── ARIAL.TTF             ← Font file (required)
├── maze.exe              ← Compiled executable (after build)
├── sfml-graphics-2.dll
├── sfml-window-2.dll
├── sfml-system-2.dll
├── openal32.dll
└── .vscode/
    └── tasks.json
```

---

## ⚙️ Requirements

| Tool | Version | Link |
|---|---|---|
| MinGW-w64 (GCC) | 15.x | Via Chocolatey |
| SFML | 3.0.0 | [sfml-dev.org](https://www.sfml-dev.org) |
| VS Code | Latest | [code.visualstudio.com](https://code.visualstudio.com) |
| Font | ARIAL.TTF | Copied from `C:\Windows\Fonts\arial.ttf` |

---

## 🚀 Setup & Installation

### Step 1 — Install Chocolatey (PowerShell as Admin)
```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

### Step 2 — Install MinGW
```powershell
choco install mingw -y
```

### Step 3 — Download SFML 3.0.0
Download the **GCC 14.2.0 MinGW 64-bit** build from:
```
https://github.com/SFML/SFML/releases/download/3.0.0/SFML-3.0.0-windows-gcc-14.2.0-mingw-64-bit.zip
```
Extract to `C:\SFML-3.0.0`

### Step 4 — Copy font to project folder
```powershell
Copy-Item "C:\Windows\Fonts\arial.ttf" -Destination "<your-project-folder>\ARIAL.TTF"
```

### Step 5 — Copy SFML DLLs to project folder
```powershell
$proj = "<your-project-folder>"
Copy-Item "C:\SFML-3.0.0\bin\sfml-graphics-2.dll" $proj
Copy-Item "C:\SFML-3.0.0\bin\sfml-window-2.dll"   $proj
Copy-Item "C:\SFML-3.0.0\bin\sfml-system-2.dll"   $proj
Copy-Item "C:\SFML-3.0.0\bin\openal32.dll"         $proj
```

---

## 🔨 Build & Run

```powershell
cd "<your-project-folder>"

g++ maze_runner.cpp -o maze.exe `
  -I"C:/SFML-3.0.0/include" `
  -L"C:/SFML-3.0.0/lib" `
  -lsfml-graphics -lsfml-window -lsfml-system

.\maze.exe
```

Or press **Ctrl + Shift + B** in VS Code if you have `tasks.json` set up.

---

## 🎮 Controls

| Key | Action |
|---|---|
| `W` / `↑` | Move Up |
| `S` / `↓` | Move Down |
| `A` / `←` | Move Left |
| `D` / `→` | Move Right |
| `H` | Toggle BFS hint |
| `B` | Toggle backtracking auto-solver |
| `R` | Restart current level |
| `ESC` | Main menu / Quit |

---

## 🎯 Difficulty Levels

| Difficulty | Maze Size | Time Limit |
|---|---|---|
| **Easy** | 11 × 11 | 120s |
| **Medium** | 15 × 15 | 90s |
| **Hard** | 21 × 21 | 60s |

Select by clicking a button or pressing `[1]`, `[2]`, `[3]` on the menu, then hit `Enter`.

---

## 🏅 Scoring

```
Level Score  =  (Time Remaining × 10)  −  (Moves × 2)  +  300
Total Score  =  Sum of all level scores across 3 levels
```

Complete all 3 levels within the time limit to win. If time runs out, it's game over.

---

## 📐 How the Algorithms Work

### Maze Generation — Recursive Backtracking
Starting from cell (0,0), the algorithm randomly visits unvisited neighbours and carves walls between them, backtracking when no unvisited neighbours remain. This always produces a **perfect maze** — exactly one path exists between any two cells.

```
1. Start at (0,0), mark visited
2. Pick a random unvisited neighbour
3. Remove the wall between current cell and neighbour
4. Recurse into the neighbour
5. If no unvisited neighbours exist, backtrack
```

### BFS Hint — Breadth-First Search
When `[H]` is pressed, BFS explores the maze level by level from the player's position, guaranteeing the **shortest path** to the goal. The path is drawn as green dots.

```
1. Enqueue player's current position
2. Explore all reachable cells layer by layer
3. Track the parent of each visited cell
4. Reconstruct the path backwards from the goal
```

### Auto-Solver — Backtracking
When `[B]` is pressed, the solver tries every direction from the current cell. If it hits a dead end it backtracks and tries another direction, eventually finding the exit.

```
1. From current cell, try all 4 directions
2. Move into any unvisited reachable cell
3. If goal is reached → done
4. If all directions fail → backtrack to previous cell
```

---

## 👨‍💻 Author

**Ayush**
B.Tech CSE | JIIT
