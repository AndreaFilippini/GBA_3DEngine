# GBA 3DEngine
3D Engine for GBA to visualize 3D models on screen.

# Dependencies
[VB.NET](https://learn.microsoft.com/it-it/dotnet/visual-basic/)

[Devkit PRO](https://sourceforge.net/projects/devkitpro/)

# OBJ Model
First, you need to take the model in .obj format that you want to put inside the game.
There is an example file in the src folder called "example.obj" like the one below:

![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/example.png)

Since this number of polygons in most of the models is too large to be supported by the GBA console, it's necessary to sample them, taking a subset with a converter.

A good range for the number of polygons (triangles) is between 200 and 500.

The number of polygons is given at the end of the obj file as an annotation (e.g. # 400 polygons).

# Convert Model to C data
To convert the model to a suitable C data we can use the executable file "createData.exe" in the **src/modConv/VBConv** folder.

Once either program has been executed, a txt file containing a points[] array of xyz triangles will be generated to and which can be inserted directly into our C code with a simple import directive.

# Convert Model with VB.NET
To convert the OBJ model open the CMD prompt and type **createData arg0 arg1 arg2** where:
- arg0 contains the **path** of the model with .obj extension
- arg1 contains the **scale**, a float that rapresents the the scale of the model (put 1 if you don't want to change the scale) 
- arg2 contains the **resolution**, which indicates the sampling interval (e.g., if we put 2 we will have 400 polygons / 2 = 200 triangles in the final model)

You can view the source code of executable in "Module1.vb" file in the same folder.

# 3DEngine
Once you have downloaded Devkit Pro, you can place the files contained in src inside the **DEVKITARM-R41_WIN32/devkitARM/bin** folder.

The engine is based on projection matrices:

[3D Projection matrices](https://en.wikipedia.org/wiki/3D_projection)

Initially, screen-related parameters are set for proper display using the **main** function.

After that, in the main loop, we proceed to display the model on the screen with the **render3D** function.

Each point of a triangle is multiplied by a specific matrix in the **projectPoint** function to obtain the coordinates of the new triangle to be displayed on the screen.

The display of individual vertices the **setSanitizePixel()** function is used, which translates the coordinates according to a cartesian reference system starting from the origin (center of the screen) and then calls the **setPixelToCanvas()** function to actually draw the pixel on the screen.

# Compile C code to GBA ROM
To compile the code, it is necessary to call the only **gbac.bat**.
The bat file will execute all the necessary compilers in the DevKitARM folder, going from the C file to an asm file, moving to an elf file, and finally to the final rom with **gba** extension.

# Final Result
![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/random.gif)
![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/torus.gif)
![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/sphere.gif)

Any GBA emulator, such as visual boy advance, can be used to view the final result.
