1. Figure out how struct declarations work and if they should go in your header files

2. Create a stop button that frees all the memory we allocated on the heap

3. Do we want to implement paletted color?

4. Use consts everywhere, for everything

5. Think of a better system for initialization and dealing with the boundary - I don't like these separate JS start() and C run() functions - we also need to figure out how to handle input, tick, the C-side update(), and synchronization

~~6. Go back to the section on parsing PLG files and let's write an OBJ->JSON transformer and use the same method we used for wasmface to bring them into the world (i think we should just send serialized plain ASCII over the wire so as to avoid needing a JSON library in C)~~

7. What's up with this awful line clipping algo? Can we remove it from our project and implement something better? Sutherland-Cohen algorithm?

8. Develop a more rigorous understanding of all linear algebra: calculating magnitude of a 3d vector, deriving a 3d vector from two 3d points, dot product, cross product, muliplying two 4x4 matrices together, all transformations. Do the same for Bresenham's algorithm, etc. Prepare lightning talks on these topics to cement your knowledge

9. Deal with circular dependencies and forward declaration of typedef structs - you currently cannot have a function prototype for drawObjectWire in gfx.h

10. render.c is a better name than gfx.c

11. Deep think on how to best modularize the code to avoid circular dependencies

12. Implement lookup tables for trig functions
