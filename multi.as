        lw      0       2       mcand   load reg2 with mcand
        lw      0       3       mplier  load reg3 with mplier
        lw      0       4       mask	load reg4 with mask
        lw 		0		5		neg1	load reg5 with neg1
        lw		0		6		count	load reg6 with count
		nor		3		3		3		nor mplier by itself
start   beq     0       6       done    goto end of program when reg6 == 0
		nor		4		4		7		nor mask into reg7
		nor		7		3		7		nor mask with nord mplier into reg7 
		beq 	0       7       move	goto end of loop
		add     1		2		1		add contents of mcand to sum reg1
move    add     6       5       6       decrement reg6
		add     2		2		2		increment mcand by itself
		add  	4		4		4 		incrememnt mask by itself
        beq     0       0       start   go back to the beginning of the loop
done    halt                            end of program
mcand   .fill   32766
mplier  .fill   10383
mask    .fill   1
neg1    .fill   -1
count 	.fill	15
