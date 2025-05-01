/* Toto je interaktivni program na lamani radku na pomezi slabik.
 * Je psany pouze pro cestinu.
 *
 * Poznamky pro editaci:
 * 	kodovani UTF-8;
 * 	sirka tabelatoru, aby byl kod pekne citelny: 2.
 *
 * Matej Korvas, 2009
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HLASKY.H"

#define MAX_D_SYL 15  			 //maximalni delka slabiky ve znacich
#define REZERVA 41					//rezerva pro dlouhe nezalomitelne úseky (napr. posloupnosti tecek nebo pravych zavorek)
#define DLOUHYCH_RADKU 1000	//pocet radku, ktere se nepodarily zalamat, ktere si budeme pamatovat
#define DELKA_CISLA 12			//explicitne predpokladejme, ze kazde cislo pujde zapsat tolika znaky
#define DELKA_ZPRAVY 150		//max. delka textu chybove zpravy o nezalomenych radcich (na pocet znaku)
#define PripravNovouSlabiku	\
	{jeExploze = 1;						\
	konciSlovo = 0;					\
	delkaNove = 0;						\
	if( delkaSlabiky == 0 )		\
		radekNekonciHlaskou = 1;\
	else											\
		radekNekonciHlaskou = 0;}
#define ZdeLzeLamat															\
		{switch( nesekatZaOU )											\
		{																						\
			case 1:																		\
				if( !radekKonciJPPredlozkou( radek, delkaRadku, delkaOcasku ) )	\
					ZdeLzeOpravduLamat										\
				else																		\
					delkaSlabiky = 0;											\
				break;																	\
			case 0:																		\
				if( !radekKonciNeslabicne( radek, delkaRadku, delkaOcasku )	) \
					ZdeLzeOpravduLamat										\
				else																		\
					delkaSlabiky = 0;											\
		}																						\
		PripravNovouSlabiku}	//ZdeLzeLamat se vola, kde konci slabika nebo slovo.
#define ZdeLzeOpravduLamat																								\
		{zpracujRadek( radek, &delkaRadku, maxDelkaRadku,											\
									 &cisloRadku, dlouheRadky, &pDlouheRadky, &delkaOcasku,	\
									 slabika, &delkaSlabiky, nova,													\
									 delkaNove, konciSlovo, &posledniZlom,								\
									 &maVrchol );																						\
		}											//ZdeLzeOpravduLamat bere v uvahu i neslabicne predlozky, popr. predlozky "o" a "u".

typedef unsigned char pism_t;	//typ pro zachycovani psanych znaku (pismen)
typedef char hlas_t;					//typ pro zachycovani hlasek
typedef struct{
	pism_t* zneni;			//pole znaku v radku
	int delkaRadku;			//delka platne casti pole <zneni>
	int cisloRadku;			//cislo radku v ramci textu
	int delkaOcasku;		//delka suffixu, ktery lze odlomit na novy radek
	short posledniZlom;	//pozice posledniho znaku, za kterym lze radek zlomit
	} radek_t;	//typ pro praci s radkem
typedef struct{
	hlas_t* zneni;				//pole hlasek
	short delkaSlabiky;	//delka pole <zneni>
	_Bool maVrchol;			//zda slabika obsahuje slabicny vrchol
	_Bool jeExploze;		//zda jsme v explozni fazi slabiky
	} slabika_t;	//typ pro praci se slabikou
typedef struct{
	int maxDelkaRadku;	//maximalni zadana delka radku
	int* dlouheRadky;		//pole s cisly radku, ktere se nepodarilo zalamat
	int dDlouheRadky;		//delka pole <dlouheRadky>
	_Bool nesekatZaOU;	//zda se nemaji radky lamat ani po jednoznakych predlozkach
	} globalni_t;	//snuska "globalnich" promennych
_Bool jeZnakNovehoRadku( const pism_t znak )
{
	return (char)znak == '\n' || (char)znak == '\r';
}
_Bool jeNemezerovy( const pism_t znak )
{
	return (char)znak != ' ' && (char)znak != '\t' && (char)znak != '\v' && !jeZnakNovehoRadku(znak);
}
_Bool jeMezerovy( const pism_t znak )
{
	return (char)znak == ' ' || (char)znak == '\t' || (char)znak == '\v' || jeZnakNovehoRadku(znak);
}
/* Tato funkce odpovi, jestli dany znak (parametr hlaska) povazujeme
 * a umime zpracovat jako platnou hlasku.															*/
_Bool jeHlaska( const hlas_t hlaska )
{
	return hlaska == HLASKA_A || hlaska == HLASKA_B || hlaska == HLASKA_C || hlaska == HLASKA_D
				 || hlaska == HLASKA_E || hlaska == HLASKA_F || hlaska == HLASKA_G || hlaska == HLASKA_H
				 || hlaska == HLASKA_CH || hlaska == HLASKA_I || hlaska == HLASKA_J || hlaska == HLASKA_K
				 || hlaska == HLASKA_L || hlaska == HLASKA_M || hlaska == HLASKA_N || hlaska == HLASKA_O
				 || hlaska == HLASKA_P || hlaska == HLASKA_Q || hlaska == HLASKA_R || hlaska == HLASKA_S
				 || hlaska == HLASKA_T || hlaska == HLASKA_U || hlaska == HLASKA_V || hlaska == HLASKA_W
				 || hlaska == HLASKA_X || hlaska == HLASKA_Y || hlaska == HLASKA_Z;
}
/* Tato funkce pozna znak pro neslabicnou predlozku.	*/
_Bool jeNeslabicna( const pism_t predlozka )
{
	return (char)predlozka == 'k' || (char)predlozka == 's' || (char)predlozka == 'v' || (char)predlozka == 'z' ||
		(char)predlozka == 'K' || (char)predlozka == 'S' || (char)predlozka == 'V' || (char)predlozka == 'Z';
}
/* Tato funkce pozna znak pro jednopismennou predlozku. */
_Bool jeJPPredlozka( const pism_t predlozka )
{
	return (char)predlozka == 'o' || (char)predlozka == 'u' || (char)predlozka == 'O' || (char)predlozka == 'U' || 
		jeNeslabicna(predlozka);
}
/* Touto funkci se vypisuje zadany vystup programu.		*/
void vypis( const pism_t znak )
{
	putchar( (char)znak );
}
/* Touto funkci se vypisuje chybovy vystup programu.	*/
void vypisChybu( const char* zprava )
{
	fprintf( stderr, "CHYBA: %s\n", zprava );
}
/* Vraci retezcovou reprezentaci celeho cisla.				*/
char* intToStr( const int _int )
{
	char* ret = (char*) calloc( DELKA_CISLA, sizeof(char) );
	sprintf( ret, "%d", _int );
	return ret;	
}
/* Odpovi, zda lze takovyto znak odlomit od predchazejiciho retezce znaku
 * a umistit jej na novy radek.
 * Predpoklada, ze dany znak neudava hlasku, ale interpunkci.							*/
_Bool lzeOdlomit( const pism_t znak )
{
	return (char)znak != '.' && (char)znak != ',' && (char)znak != ';' && (char)znak != '-' && (char)znak != ')'
			&& (char)znak != ']' && (char)znak != '}' && (char)znak != '>' && (char)znak != '%' && (char)znak != '!'
			&& (char)znak != '?' && (char)znak != '"' && (char)znak != '\'' && (char)znak != '/' && (char)znak != '\\'
			&& (char)znak != '~';
}
/* Odpovi, zda radek obsahuje pred zacatkem ocasku neslabicnou predlozku. */
_Bool radekKonciNeslabicne( const radek_t* const _radek )
{
	pism_t* radek = (*_radek).zneni;
	int posledniNemezerovy = (*_radek).delkaRadku - 1 - (*_radek).delkaOcasku;
			//1 se odecita kvuli indexaci od 0

	//Najdeme posledni nemezerovy znak:
	while( posledniNemezerovy >= 0 && jeMezerovy(radek[ posledniNemezerovy ]) )
		posledniNemezerovy--;

	if( posledniNemezerovy < 0 )	//Tj. radek obsahuje same mezerove znaky.
		return 0;
	else if( posledniNemezerovy == 0 )	//Obsahuje-li radek pouze jeden znak:
		return jeNeslabicna( radek[posledniNemezerovy] );
	else
		return jeNeslabicna( radek[posledniNemezerovy] ) &&
							jeMezerovy( radek[posledniNemezerovy-1] );
}
/* Odpovi, zda radek obsahuje pred zacatkem ocasku jednopismennou predlozku.	*/
_Bool radekKonciJPPredlozkou( const radek_t* const _radek )
{
	pism_t* radek = (*_radek).zneni;
	int posledniNemezerovy = (*_radek).delkaRadku - 1 - (*_radek).delkaOcasku;
			//1 se odecita kvuli indexaci od 0

	//Najdeme posledni nemezerovy znak:
	while( posledniNemezerovy >= 0 &&
		 			jeMezerovy(radek[ posledniNemezerovy ]) )
		posledniNemezerovy--;
	if( posledniNemezerovy < 0 )	//Tj. radek obsahuje same mezerove znaky.
		return 0;
	else if( posledniNemezerovy == 0 )	//Obsahuje-li radek pouze jeden znak:
		return jeJPPredlozka( radek[posledniNemezerovy] );
	else
		return jeJPPredlozka( radek[posledniNemezerovy] ) &&
							jeMezerovy( radek[posledniNemezerovy-1] );
}
/* Urci, jaky maly znak anglicke abecedy odpovida ceskemu znaku <znak>.
 * Pokud zadny, vraci ' '.
 */
char naMalyAnglicky( const pism_t znak )
{
	switch ( (unsigned char) znak )
	{
		case 'a':
		case 'A':
//		case 'á':
		case 225:
//		case 'Á':
		case 193:
			return 'a';
		case 'b':
		case 'B':
			return 'b';
		case 'c':
		case 'C':
//		case 'č':
		case 232:
//		case 'Č':
		case 200:
			return 'c';
		case 'd':
		case 'D':
//		case 'ď':
		case 239:
//		case 'Ď':
		case 208:
			return 'd';
		case 'e':
		case 'E':
//		case 'é':
		case 233:
//		case 'É':
		case 201:
//		case 'ě':
		case 236:
//		case 'Ě':
		case 204:
			return 'e';
		case 'f':
		case 'F':
			return 'f';
		case 'g':
		case 'G':
			return 'g';
		case 'h':
		case 'H':
			return 'h';
		case 'i':
		case 'I':
//		case 'í':
		case 237:
//		case 'Í':
		case 205:
			return 'i';
		case 'j':
		case 'J':
			return 'j';
		case 'k':
		case 'K':
			return 'k';
		case 'l':
		case 'L':
			return 'l';
		case 'm':
		case 'M':
			return 'm';
		case 'n':
		case 'N':
//		case 'ň':
		case 242:
//		case 'Ň':
		case 210:
			return 'n';
		case 'o':
		case 'O':
//		case 'ó':
		case 243:
//		case 'Ó':
		case 211:
			return 'o';
		case 'p':
		case 'P':
			return 'p';
		case 'q':
		case 'Q':
			return 'q';
		case 'r':
		case 'R':
//		case 'ř':
		case 248:
//		case 'Ř':
		case 216:
			return 'r';
		case 's':
		case 'S':
//		case 'š':
		case 185:
//		case 'Š':
		case 169:
			return 's';
		case 't':
		case 'T':
//		case 'ť':
		case 187:
//		case 'Ť':
		case 171:
			return 't';
		case 'u':
		case 'U':
//		case 'ú':
		case 250:
//		case 'Ú':
		case 218:
//		case 'ů':
		case 249:
//		case 'Ů':
		case 217:
			return 'u';
		case 'v':
		case 'V':
			return 'v';
		case 'w':
		case 'W':
			return 'w';
		case 'x':
		case 'X':
			return 'x';
		case 'y':
		case 'Y':
//		case 'ý':
//		case 'Ý':
			return 'y';
		case 'z':
		case 'Z':
//		case 'ž':
		case 190:
//		case 'Ž':
		case 174:
			return 'z';
		default:
			return ' ';
	}
	return (char) znak;
}
/* Odpovi, zda je dany znak ceske pismeno (pomoci fce naMalyAnglicky). */
_Bool jePismeno( const pism_t znak )
{
	return jeHlaska( (hlas_t) naMalyAnglicky(znak) );
}
/* Odpovi, zda pri zalomeni radku pred danou pozici je treba na zlom vlozit pomlcku. 
 * Navratova hodnota = 1, pokud pomlcku vlozit treba, 0 pokud netreba.							*/
short pomlckaPred( const pism_t* radek, const int delkaRadku, const int pozice )
{
	return (pozice > 0) && (delkaRadku > pozice) &&
			jePismeno(radek[ pozice ]) && jePismeno(radek[ pozice - 1 ]) ? 1 : 0;
}
/* Oddeli zakonceni radku od daneho znaku a vezme je za novy radek.
 * <(*_radek).posledniZlom> udava index znaku, ktery se prvni ma ocitnout na novem radku. */
void novyRadek( radek_t* const _radek )
{
	pism_t* radek = (*_radek).zneni;
	int delkaRadku = (*_radek).delkaRadku;
	int delkaOcasku = (*_radek).delkaOcasku;
	int zacatekLomu = (*_radek).posledniZlom;
	short s;

	//Preskocime pripadne mezerove znaky na konci radku:
	for( s=zacatekLomu; jeMezerovy(radek[ s ]); s++ )
		;
	zacatekLomu = s;
	//Odlomek radku prekopirujeme napriste:
	while( s<delkaRadku )
	{
		radek[ s - zacatekLomu ] = radek[ s ];
		s++;
	}
	//Aktualizujeme parametry radku:
	delkaRadku -= zacatekLomu;
	(*_radek).delkaRadku = delkaRadku;
	(*_radek).posledniZlom = delkaRadku - delkaOcasku;
	(*_radek).cisloRadku++;
	return;
}
// Pripravi strukturu slabiky predanou v argumentu na novou slabiku.
void novaSlabika( slabika_t* const _slabika )
{
	(*_slabika).delkaSlabiky = 0;
	(*_slabika).maVrchol = 0;
	(*_slabika).jeExploze = 1;
	return;
}
/* Zaznamena, ze <_radek> lze zalomit pred znakem <*_delkaRadku - *_delkaOcasku>,
 * a pokud je tento index vetsi nez <maxDelkaRadku>, radek zalomi, kde nejdal to jde.
 * Prvni, zalomenou cast radku vypise funkci vypis() a druhou ulozi do struktury
 * <*_radek>. Tento postup opakuje, pokud je i u noveho radku index znaku, kde jej lze
 * zalomit, vetsi nez <maxDelkaRadku>.
 * Pokud se radek nepodari zalomit, aby byl dost kratky, ulozi se informace o nem do
 * pole <_pDlouheRadky> ve strukture <_globalni>.
 * Zaroven aktualizuje udaje v poli <_slabika> podle pole <_nova> (nadchazejici to
 * slabiky, u ktere vsak neni oznaceno, zda ma vrchol).
 */
void zdeLzeLamat( radek_t* _radek, slabika_t* _slabika, slabika_t* _nova, globalni_t* _globalni )
{
	//hodnoty z argumentu:
		//parametry radku:
	pism_t* const radek = (*_radek).zneni;
	int delkaRadku = (*_radek).delkaRadku;
	int cisloRadku = (*_radek).cisloRadku;
	int delkaOcasku = (*_radek).delkaOcasku;
	int posledniZlom = (*_radek).posledniZlom;
		//"globalni" promenne:
	const int maxDelkaRadku = (*_globalni).maxDelkaRadku;
	int* const dlouheRadky = (*_globalni).dlouheRadky;
	int* const dDlouheRadky = &((*_globalni).dDlouheRadky);
	const _Bool nesekatZaOU = (*_globalni).nesekatZaOU;
		//parametry slabik:
	hlas_t* const slabika = (*_slabika).zneni;
	hlas_t* const nova = (*_nova).zneni;
	_Bool maVrchol;		//Toto neinicializujeme hodnotou z argumentu, protoze se bude inicializovat 0.

	//odvozene hodnoty:
	const short pomlckaPredOcaskem = pomlckaPred( radek, delkaRadku, delkaRadku - delkaOcasku ) ? 1 : 0;
	const short pomlckaPredZlomem = pomlckaPred( radek, delkaRadku, posledniZlom ) ? 1 : 0;
	const int platnaDelka = delkaRadku - delkaOcasku + pomlckaPredOcaskem;
		//delka nejdelsiho znameho prefixu radku, za kterym muzeme radek zalomit
//	const _Bool jeHotovyRadek = (platnaDelka == delkaRadku) && (delkaRadku == maxDelkaRadku);
		//zda lze otisknout proste takovyto radek

	short s;

	//Nejdrive zjistime, jestli zde opravdu lze lamat; je treba osetrit, jestli radek zrovna nekonci
	//neslabicnou ci jednoznakou predlozkou.
	switch( nesekatZaOU )
	{																						
		case 1:																		
			if( radekKonciJPPredlozkou(_radek) )	//Pokud je sice konec slova, ale nechceme lamat,
			{
				novaSlabika( _slabika );
				return;
			}
			break;
		case 0:																		
			if( radekKonciNeslabicne(_radek) )		//Pokud je sice konec slova, ale nechceme lamat,
			{
				novaSlabika( _slabika );
				return;
			}
	}
	if( platnaDelka > maxDelkaRadku )	//Je-li radek uz moc dlouhy:
	{
		//Pokud jej navic ani neumime spravne zalomit:
		if( posledniZlom == 0 ||
				posledniZlom + pomlckaPredZlomem > maxDelkaRadku )
			//Zaneseme si jej do pole dlouhych radku.
			dlouheRadky[ (*dDlouheRadky)++ ] = cisloRadku;	

		//Nyni radek vypiseme, dokud jej neumime zlomit:
		for( s=0; s < posledniZlom; s++ )
				vypis( radek[s] );
		if( pomlckaPredZlomem )
			vypis( '-' );
		vypis( '\n' );
		//Zalozime na novy radek.
		novyRadek( _radek );
		//Pokud posledni zlom vyjde moc daleko, musime jeste i odlomek zalamat.
		if( (*_radek).posledniZlom >= maxDelkaRadku )
		{
			zdeLzeLamat( _radek, _slabika, _nova, _globalni );
			return;
		}
	}
	else	//Pokud radek jeste netreba zalamovat:
		//Posuneme posledni znamy zlom radku.
		(*_radek).posledniZlom = delkaRadku - delkaOcasku;

	//Zbyva jeste aktualizovat udaje o slabikach:
	maVrchol = 0;
	if( (*_nova).delkaSlabiky )	//je-li treba prekopirovat nacatou slabiku <nova>
	{
//		(*_radek).konciPismenem = 1;	//aktualizujeme promennou radku <konciPismenem>
		slabika[0] = nova[0];
		if( uzavZ( slabika[0] ) > 3 )	//kvuli neslabikotvornosti r, l, jsou-li na zacatku
			maVrchol = 1;
		if( (*_nova).delkaSlabiky > 1 )	//aneb if( delkaNove == 2 )
		{
			slabika[1] = nova[1];
			(*_slabika).delkaSlabiky = 2;
			if( (uzavZ( slabika[1] ) > 2) ||
					 ((slabika[1] == 'm')  &&  ((slabika[0] == 'd') || (slabika[0] == 's')))  )
																			//osetreni vyjimecnych slabik "dm" a "sm"
				maVrchol = 1;
		}
		else
			(*_slabika).delkaSlabiky = 1;
	}
	else
	{	
//		(*_radek).konciPismenem = 0;	//aktualizujeme promennou radku <konciPismenem>
		(*_slabika).delkaSlabiky = 0;
	}

	(*_slabika).maVrchol = maVrchol;
	(*_slabika).jeExploze = 1;						
	(*_nova).delkaSlabiky = 0;

	return;
}
// Odpovi, zda posledni znak na radku je pismeno.
_Bool konciPismenem( const radek_t* const _radek )
{
	pism_t znak;
	if( (*_radek).delkaRadku > 0 )
	{
		znak = (*_radek).zneni[ (*_radek).delkaRadku - 1 ];
		return jePismeno(znak);
	}
	return 0;
}
/* Rozseka obsah textoveho souboru na slabiky.
 *
 *	 fstup: odkaz na vstupni soubor
 *	 maxDelkaRadku: pozadovana delka radku po zalamani
 *	 slucovatMezery: zda se maji posloupnosti mezerovych znaku
 *	 								v ramci radku nahradit jedinym
 *	 nesekatZaOU: zda se nepovoluje ukoncit radek jednopismennou predlozkou
 *
 * Funkce misto puvodnich ruznych mezerovych znaku dosazuje vzdy mezeru (' ').
 *
 * Funkce neumi rozpoznat, co znaci konce radku. Dva po sobe nasledujici znaky
 * z mnoziny [\n\r] povazuje za jedno odradkovani. Pokud je radek znacen jednim
 * timto znakem, bere jej take za jedno odradkovani.
 */
void rozsekejText( FILE* fstup, const int maxDelkaRadku, const _Bool slucovatMezery,
	 const _Bool nesekatZaOU	)
{
//DEKLARACE PROMĚNNYCH
slabika_t slabika, nova;
//hlas_t	slabika[MAX_D_SYL];
//					//"hlasky" spadajici do aktualni slabiky
//					//slouzi k rozhodovani, zda konci slabika
//short	delkaSlabiky = 0;			//delka platne casti pole <slabika>
//hlas_t	nova[2];							//"hlasky" spadajici do pristi slabiky
//short delkaNove = 0;				//delka platne casti pole <nova>
//int		delkaOcasku = 0;			//delka konce radku, ktery nelze prelomit

short	s;
int		pomInt;
pism_t	aktZnak, dalsiZnak = ' ';	//znaky, cili pisemne symboly
hlas_t	hlaska, pomHlaska;				//"hlaska" -- nezachovava se "diakritika"

radek_t radek;
//pism_t	radek[ maxDelkaRadku + MAX_D_SYL + REZERVA ];
//					//aktualni radek
//					//slouzi k uchovavani prectenych znaku				

//int		delkaRadku = 0;		
//				//kolik je na akt. radku znaku (podle ascii; ch = 2)

//int		posledniZlom = 0,	//nejzazsi misto v akt. radku, kde jej lze zalomit
//			cisloRadku = 0; 	//poradi vypisovaneho radku

globalni_t globalni;					//struktura s "globalnimi" promennymi
// int		dlouheRadky[ 1000 ],	
				//pole s poradimi radku, ktere se nepovedlo zalomit
//			pDlouheRadky = 0;			
				//delka tohoto pole

//_Bool	konciSlovo	= 0,	//zda aktualni slabikou konci slovo
//														//nastaveno na 0, aby se nemohlo zacyklit
//			radekNekonciHlaskou = 0,	//zda lze zde zacit novy radek
//														//nastaveno na 0, aby se nemohlo zacyklit
//			jeExploze = 1,			//zda jsme v explozni fazi slabiky
//			maVrchol = 0;				//zda jsme v <slabika> zaznamenali slabicny vrchol
short uzAkt,							//uzavrenost aktualni hlasky zleva (tj. na zacatku)
			uzMin;							//uzavrenost minule hlasky zprava (tj. na konci)

const int end_of_file = EOF;

_Bool bylKonecRadku = 0;	//zda byl predchozi nacteny znak znak konce radku
char	*pomRet = (pism_t*)calloc( DELKA_CISLA + DELKA_ZPRAVY, sizeof(pism_t) );

//	inicializace pouzivanych struktur typu <radek_t> a <slabika_t>
slabika.zneni = (hlas_t*)calloc( MAX_D_SYL, sizeof(hlas_t) );
novaSlabika( &slabika );
nova.zneni = (hlas_t*)calloc( 2, sizeof(hlas_t) );	// Na novou slabiku vzdy spotrebujeme nejvyse dva znaky.
novaSlabika( &nova );
radek.zneni = (pism_t*)calloc( maxDelkaRadku + MAX_D_SYL + REZERVA, sizeof(pism_t) );
radek.delkaRadku = 0;
radek.cisloRadku = 0;
radek.delkaOcasku = 0;
radek.posledniZlom = 0;
//	inicializace struktury globalnich promennych
globalni.maxDelkaRadku = maxDelkaRadku;															//maximalni zadana delka radku
globalni.dlouheRadky = (int*)calloc( DLOUHYCH_RADKU, sizeof(int) );	//pole na zaznamy o nezalomenych radcich
globalni.dDlouheRadky = 0;																					//delka tohoto pole
globalni.nesekatZaOU = nesekatZaOU;																	//zda se radky nesmi koncit jednopismennou predlozkou

	//--- hlavni cyklus - nacitani vstupu ------------------
	while( (pomInt = fgetc(fstup)) != EOF )
	{
		//Nacteny znak ulozime jednak s pripadnou diakritikou (jako <aktZnak>)
		aktZnak = (pism_t) pomInt;
		//a druhak bez ni (jako <hlaska>).
		hlaska = (hlas_t) naMalyAnglicky( aktZnak );
		//Osetrime dvojznak 'ch':
		if( (aktZnak == 'c') || (aktZnak == 'C') )
		{
			if( (pomInt = fgetc(fstup)) != EOF )
			{
				dalsiZnak = (pism_t) pomInt;
				if( (dalsiZnak == 'h') || (dalsiZnak == 'H') )
					hlaska = HLASKA_CH;
				else
				{
					ungetc( (char)dalsiZnak, fstup );
					dalsiZnak = ' ';
				}
			}
		}

		//--- zacina-li novy shluk pismen ----------------------
		//    (aneb je-li dosavadni nemezerovy vstup zpracovan)
		if( !konciPismenem(&radek) )
		{
			if( jeHlaska(hlaska) )	//je-li dalsi nacteny znak pismeno
															//(Kontroluje se hlaska, aby nenastalo nedorozumeni kvuli "ch".)
			{
				bylKonecRadku = 0;
				zdeLzeLamat( &radek, &slabika, &nova, &globalni );

//				radek.nekonciHlaskou = 0;
				radek.zneni[ (radek.delkaRadku)++ ] = aktZnak;
				radek.delkaOcasku = 1;
				if( dalsiZnak != ' ' )
				{
					radek.zneni[ (radek.delkaRadku)++ ] = dalsiZnak;
					radek.delkaOcasku = 2;
					dalsiZnak = ' ';
				}
				slabika.zneni[ (slabika.delkaSlabiky)++ ] = hlaska;
				// Kvuli neslabikotvornosti r, l, jsou-li na zacatku,
				// porovnavame s 3 (a ne s 2).
				if( uzavZ( hlaska ) > 3 )			
					//A pripadne si poznamename, ze slabika ma vrchol.
					slabika.maVrchol = 1;
			}
			else	//if( !jeHlaska(hlaska) )
			{
				//Nejdrive osetrime, jestli jsme nenarazili na pulku znaku pro zacatek radku.
				if( jeMezerovy(aktZnak) )
				{
					if( jeZnakNovehoRadku(aktZnak) )
					{
						if( !bylKonecRadku )
							bylKonecRadku = 1;
							//Pokud jsme nacetli prvni znak pro konec radku,
							//zapamatujeme si to do promenne <bylKonecRadku>
							//a pokracujeme, jako by to byvala byla mezera.
							//(Pokud nacteme dalsi, spadneme opet do teto vetve (delkaSlabiky == 0) &&
							//&& ( !jeHlaska(hlaska) ) && ( jeMezerovy(aktZnak) ) a takovy
							//pripad osetrime v alternativni vetvi k tomuto "if".
						else	//if( bylKonecRadku )
						{
							//Druhy znak pro konec radku ignorujeme.
							bylKonecRadku = 0;
							continue;
						}
					}
					else
						bylKonecRadku = 0;

					zdeLzeLamat( &radek, &slabika, &nova, &globalni );

					//Nacetli-li jsme tedy mezerovy znak:
					if( radek.delkaRadku != 0 )	//Na zacatek radku mezeru davat nebudeme.
					{
							//Pokud mezery neslucujeme, proste ji pridame.
						if( !slucovatMezery && !jeZnakNovehoRadku(aktZnak) )
						{
							radek.zneni[ (radek.delkaRadku)++ ] = ' ';
							radek.delkaOcasku = 1;
						}
							//Jinak se jeste podivame, jestli predchozi byl nemezerovy:
						else if( jeNemezerovy(radek.zneni[ radek.delkaRadku-1 ]) )
						{
							//nechame v radku na jeho miste mezeru:
							radek.zneni[ (radek.delkaRadku)++ ] = ' ';
							radek.delkaOcasku = 1;
						}
					}
				}
				else	//if( !jeMezerovy(aktZnak) )
				{
					if( lzeOdlomit(aktZnak) )
					{
						zdeLzeLamat( &radek, &slabika, &nova, &globalni );
						radek.zneni[ (radek.delkaRadku)++ ] = aktZnak;
						radek.delkaOcasku = 1;
					}
					else
					{
						radek.zneni[ (radek.delkaRadku)++ ] = aktZnak;
						(radek.delkaOcasku)++;
					}
				}
			}
		}

		//--- pokud jiz neprazdny shluk pismen mame ------------
		else	//"if( konciPismenem(radek) )"
		{
			bylKonecRadku = 0;
			if( !jeHlaska(hlaska) )
			{
//				radek.nekonciHlaskou = 1;
//				radek.konciSlovo = 1;
				if( lzeOdlomit(aktZnak) )	//Lze-li takovyto znak odlomit na novy radek
				{
					if( jeMezerovy(aktZnak) )
					{
						if( jeZnakNovehoRadku(aktZnak) )
							//assert !bylKonecRadku
							bylKonecRadku = 1;
							//Pokud jsme nacetli prvni znak pro konec radku,
							//pouze si to zapamatujeme do promenne <bylKonecRadku>.

						zdeLzeLamat( &radek, &slabika, &nova, &globalni );

						if( radek.delkaRadku > 0 )
						{
							radek.zneni[ (radek.delkaRadku)++ ] = ' ';
							radek.delkaOcasku = 1;
						}
					}
					else
					{
						zdeLzeLamat( &radek, &slabika, &nova, &globalni );

						radek.zneni[ (radek.delkaRadku)++ ] = aktZnak;
						radek.delkaOcasku = 1;
					}
					continue;
				}
				else	//(if !lzeOdlomit(aktZnak) ); assert jeNemezerovy(aktZnak)
				{
						//Pokud tento znak nelze odlomit, nechame jej prilepeny k dosavadnimu
						//radku a cekame na nejaky znak, kde pujde radek zlomit.
					radek.zneni[ (radek.delkaRadku)++ ] = aktZnak;
					(radek.delkaOcasku)++;
					continue;
				}
			}

			//if( jeHlaska(hlaska) )
				//Nacetli-li jsme hlasku, pustime se do rozhodovani, zda je zde prelom slabiky:

			//--- nastaveni pomocnych promennych ---
			uzAkt = uzavZ( hlaska );									//uzavrenost nactene "hlasky"
			pomHlaska = slabika.zneni[ slabika.delkaSlabiky - 1 ];	//predchozi "hlaska"
			uzMin = uzavK( pomHlaska );								//uzavrenost predchozi "hlasky"
			//--- konec nastavovani promennych -----
			
			radek.zneni[ (radek.delkaRadku)++ ] = aktZnak;
			if( dalsiZnak != ' ' )
			{
				radek.zneni[ (radek.delkaRadku)++ ] = dalsiZnak;
				dalsiZnak = ' ';
			}

			if( uzAkt > 2 )
				slabika.maVrchol = 1;

			//--- jsme-li v prvni pulce slabiky (pred vrcholem) ---
			if( slabika.jeExploze )
			{
				if( uzAkt == uzMin )
				{
					if( uzAkt > 2 )	
						/* Jsou-li vedle sebe dve samohlasky stejne uzavrenosti
						 * nebo r a l:                                          */
					{
						nova.zneni[0] = hlaska;
						nova.delkaSlabiky = 1;			//narazili jsme na predel slabik
					}
					else if( (slabika.maVrchol) && (uzAkt == 2) && (hlaska != pomHlaska) )	
						//Mame vedle sebe m a n:
					{
						nova.zneni[0] = hlaska;
						nova.delkaSlabiky = 1;			//narazili jsme na predel slabik
					}
					else
						slabika.zneni[ (slabika.delkaSlabiky)++ ] = hlaska;
				}
				else	//( uzAkt != uzMin )
				{
					if( uzAkt < uzMin )
						//Zacina serie implozi, nebo se jedna o nejakou zvrhlost:
					{
						slabika.zneni[ (slabika.delkaSlabiky)++ ] = hlaska;
						if( slabika.maVrchol )
							slabika.jeExploze = 0;
					}
					else	//( uzAkt > uzMin )
					{
						if( uzMin > 3 )	//Pozorujeme-li stoupavou dvouhlasku:
						{
							nova.zneni[0] = hlaska;
							nova.delkaSlabiky = 1;				//narazili jsme na predel slabik
						}
						else	//obycejne pokracovani explozi
							slabika.zneni[ (slabika.delkaSlabiky)++ ] = hlaska;
					}
				}
			}
			//--- konec "jsme-li v prvni pulce slabiky" -----------

			//--- jsme-li v druhe pulce slabiky (za vrcholem) -----
			else
			{
				if( uzAkt > uzMin )	//Na predchozim znaku probehla exploze:
				{
					if( uzAkt == 1 )
						/* zlotrilost pobocne slabiky v podobe frikativy (napr.
						 * (lid-)Stvo, (ob-)ZVlasť                              */
					{
						if( (pomInt = fgetc( fstup )) != 0 )
						{
							dalsiZnak = (pism_t) pomInt;
							pomHlaska = naMalyAnglicky( dalsiZnak );
  						if( uzavK( pomHlaska ) < 2 )
								//pokud opravdu nasleduje exploziva
							{
								nova.zneni[0] = hlaska;
								nova.zneni[1] = pomHlaska;
								nova.delkaSlabiky = 2;
							}
							else
								/* standardni prubeh - na predchozim znaku nastala
								 * exploze                                          */
							{
								ungetc( dalsiZnak, fstup );
								nova.zneni[0] = slabika.zneni[ --(slabika.delkaSlabiky) ];
								nova.zneni[1] = hlaska;
								nova.delkaSlabiky = 2;
							}
							dalsiZnak = ' ';
						}
						else	
							// standardni prubeh - na predchozim znaku nastala exploze
						{
							nova.zneni[0] = slabika.zneni[ --(slabika.delkaSlabiky) ];
							nova.zneni[1] = hlaska;
							nova.delkaSlabiky = 2;
						}
					}
					//konec osetreni zlotrilosti
					else	//(  if( uzAkt > uzMin && uzAkt != 1 )  )
					{
						//osetreni stoupave dvouhlasky:
						if( uzMin > 3 )
						{
							nova.zneni[0] = slabika.zneni[ --(slabika.delkaSlabiky) ];
							nova.delkaSlabiky = 1;
							ungetc( hlaska, fstup );
						}
						//bezny prubeh - na predchozim znaku nastala exploze
						else
						{
							nova.zneni[0] = slabika.zneni[ --(slabika.delkaSlabiky) ];
							nova.zneni[1] = hlaska;
							nova.delkaSlabiky = 2;
						}
					}
				}
				else	//(  if( uzAkt <= uzMin )  )
					slabika.zneni[ (slabika.delkaSlabiky)++ ] = hlaska;
			}
			if( nova.delkaSlabiky == 0 )
			{
				(radek.delkaOcasku)++;
				if( slabika.zneni[ slabika.delkaSlabiky - 1 ] == HLASKA_CH )
					(radek.delkaOcasku)++;
			}
			else
			{
				radek.delkaOcasku = nova.delkaSlabiky;
				for( s=0; s<nova.delkaSlabiky; s++ )
				{
					if( nova.zneni[s] == HLASKA_CH )
						(radek.delkaOcasku)++;
				}
			}
				//Do <radek.delkaOcasku> si zapamatujeme delku posledniho nedelitelneho
				//shluku pismen na radku.
			//--- konec: "jsme-li v druhe pulce slabiky" ----------
			if( nova.delkaSlabiky > 0 )	//Tj. narazili-li jsme na konec slabiky.
				zdeLzeLamat( &radek, &slabika, &nova, &globalni );	//Nechame vypsat aktualni radek a pripravime promenne na novy.
		}
	//--- konec rozhodovani o konci slabiky podle nacteneho znaku ---
	}
	//---------- konec hlavniho cyklu - nacitani vstupu ------------

	//Mozna nam prisel konec vstupu bezprostredne po konci nektere slabiky:
	if( slabika.delkaSlabiky > 0 )	//Nejprve prilepime posledni slabiku k radku
	{
//		radek.konciSlovo = 1;
		zdeLzeLamat( &radek, &slabika, &nova, &globalni );						//a pripadne jej vypiseme;
	}

	if( radek.delkaRadku > 0 )
	{
//		radek.konciSlovo = 1;
		zdeLzeLamat( &radek, &slabika, &nova, &globalni );
	}
	if( radek.delkaRadku > 0 )		//a pokud jeste neco zbyva,
	{
		for( s=0; s<radek.delkaRadku; s++ )
			vypis( radek.zneni[s] );
		vypis( '\n' );
	}

	for( pomInt = 0; pomInt < globalni.dDlouheRadky; pomInt++ )
	{
		pomRet[0] = '\0';
		strcat( pomRet, "Radek cislo " );
		strcat( pomRet, intToStr( globalni.dlouheRadky[ pomInt ] ) );
		strcat( pomRet, " se nepodarilo zalomit a je prilis dlouhy.");
		vypisChybu( pomRet );
	}
}

int main()
{
	const int maxDelkaJmena = 1000;
	char jmeno[ maxDelkaJmena ];
	int maxDelkaRadku = 60;								//pozadovana maximalni delka radku
	FILE* fstup;

	printf( "\nZadej cestu k vstupnimu souboru s textem.\n>" );
	scanf( "%s", jmeno );
	while( (fstup = fopen( jmeno, "r" )) == NULL )	//opakovane pokusy otevrit soubor, ktery se otevrit nedari
	{
    printf( "Vstupni soubor pod touto cestou se nepodarilo otevrit.\n\n" );
    printf( "Zadej cestu k vstupnimu souboru.\n>" );
		scanf( "%s", jmeno );
  }
	printf( "Zadej pozadovanou delku radku.\n>" );
	scanf( "%d", &maxDelkaRadku );
	rozsekejText( fstup, maxDelkaRadku, 1, 1 );		//veskera prace se odvede zde

	fclose(fstup);
	return 0;
}

/*
 * ZBYVA:
 *   - dodelat zmeny souvisejici se zavedenim struktur "radek" a "slabika"
 *   - opravit praci se znaky vs. s hlaskami (viz FIXME);
 *   - posvitit si na delkuOcasku;
 *   - predelat vsechny indexy do radku na stejny typ.
 */
 
