jerk
====

jerk runs two commands and connects their stdin and stdout in a cycle.

	Usage:
		jerk <command1> [arg1 ...] ; <command2> [arg1 ...]

Make sure to escape the semicolon from the shell.

Building
--------

There is a really simple Makefile.

	make

This should produce the `jerk` binary. You can copy this to some folder in your
PATH if you want to use it on a regular basis (I'm not sure why).

	cp jerk /usr/local/bin # For example

Examples
--------

	jerk cat jerk.c - \; cat - # cat a file back and forth

	jerk cat jerk.c - \; tee big_file.c # write all the looped io to disk

	jerk sed 's/\t/    /g' jerk.c \; sed 's/    /\t/g' # tabs vs. spaces

License
-------

I licensed this under GPLv3 or later. See COPYING for details.
