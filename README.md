# dpp
Command line tool for searching for and outputting notes in code that are for production.

#Usage

simply say `dpp C:\srcDirectory C:\output.txt`

It will search for all strings fitting the pattern 'NOTE(name) blah blah comment', without the single quotes.
Then it will simply print out which line they're on and which file they're in.

I expand the tool as time goes on for more information and other language support etc. It will only look in .cpp and .h files.
