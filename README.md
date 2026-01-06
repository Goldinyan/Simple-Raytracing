
# Simple Raytracing (SDL2)

A minimal, fast and clean implementation of **raytracing / ray‑casting** in C.  
Rendering is done using **SDL2**, and the simulation demonstrates light rays interacting with circular objects.

---

## Features

- Light source that follows the mouse  
- Rays emitted in all directions   
- Simple shadow simulation  
- Minimal and readable C code  

---

## Requirements

You need:

- a C compiler (e.g. `clang` or `gcc`)
- SDL2 development libraries  
  (on macOS via Homebrew: `brew install sdl2`)

---

## Build

Compile using:

```sh
gcc main.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -lm -o main
```

**Important:**  
You must link against:

- `-lSDL2` → SDL library  
- `-lm` → math library (for `sin`, `cos`, `pow`, etc.)

---

## Run

```sh
./main
```

---

## Controls

- Move the mouse -> the light source follows the cursor  
- Rays update in real time  
- The large circle moves automatically and casts a shadow  

---

## Repository

Clone the project:

```sh
git clone https://github.com/Goldinyan/Simple-Raytracing.git
```

---

## License

MIT License — see `LICENSE` file.

---
