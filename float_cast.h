#ifndef FLOAT_CAST_H
#define FLOAT_CAST_H

/*	The presence of the required functions are detected during the configure
**	process and the values HAVE_LRINT and HAVE_LRINTF are set accordingly in
**	the config.h file.
*/

#if (HAVE_LRINT && HAVE_LRINTF)

    /*	These defines enable functionality introduced with the 1999 ISO C
    **	standard. They must be defined before the inclusion of math.h to
    **	engage them. If optimisation is enabled, these functions will be
    **	inlined. With optimisation switched off, you have to link in the
    **	maths library using -lm.
    */

    #define	_ISOC9X_SOURCE 1
    #define  _ISOC99_SOURCE 1

    #define	__USE_ISOC9X   1
    #define	__USE_ISOC99   1

    #include	<math.h>

#elif (defined (WIN32) || defined (_WIN32))

    /*	Win32 doesn't seem to have these functions.
    **	Therefore implement inline versions of these functions here.
    */

    __inline long int
    lrint (double flt)
    {	int intgr;

        _asm
        {	fld flt
            fistp intgr
            } ;

        return intgr ;
    }

    __inline long int
    lrintf (float flt)
    {	int intgr;

        _asm
        {	fld flt
            fistp intgr
            } ;

        return intgr ;
    }

    __inline long long int
    llrint (double flt)
    {	long long int intgr;

        _asm
        {	fld flt
            fistp intgr
            } ;

        return intgr ;
    }

    __inline long long int
    llrintf (float flt)
    {	long long int intgr;

        _asm
        {	fld flt
            fistp intgr
            } ;

        return intgr ;
    }

#else

   /* dmazzoni: modified these to do a proper rounding, even though
    * it's slower.  Correctness and consistency is more important
    * than speed, especially since lrint/lrintf are certainly not
    * available everywhere.
    *
    * MM: Now uses internal math.h rint() function
    */

   #include	<math.h>

   #define	lrint(dbl)		((int)rint(dbl))
    #define	lrintf(flt)		((int)rint(flt))

#endif

#endif // FLOAT_CAST_H
