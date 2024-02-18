# GBA 3DEngine
3D Engine for GBA, specifically for Pokémon Ruby (USA) game, to visualize 3D models on screen.

# Dependencies
[VB.NET](https://learn.microsoft.com/it-it/dotnet/visual-basic/) or [Haskell](https://www.haskell.org/)

[Devkit PRO](https://sourceforge.net/projects/devkitpro/)

# OBJ Model
First, you need to take the model in .obj format that you want to put inside the game.
There is an example file in the src folder called "example.obj" like the one below:

![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/example.png)

As can be seen, the model has about 250,000 vertices.

Since this number of vertecies is too large to be supported by the GBA console, it's necessary to sample them, taking a subset.

A good range for the number of vertices is between 200 and 500.

# Convert Model to C data
To convert the model to a suitable C data we can use two converters:
- the haskell program "vertex.hs" in the **src/modConv/haskellConv** folder
- the executable file "createData.exe" in the **src/modConv/VBConv** folder

Once either program has been executed, a txt file containing a points[][] matrix of xyz coordinates will be generated to and which can be inserted directly into our C code with a simple import directive.

# Convert Model with VB.NET
To convert the OBJ model open the CMD prompt and type **createData arg0 arg1 arg2** where:
- arg0 contains the **path** of the model with .obj extension
- arg1 contains the **scale**, a float that rapresents the the scale of the model (put 1 if you don't want to change the scale) 
- arg2 contains the **resolution**, which indicates the sampling interval (e.g., if we put 600 we will have 257087 vertices / 600 = 428 vertices in the final model)

You can view the source code of executable in "Module1.vb" file in the same folder.

# Convert Model with Haskell
After starting GHCi, the interactive version of the GHC compiler, at the command prompt, load the module with **:l vertex** command.

Next we can call **:main ObjPath**, where ObjPath is the path to the model you want to convert.

In the haskell main program there are two parameters to be changed according to one's needs:
- **Scale** in scaleXYZ, a float that rapresents the the scale of the model (put 1 if you don't want to change the scale) 
- **Resolution** in resolution function, which indicates the sampling interval (e.g., if we put 600 we will have 257087 vertices / 600 = 428 vertices in the final model)

# 3DEngine
Once you have downloaded Devkit Pro, you can place the files contained in src inside the **DEVKITARM-R41_WIN32/devkitARM/bin** folder.

The engine is based on projection matrices:

[3D Projection matrices](https://en.wikipedia.org/wiki/3D_projection)

Initially, screen-related parameters are set for proper display using the **main** function.

Inside it, the functions **loadCanvas()** and **loadModelPal()** are called to load the data in BG0 and model palettes, respectively.

The GBA screen has 4 layers called BG ranging from BG0 (highest priority) to BG3 (lowest priority).

Each model coordinate is multiplied by a specific matrix within the **projectPoint** function to obtain the new point coordinate to be displayed on the screen.

The display of individual vertices the **setSanitizePixel()** function is used, which translates the coordinates according to a cartesian reference system starting from the origin (center of the screen) and then calls the **setPixelToCanvas()** function to actually draw the pixel on the screen.

# Compile C code
To compile the code, it is necessary to call the **casm.bat** file with a single parameter, specifically the path of the 3DEngine .c file.

You need to specify the memory offset where you want to insert the code in the preprocessor directive **#define StartROM** (default value 0x08700000).

The file will initially be converted to an assembly file with the extension .asm and then to a binary file.

Any hex editor can be used to enter the binary file.

The scripting code to call the code inserted inside the rom must follow the form::

**#dynamic 0x800000**

**#org @start**

**callasm 0xStartROM+1**

**end**

The insertion of this script is done with the help of a couple of GBA pokémon game hacking tools called **XSE** and **Advance Map**.

# Final Result
![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/example.gif)
