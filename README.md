# Minecraft clone using C++ and OpenGL

### About

This project is the result of what I've learnt from a set of excellent tutorial articles on OpenGL at ![learnopengl.com](https://learnopengl.com/).

OpenGL is merely a specification; its implementation is found in your computer's graphic drivers (NVIDIA, AMD, Intel, etc.). To facilitate the use of these implementations:
- `GLAD` is used to access OpenGL functions.
- `GLFW` is used to manage the window and OpenGL context.

I used GLM, which provides useful mathematical functions such as `glm::dot` and data types like `glm::vec3` and `glm::mat4`. It facilitates matrix multiplication and the creation of matrices such as the projection matrix using `glm::perspective`, for example.

To load the texture image, I used `stb_image.h` created by Sean Barrett, which you can find ![here](https://github.com/nothings/stb/blob/master/stb_image.h).

### Classes

- `Game` : keeps track of blocks, does rendering, manages creation and destruction of blocks, and processes input.
- `Camera` : produces view and projection matrices from its basis vectors which are continuously updated in `Game`.
- `Block` : renders one block.
- `ShaderProgram` : an easy way to create a shader program just from a filepath to a vertex and fragment shader. Allows setting of uniforms.
- `Crosshair` : renders the crosshair ontop of the screen.

### Game Controls

- Plane movement : `WASD`
- Up : `SPACE`
- Down : `LEFT SHIFT`
- Zoom in / out : `SCROLL`
- Place block : `LEFT CLICK`
- Remove block : `RIGHT CLICK`
- Place sand : `1`
- Place grass : `2`
- Place dirt : `3`
- Place gravel : `4`
- Place snow : `5`
- Place cherry leaves : `6`
- Place oak log : `7`
