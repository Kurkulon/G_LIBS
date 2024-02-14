#GCC filter script
#Converts GCC error messages into a format compatible with Visual Studio
#This file is provided "as is" under the BSD license

#To debug binaries produced by GCC with Visual Studio debugger, try VisualGDB:
# 						http://visualgdb.com/
#"cpp\main.cpp", line 310 (col. 8): cc0256:  error: invalid redeclaration of type name "ReqRcv01" (declared at line 42 of "d:\Work\G26X_SOFT\G26X.1\G26X.1.DSP\..\..\Include\G_RCV.h")
#STDOUT->autoflush(1);
#	if (/^\"([^ ]+)\", line ([0-9]+) \(col\. ([0-9]+): (.*)$/)


while(<STDIN>)
{
	if (/^\"([^ ]+)\", line ([0-9]+) \(col\. ([0-9]+)\):(.*)$/)
	{
		print "$1($2,$3) : $4\n";
	}
	else
	{
		print "$_";
	}
}