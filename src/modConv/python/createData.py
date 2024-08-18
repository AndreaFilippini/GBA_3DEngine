import argparse
import sys
import os

# class used to limit the values of the range of arguments related to the texture color of the model
class rgbRange(argparse.Action):
    def __call__(self, parser, namespace, values, option_steing=None):
        if values not in range(0,256):
            parser.exit(1, message= f"'ERROR: {self.dest}' should be in [0-255]")
        setattr(namespace, self.dest, values)

# class used to limit the values of the model scale range
class floatRange(argparse.Action):
    def __call__(self, parser, namespace, values, option_steing=None):
        if values < 0.0 or values > 10.0:
            parser.exit(1, message= f"'ERROR: {self.dest}' should be in [0.0-10.0]")
        setattr(namespace, self.dest, values)

# instantiate the argument parser
parser = argparse.ArgumentParser()

# add the converter arguments to the parser
parser.add_argument('-path', help='Path of obj file', required=True)
parser.add_argument('-scl', action=floatRange, metavar="[0.0-10.0]", type=float, help='Rapresents the the scale of the model')
parser.add_argument('-r', action=rgbRange, metavar="[0-255]", type=int, help='r of rgb texture color of the model')
parser.add_argument('-g', action=rgbRange, metavar="[0-255]", type=int, help='g of rgb texture color of the model')
parser.add_argument('-b', action=rgbRange, metavar="[0-255]", type=int, help='b of rgb texture color of the model')

# get the values of the arguments
args = parser.parse_args()
path, scl, r, g, b = args.path, args.scl, args.r, args.g, args.b

# if the file path of the model doesn't exist, then return an error and terminate execution
if not os.path.exists(path):
    print(f"ERROR: {path} file doesn't exist")
    sys.exit()

# if one of the argument values is 'None', then initialize it to a default value
if scl == None:
    scl = 1.0
if r == None:
    r = 255
if g == None:
    g = 255
if b == None:
    b = 255

# initialize the output string and the variable that will contain the contents of the obj file
obj_file = None
output = "struct point points [] = {\n"

# try to read the obj file
try:
    obj_file = open(path, "r").read()
except:
    print(f"ERROR: Could not open {path}")
    sys.exit()

# init accumulators of the vertices and faces of the model
vertex_list, face_list = [], []

# split obj file in lines
obj_file = obj_file.split('\n')

# iterate on each line of the
for line in obj_file:

    # split each line to get the data
    splitted = line.split(' ')

    # if it's a vertex, then put xyz values in the list
    if splitted[0] == 'v':
        vertex_list += [splitted[1:]]
    elif splitted[0] == 'f':
        # otherwise if it's a face, filter the parts of which it is composed, removing unnecessary parts,
        # and creates triangles based on the value of the vertices for the current face
        face_data = [i for i in splitted[1:] if (i != '') and (i != ' ')]
        for i in range(len(face_data) - 2):
            face_list += [[face_data[0], face_data[i+1], face_data[i+2]]]

# init vertex counter
counter = 0

# iter on each face
for face in face_list:
    # iter on each vertex of the current face
    for vertex in face:
        # get the vertex index and get its xyz values by accessing the corresponding list
        vertex_index = vertex.replace('//', '/').split('/')[0]
        vertex = vertex_list[int(vertex_index) - 1]

        # get final xyz values, multiplying them by the scale value
        xyz_values = [int(float(i) * scl) for i in vertex]

        # for each vertex, add the part that encodes it into the final C structure
        output += '	[' + str(counter) + "] = {.x = " + str(xyz_values[0]) +\
                  ", .y = " + str(xyz_values[1]) + ", .z = " + str(xyz_values[2]) +\
                  ", RGB(" + str(r >> 3) + ", " + str(g >> 3) + ", " + str(b >> 3) + ")},\n"

        # increase vertex counter 
        counter += 1

# add the final part of the C structure
output +=  "};"

# generate the output file, containing all the triangles that make up the model
try:
    output_file = open(path + ".txt", "w")
    output_file.write(output)
    output_file.close()
except:
    print(f"ERROR: Could not create output file '{path}.txt'")
