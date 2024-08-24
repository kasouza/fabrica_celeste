# Fabrica Celeste
3D blocky game about factories in the sky or something.

Very very very early in dev right now (can't even call it a game honestly)

## How to build and run
*You will need vcpkg installed on your machine, see [how to install here](https://learn.microsoft.com/pt-br/vcpkg/get_started/get-started?pivots=shell-cmd)*
```
cmake --preset=default
cmake --build build
./build/fabrica
``` 

## Important notes
### Dependencies
- This project users opengl 3.3, so make sure your machine supports it.
- This project uses vcpkg to manage dependencies, so make sure you have it installed.
- This project uses cmake to build, so make sure you have it installed.
- This project uses clang-format to format the code, so make sure you have it installed.

### Code style
#### Memory management
- Most functions do not allocate the memory for the return value, so make sure to pass a pointer to the return value. This allows the caller to allocate the memory in the stack or heap.
    - Ex: `fabrica_world_init` will initialize a `fabrica_World` object passed as an argument. The caller must allocate the memory for the object: `fabrica_World world; fabrica_world_init(&world);`
    - If there happens to be an exception to this rule, the function function that allocates the memory will be named `*_create` and will return it to the caller instead of receiving a pointer to the object as an argument.
        - Ex: `fabrica_world_create` will allocate memory for a `fabrica_World` object and return it to the caller: `fabrica_World* world = fabrica_world_create();`

- `*_destroy` functions will free the memory used by the object, but the object itself will not be destroyed. This is to allow the caller to free the memory in the stack or heap.
    - Ex: `fabrica_world_init` will initialize a `fabrica_World` object passed as an argument. If later the `fabrica_world_destroy` function is called, the memory used internally by the object will be freed, but the object itself will not be destroyed. This allows the caller to free the memory in the stack or heap.
    - If there happens to be an exception to this rule, the function that frees the memory will be named `*_free` and will take a pointer to the object as an argument, freeing the memory used by the object and the *object itself*.
        - Ex: `fabrica_world_free` will free all memory used by the `fabrica_World` object and the object itself: `fabrica_World world; fabrica_world_init(&world); fabrica_world_free(&world); // world is now invalid`

- When memory needs to be allocated to be used internally by the object, the function will take an `fabrica_Allocator` object as an argument. This object will be used to allocate and free memory.
