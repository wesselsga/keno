Set args = WScript.Arguments

If (Wscript.Arguments.Count > 1) Then
	infile = args(0)
	outdir = args(1)
Else
	infile = ""
	outfile = ""
End if

WScript.Echo ( "unzipping " & infile)

Set oshell = CreateObject("shell.application")
Set osrc = oshell.NameSpace(infile).Items()
Set otarget = oshell.NameSpace(outdir)
opt = 256
otarget.CopyHere osrc, opt
