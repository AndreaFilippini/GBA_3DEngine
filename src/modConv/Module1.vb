Imports System.IO

Module Module1

    Sub Main(ByVal args() As String)
        If args.Length <> 3 Then
            Console.WriteLine("ERROR #0 - Wrong parameters number")
            Return
        End If

        If Not My.Computer.FileSystem.FileExists(args(0)) Then
            Console.WriteLine("ERROR #1 - Obj file not found")
            Return
        End If

        Dim scale As Double
        Dim range As Integer

        If Not Double.TryParse(args(1), scale) Then
            Console.WriteLine("ERROR #2 - Incorrect scale")
            Return
        End If

        If Not Integer.TryParse(args(2), range) Then
            Console.WriteLine("ERROR #3 - Incorrect sampling range")
            Return
        End If

        Dim counter As Integer = 0
        Dim globalCounter As Integer = 0
        Dim indexNum As Integer = 0
        Dim roundX, roundY, roundZ As String
        Dim tempNum As Double
        Dim finalString As String = String.Empty
        Dim currentState As Integer = 0

        finalString += "const struct point points [] = {" + vbCrLf

        Dim Lines = File.ReadAllLines(args(0))
        Dim vertexCount = 400
        Dim faceList As New ArrayList
        Dim vertexList(vertexCount) As String

        counter = 0
        For Each line In Lines
            Dim splittedLine() As String = line.Split
            If (splittedLine.Length = 4) And (splittedLine(0) = "v") Then
                vertexList(counter) = line
                counter += 1
            ElseIf (splittedLine(0) = "f") Then
                If splittedLine.Length = 6 Then
                    Dim newTriangle As String = splittedLine(0) + " " + splittedLine(2) + " " + splittedLine(3) + " " + splittedLine(4)
                    faceList.Add(newTriangle)
                    newTriangle = splittedLine(0) + " " + splittedLine(1) + " " + splittedLine(2) + " " + splittedLine(3)
                    faceList.Add(newTriangle)
                Else
                    faceList.Add(line)
                End If
            End If
        Next

        counter = 0
        For Each face In faceList
            counter += 1
            counter += 1
            If (counter >= range) Then
                counter = 0

                Dim splittedLine() As String = face.Split
                Dim currentVertex() As String
                For Each vertexIndex In splittedLine

                    If (vertexIndex <> "f") And (vertexIndex <> " ") And (vertexIndex <> "") Then

                        Dim processedVertexIndex As String = vertexIndex.Split("/")(0)

                        currentVertex = vertexList(processedVertexIndex - 1).Split

                        roundX = currentVertex(1).Split(".")(0)
                        tempNum = Convert.ToInt32(roundX) / 1
                        roundX = tempNum.ToString
                        roundY = currentVertex(2).Split(".")(0)
                        tempNum = Convert.ToInt32(roundY) / 1
                        roundY = tempNum.ToString
                        roundZ = currentVertex(3).Split(".")(0)
                        tempNum = Convert.ToInt32(roundZ) / 1
                        roundZ = tempNum.ToString

                        finalString += "	" + "[" + indexNum.ToString + "] = {.x = " + roundX + ", .y = " + roundY + ", .z =  " + roundZ + "}," + vbCrLf
                        indexNum += 1
                    End If
                Next

            End If

        Next

        finalString += "}"

        Try
            File.WriteAllText(args(0) + ".txt", finalString)
        Catch
            Console.WriteLine("ERROR #6 - Error during output file creation")
            Return
        End Try

    End Sub

End Module
