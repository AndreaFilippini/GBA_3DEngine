Imports System.IO

Module Module1

    Sub Main(ByVal args() As String)
        If args.Length <> 5 Then
            Console.WriteLine("ERROR #0 - Wrong parameters number")
            Return
        End If

        If Not My.Computer.FileSystem.FileExists(args(0)) Then
            Console.WriteLine("ERROR #1 - Obj file not found")
            Return
        End If

        Dim scale As Double

        If Not Double.TryParse(Replace(args(1), ".", ","), scale) Then
            Console.WriteLine("ERROR #2 - Incorrect scale")
            Return
        End If

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

        finalString += "struct point points [] = {" + vbCrLf

        Dim Lines = File.ReadAllLines(args(0))
        Dim vertexCount = 30000
        Dim faceList As New ArrayList
        Dim vertexList(vertexCount) As String

        For Each line In Lines
            Dim splittedLine() As String = line.Split
            If splittedLine(0) = "v" Then
                vertexList(counter) = line
                counter += 1
            ElseIf (splittedLine(0) = "f") Then
                If splittedLine.Length = 6 Then
                    Dim newTriangle As String = splittedLine(0) + " " + splittedLine(1) + " " + splittedLine(2) + " " + splittedLine(3)
                    faceList.Add(newTriangle)
                    newTriangle = splittedLine(0) + " " + splittedLine(1) + " " + splittedLine(3) + " " + splittedLine(4)
                    faceList.Add(newTriangle)
                Else
                    faceList.Add(line)
                End If
            End If
        Next

        For Each face In faceList
            Dim splittedLine() As String = face.Split
            Dim currentVertex() As String
            For Each vertexIndex In splittedLine

                If (vertexIndex <> "f") And (vertexIndex <> " ") And (vertexIndex <> "") Then
                    Dim processedVertexIndex As String = vertexIndex.Split("/")(0)

                    currentVertex = Replace(vertexList(processedVertexIndex - 1), ".", ",").Split

                    roundX = (currentVertex(1) * scale)
                    tempNum = Math.Round(Convert.ToDouble(roundX), 0)
                    roundX = tempNum.ToString
                    roundY = (currentVertex(2) * scale)
                    tempNum = Math.Round(Convert.ToDouble(roundY), 0)
                    roundY = tempNum.ToString
                    roundZ = (currentVertex(3) * scale)
                    tempNum = Math.Round(Convert.ToDouble(roundZ), 0)
                    roundZ = tempNum.ToString

                    finalString += "	" + "[" + indexNum.ToString + "] = {.x = " + roundX + ", .y = " + roundY + ", .z =  " + roundZ + ", RGB(" +
                                          (r >> 3).ToString + ", " + (g >> 3).ToString + ", " + (b >> 3).ToString + ")}," + vbCrLf
                    indexNum += 1
                End If
            Next
        Next

        finalString += "};"

        Try
            File.WriteAllText(args(0) + ".txt", finalString)
        Catch
            Console.WriteLine("ERROR #5 - Error during output file creation")
            Return
        End Try

    End Sub

End Module
