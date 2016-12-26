struct square_in {		/* input (argument) */
	long	argl;
};

struct square_out {		/* output (argument) */
	long	resl;
};

program SQUARE_PROG {
	version SQUARE_VERS {
		square_out SQUAREPROC(square_in) = 1; /* procedure
						       * numer = 1 */
	} = 1;			/* version number */
        version SQUARE_VERS2 {
               square_out SQUAREPROC(square_in) = 1;
        } = 2;
        /* 
         * version SQUARE_VERS3 {
         *         square_out SQUAREPROC(square_in) = 1;
         * } = 3;
         */
} = 0x31230000;			/* program number */

