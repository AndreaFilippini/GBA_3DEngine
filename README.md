# GBA 3DEngine
3D Engine for GBA to visualize 3D models on screen.

# Dependencies
[Devkit PRO](https://sourceforge.net/projects/devkitpro/)

[VB.NET](https://learn.microsoft.com/it-it/dotnet/visual-basic/) (only if you want to edit the obj converter)

[Python](https://www.python.org/) (only if you don't want to use the VB converter or you want to edit the python converter)

# OBJ Model, Conversion and Data Structure
First, you need to take the model in .obj format that you want to visualize.

To convert the model to a suitable C data you can use the executable file "createData.exe" in the **src/modConv/VB** folder.

Alternatively, you can use the script "createData.py" in the **src/modConv/python** folder.

Once the program or the python script has been executed, a txt file containing an array of points will be generated to and which can be inserted directly into our C code with a simple import directive.

Each triangle is represented by three consecutive points in the generated array.

# Convert OBJ Model to C data
To convert the OBJ model with the VB converter, open the CMD prompt and type **createData arg0 arg1 r g b** where:
- arg0 contains the **path** of the model with .obj extension
- arg1 contains the **scale**, a float that rapresents the the scale of the model (put 1 if you don't want to change the scale) 
- r,g,b represent the base color of the model, with values ranging from 0 to 255

For example, if you have torus in your obj file and you want its size to be half the original and with white as the base color just write ***createData torus.obj 0,5 255 255 255***.

You can view the source code of executable in "Module1.vb" file in the same folder.

In case you wanted to use the python converter, open the CMD prompt and type **python createData.py -path arg0 -scl arg1 -r r -g g -b b** where ***arg0 arg1 r g b*** have the same meaning as shown above.

If you want to get the same result as before, just write ***python createData.py -path torus.obj -scl 0.5 -r 255 -g 255 -b 255***.

# 3DEngine
Once you have downloaded Devkit Pro, you can place the files contained in src inside the **DEVKITARM-R41_WIN32/devkitARM/bin** folder.

The engine is based on projection matrices:

[3D Projection matrices](https://en.wikipedia.org/wiki/3D_projection)

Initially, screen-related parameters are set for proper display using the **main** function.

After that, in the main loop, we proceed to display the model on the screen with the **render3D** function, in which each point of a triangle is multiplied by a specific matrix in the **projectPoint** function to obtain the coordinates of the new triangle to be displayed on the screen.

# Compile C code to GBA ROM
To compile the code, it's necessary to call **gbac.bat**.
The bat file will execute all the necessary compilers in the DevKitARM folder, going from the C file to an asm file, moving to an elf file, and finally to the final rom with **gba** extension.

# Final Result
![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/3Dtorus.png)
![](https://github.com/AndreaFilippini/GBA_3DEngine/blob/main/images/3Dhouse.png)

Any GBA emulator, such as visual boy advance, can be used to view the final result.
