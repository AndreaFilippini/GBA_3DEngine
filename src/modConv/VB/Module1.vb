Imports System.IO

Module Module1

    Sub Main(ByVal args() As String)

        'check number of parameters
        If args.Length <> 5 Then
            Console.WriteLine("ERROR #0 - Wrong parameters number")
            Return
        End If

        'check if obj file exists
        If Not My.Computer.FileSystem.FileExists(args(0)) Then
            Console.WriteLine("ERROR #1 - Obj file not found")
            Return
        End If

        Dim scale As Double

        'get obj scale
        If Not Double.TryParse(Replace(args(1), ".", ","), scale) Then
            Console.WriteLine("ERROR #2 - Incorrect scale")
            Return
        End If

        'parse (r g b) texture color
        Dim r, g, b As Integer
        If Not Integer.TryParse(args(2), r) Then
            Console.WriteLine("ERROR #3 - Incorrect R value")
            Return
        End If

        If Not Integer.TryParse(args(3), g) Then
            Console.WriteLine("ERROR #3 - Incorrect G value")
            Return
        End If

        If Not Integer.TryParse(args(4), b) Then
            Console.WriteLine("ERROR #3 - Incorrect B value")
            Return
        End If

        If (r < 0) Or (r > 255) Then
            Console.WriteLine("ERROR #4 - R value outside range")
            Return
        End If

        If (g < 0) Or (g > 255) Then
            Console.WriteLine("ERROR #4 - G value outside range")
            Return
        End If

        If (b < 0) Or (b > 255) Then
            Console.WriteLine("ERROR #4 - B value outside range")
            Return
        End If

        Dim counter As Integer = 0
        Dim indexNum As Integer = 0
        Dim roundX, roundY, roundZ As String
        Dim tempNum As Double
        Dim finalString As String = String.Empty
        Dim currentState As Integer = 0

        'init C struct
        finalString += "struct point points [] = {" + vbCrLf

        Dim Lines = File.ReadAllLines(args(0))
        Dim faceList As New ArrayList
        Dim vertexList As New ArrayList

        'iterate over each obj line
        For Each line In Lines
            Dim splittedLine() As String = line.Split

            'if the line contains a vertex, added in the vertex list
            If splittedLine(0) = "v" Then
                vertexList.Add(line)
                counter += 1

            ElseIf (splittedLine(0) = "f") Then
                ' otherwise If it's a face, filter the parts of which it is composed, removing unnecessary parts,
                ' and creates triangles based on the value of the vertices For the current face
                Dim cleanSplit As New ArrayList
                For Each part In splittedLine
                    If part <> "" And part <> " " Then cleanSplit.Add(part)
                Next
                For faceIndex As Integer = 1 To (cleanSplit.Count() - 3)
                    Dim newTriangle As String = cleanSplit(0) + " " + cleanSplit(1) + " " + cleanSplit(faceIndex + 1) + " " + cleanSplit(faceIndex + 2)
                    faceList.Add(newTriangle)
                Next
            End If
        Next

        'iterate over each collected face
        For Each face In faceList
            Dim splittedLine() As String = face.Split
            Dim currentVertex() As String
            For Each vertexIndex In splittedLine

                If (vertexIndex <> "f") And (vertexIndex <> " ") And (vertexIndex <> "") Then

                    'get the values based on the scale set as a parameter and build the C structure
                    Dim processedVertexIndex As String = Replace(vertexIndex, "//", "/").Split("/")(0)
                    currentVertex = Replace(vertexList(processedVertexIndex - 1), ".", ",").Split

                    ' get final xyz values, multiplying them by the scale value
                    roundX = (currentVertex(1) * scale)
                    tempNum = Math.Round(Convert.ToDouble(roundX), 0)
                    roundX = tempNum.ToString
                    roundY = (currentVertex(2) * scale)
                    tempNum = Math.Round(Convert.ToDouble(roundY), 0)
                    roundY = tempNum.ToString
                    roundZ = (currentVertex(3) * scale)
                    tempNum = Math.Round(Convert.ToDouble(roundZ), 0)
                    roundZ = tempNum.ToString

                    ' for Each vertex, add the part that encodes it into the final C Structure
                    finalString += "	" + "[" + indexNum.ToString + "] = {.x = " + roundX + ", .y = " + roundY + ", .z =  " + roundZ + ", RGB(" +
                                        (r >> 3).ToString + ", " + (g >> 3).ToString + ", " + (b >> 3).ToString + ")}," + vbCrLf

                    ' increase vertex counter
                    indexNum += 1
                End If
            Next
        Next

        ' add the final part of the C structure
        finalString += "};"

        'create a file containing the final C struct
        Try
            File.WriteAllText(args(0) + ".txt", finalString)
        Catch
            Console.WriteLine("ERROR #5 - Error during output file creation")
            Return
        End Try

    End Sub

End Module
