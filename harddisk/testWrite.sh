# the command is cited from [here]()http://stackoverflow.com/questions/14141008/grep-offset-of-ascii-string-from-binary-file)

res=$(strings -a -t x harddisk/harddisk.img | grep 'I am')
echo $res
# and the result is large than size of kernel, in which should have on string
