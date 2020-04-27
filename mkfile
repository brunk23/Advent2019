%:	%.c
	6c $stem.c
	6l -o $stem $stem.6

commit:
	git/commit *.[ch] *-input mkfile

addnew:
	git/add *.[ch] *-input
