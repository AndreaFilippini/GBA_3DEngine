--load file source with ":l vertex"
--call main with ":main objPath"
-- change parameter of scaleXYZ in main to scale the model
-- change parameter of resolution in main to define the number of vertex (one vertex every...)

import System.Environment
import System.IO

main = do
    args <- getArgs
    inputStr <- readFile (args !! 0)
    let input = map words (lines inputStr)
    let vertex = filter getVertex input
    let vertexNum = (head (filter getVertexNum input)) !! 2
    let dropVertex = resolution 600 vertex
    let coords = map fromFloatToXYZ (map (drop 1) dropVertex)
    let scaling = map (scaleXYZ 0.88) coords
    let outFilePath = ((args !! 0) ++ ".txt")
    let outStr = "const struct point points [] = {\n" ++ createStruct scaling
    outFile <- openFile outFilePath WriteMode
    hPutStr outFile outStr
    hClose outFile
    print (outFilePath ++ " created")
    putStr "Number of vertex: "
    print (length dropVertex)

getVertexNum (label:typ:_) = (label == "#Vertex") && (typ == "Count")

getVertex [] = False
getVertex (label:_) = (label == "v")

fromFloatToXYZ xs = map (fromStrintToInt.fromFloatToInt) xs

fromStrintToInt x = read x :: Integer

fromFloatToInt ('.':_) = []
fromFloatToInt (x:xs) = x:fromFloatToInt xs

scaleXYZ scale xs = map (\x -> round (scale * (fromIntegral x))) xs

createStruct [] = "\n};"
createStruct ((x:y:z:[]):xs) = "    {.x = " ++ (show x) ++
                                 ", .y = " ++ (show y) ++
                                 ", .z = " ++ (show z) ++ "},\n" ++
                                 createStruct xs

resolution vertexNum xs = do
    let newVertexList = drop vertexNum xs
    if(newVertexList == []) then [] else (head newVertexList):resolution vertexNum newVertexList


