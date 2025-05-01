#ifndef HLASKY_H
#define HLASKY_H

const short poleUzZ[27] = {6 /*a*/,0 /*b*/,0 /*c*/,0 /*d*/,5 /*e*/,1 /*f*/,\
	0 /*g*/,1 /*h*/,4 /*i*/,0 /*j*/,0 /*k*/,3 /*l*/,2 /*m*/,2 /*n*/,5 /*o*/,\
	0 /*p*/,0 /*q*/,3 /*r*/,1 /*s*/,0 /*t*/,4 /*u*/,1 /*v*/,1 /*w*/,0 /*x*/,\
	4 /*y*/,1 /*z*/,1 /*ch*/};
//pole udavajici uzavrenost jednotlivych hlasek na jejich zacatku

const short poleUzK[27] = {6 /*a*/,0 /*b*/,1 /*c*/,0 /*d*/,5 /*e*/,1 /*f*/,\
	0 /*g*/,1 /*h*/,4 /*i*/,0 /*j*/,0 /*k*/,3 /*l*/,2 /*m*/,2 /*n*/,5 /*o*/,\
	0 /*p*/,1 /*q*/,3 /*r*/,1 /*s*/,0 /*t*/,4 /*u*/,1 /*v*/,1 /*w*/,1 /*x*/,\
	4 /*y*/,1 /*z*/,1 /*ch*/};
//pole udavajici uzavrenost jednotlivych hlasek na jejich konci

#define uzavZ( hlaska ) poleUzZ[ (char)(hlaska) - 'a' ]
#define uzavK( hlaska ) poleUzK[ (char)(hlaska) - 'a' ]


#define HLASKA_A	(hlas_t) 'a'
#define HLASKA_B	(hlas_t) 'b'
#define HLASKA_C	(hlas_t) 'c'
#define HLASKA_D	(hlas_t) 'd'
#define HLASKA_E	(hlas_t) 'e'
#define HLASKA_F	(hlas_t) 'f'
#define HLASKA_G	(hlas_t) 'g'
#define HLASKA_H	(hlas_t) 'h'
#define HLASKA_CH	(hlas_t) '{'
#define HLASKA_I	(hlas_t) 'i'
#define HLASKA_J	(hlas_t) 'j'
#define HLASKA_K	(hlas_t) 'k'
#define HLASKA_L	(hlas_t) 'l'
#define HLASKA_M	(hlas_t) 'm'
#define HLASKA_N	(hlas_t) 'n'
#define HLASKA_O	(hlas_t) 'o'
#define HLASKA_P	(hlas_t) 'p'
#define HLASKA_Q	(hlas_t) 'q'
#define HLASKA_R	(hlas_t) 'r'
#define HLASKA_S	(hlas_t) 's'
#define HLASKA_T	(hlas_t) 't'
#define HLASKA_U	(hlas_t) 'u'
#define HLASKA_V	(hlas_t) 'v'
#define HLASKA_W	(hlas_t) 'w'
#define HLASKA_X	(hlas_t) 'x'
#define HLASKA_Y	(hlas_t) 'y'
#define HLASKA_Z	(hlas_t) 'z'

#endif

