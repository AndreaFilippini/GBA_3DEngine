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

        For Each line In Lines
            Dim splittedLine() As String = line.Split
            If (splittedLine.Length = 4) And (splittedLine(0) = "v") Then
                If (currentState = 0) Then
                    currentState = 1
                End If

                counter += 1
                If (counter >= range) Then
                    counter = 0

                    Try
                        roundX = splittedLine(1).Split(".")(0)
                        tempNum = Convert.ToDouble(roundX) / scale
                        roundX = Convert.ToInt32(tempNum).ToString
                        roundY = splittedLine(2).Split(".")(0)
                        tempNum = Convert.ToDouble(roundY) / scale
                        roundY = Convert.ToInt32(tempNum).ToString
                        roundZ = splittedLine(3).Split(".")(0)
                        tempNum = Convert.ToDouble(roundZ) / scale
                        roundZ = Convert.ToInt32(tempNum).ToString
                    Catch
                        Console.WriteLine("ERROR #5 - Error during matrix creation")
                        Return
                    End Try

                    finalString += "	" + "[" + indexNum.ToString + "] = {.x = " + roundX + ", .y = " + roundY + ", .z =  " + roundZ + "}," + vbCrLf
                    indexNum += 1
                End If
            Else
                If (currentState = 1) Then
                    Exit For
                End If
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
