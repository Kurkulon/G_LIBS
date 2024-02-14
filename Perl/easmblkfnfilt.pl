#GCC filter script
#Converts GCC error messages into a format compatible with Visual Studio
#This file is provided "as is" under the BSD license

#To debug binaries produced by GCC with Visual Studio debugger, try VisualGDB:
# 						http://visualgdb.com/
#[Error ea5004] "d:\Work\G26X_SOFT\G26X.1\G26X.1.DSP\..\..\..\G_LIBS\cpp\ADSP\startup_BF592A.s":65 Syntax Error in :
#STDOUT->autoflush(1);
#	if (/^\"([^ ]+)\", line ([0-9]+) \(col\. ([0-9]+): (.*)$/)


while(<STDIN>)
{
	if (/^(\[.*\]) \"([^ ]+)\":([0-9]+) (.*)$/)
	{
		print "$2($3,1) : $1 $4\n";
	}
	else
	{
		print "$_";
	}
}