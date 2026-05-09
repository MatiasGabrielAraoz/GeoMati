# GeoMati
**GeoMati** is a real time mathematical function plotter made in **C** with the **SDL3** library. It allows you to visualize mathematical expressions inputted via command-line arguments, with help of the **TinyExpr** parser for complex formula evaluation.

# What features does it have
* **Real-time rendering**: it provides instant visualization of mathematical functions.
* **Dynamic cartesian plane**: The axes and points get instantly recalculated every time you resize the window, assuring all of the planes will be centered.
* **Memory Managed:** Implemented using a custom dynamic array system and verified with AddressSanitizer to ensure zero memory leaks.

## requirements

* **SDL3**: You must have the latest SDL3 Development libraries installed in your system
* **C compiler**: You must have clang or change the COMPILER variable to the compiler you want (cc or gcc)

### How to compile
To compile you must execute this command:
```bash
make main
```

## Usage
You have to run the executable and pass the desired function as a string argument:
```bash
./GeoMati "x^2"
./Geomati "sin(x) * 50"
``` 

