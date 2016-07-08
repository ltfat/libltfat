#include "ltfat.h"
#include "ltfat/types.h"
#include "ltfat/macros.h"

#define FIRWIN_RESETCOUNTER do{ \
if (ii == domod.quot + domod.rem) \
                posInt = startInt; \
            }while(0)

LTFAT_EXTERN int
LTFAT_NAME(firwin)(LTFAT_FIRWIN win, int gl, LTFAT_TYPE* g)
{
    int status = LTFATERR_SUCCESS;
    CHECK(LTFATERR_NOTPOSARG, gl > 0, "gl must be positive");

    double step = 1.0 / gl;
    // for gl even
    double startInt = -0.5;
    const div_t domod = div(gl, 2);

    if (domod.rem)
        startInt = -0.5 + step / 2.0;

    double posInt = 0;

    switch (win)
    {
    case LTFAT_HANN:
    case LTFAT_HANNING:
    case LTFAT_NUTTALL10:
    {
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.5 + 0.5 * cos(2.0 * M_PI * posInt);
            posInt += step;
        }
        break;
    }

    case LTFAT_SQRTHANN:
    case LTFAT_COSINE:
    case LTFAT_SINE:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = sqrt(0.5 + 0.5 * cos(2.0 * M_PI * posInt));
            posInt += step;
        }
        break;

    case LTFAT_HAMMING:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.54 + 0.46 * cos(2.0 * M_PI * posInt);
            posInt += step;
        }
        break;

    case LTFAT_NUTTALL01:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.53836 + 0.46164 * cos(2 * M_PI * posInt);
            posInt += step;
        }
        break;

    case LTFAT_SQUARE:
    case LTFAT_RECT:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = fabs(posInt) < 0.5 ? 1.0 : 0.0;
            posInt += step;
        }
        break;

    case LTFAT_TRIA:
    case LTFAT_TRIANGULAR:
    case LTFAT_BARTLETT:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 1.0 - 2.0 * fabs(posInt);
            posInt += step;
        }
        break;

    case LTFAT_SQRTTRIA:
        LTFAT_NAME(firwin)(LTFAT_TRIA, gl, g);
        for (int ii = 0; ii < gl; ii++)
            g[ii] = sqrt(g[ii]);

        break;

    case LTFAT_BLACKMAN:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.42 + 0.5 * cos(2 * M_PI * posInt) + 0.08 * cos(4 * M_PI * posInt);
            posInt += step;
        }
        break;

    case LTFAT_BLACKMAN2:
    {
        double denomfac = 1.0 / 18608.0;
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 7938.0 + 9240.0 * cos(2.0 * M_PI * posInt) +
                    1430.0 * cos( 4.0 * M_PI * posInt);
            g[ii] *= denomfac;
            posInt += step;
        }
        break;
    }
    case LTFAT_NUTTALL:
    case LTFAT_NUTTALL12:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.355768 + 0.487396 * cos(2.0 * M_PI * posInt) +
                    0.144232 * cos(4.0 * M_PI * posInt) + 0.012604 * cos(6.0 * M_PI * posInt);
            posInt += step;
        }
        break;

    case LTFAT_OGG:
    case LTFAT_ITERSINE:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            double innercos = cos(M_PI * posInt);
            g[ii] = sin(M_PI / 2.0 * innercos * innercos);
            posInt += step;
        }
        break;

    case LTFAT_NUTTALL20:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 3.0 + 4.0 * cos(2.0 * M_PI * posInt) + cos(4.0 * M_PI * posInt);
            g[ii] /= 8.0;
            posInt += step;
        }
        break;

    case LTFAT_NUTTALL11:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.40897 + 0.5 * cos(2.0 * M_PI * posInt) +
                    0.09103 * cos( 4.0 * M_PI * posInt);
            posInt += step;
        }
        break;

    case LTFAT_NUTTALL02:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.4243801 + 0.4973406 * cos(2.0 * M_PI * posInt) +
                    0.0782793 * cos( 4.0 * M_PI * posInt);
            posInt += step;
        }
        break;

    case LTFAT_NUTTALL30:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 10.0 + 15.0 * cos(2.0 * M_PI * posInt) +
                    6.0 * cos( 4.0 * M_PI * posInt) + cos(6.0 * M_PI * posInt);
            g[ii] /= 32.0;
            posInt += step;
        }
        break;

    case LTFAT_NUTTALL21:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.338946 + 0.481973 * cos(2.0 * M_PI * posInt) +
                    0.161054 * cos(4.0 * M_PI * posInt) + 0.018027 * cos(6.0 * M_PI * posInt);
            posInt += step;
        }
        break;

    case LTFAT_NUTTALL03:
        for (int ii = 0; ii < gl; ii++)
        {
            FIRWIN_RESETCOUNTER;
            g[ii] = 0.3635819 + 0.4891775 * cos(2.0 * M_PI * posInt) +
                    0.1365995 * cos( 4.0 * M_PI * posInt) + 0.0106411 * cos(6.0 * M_PI * posInt);
            posInt += step;
        }
        break;

    default:
        CHECKCANTHAPPEN("Unknown window");
    };

    // Fix symmetry of windows which are not zero at -0.5
    if (!domod.rem)
        g[domod.quot + domod.rem] = 0.0;

error:
    return status;
}
#undef FIRWIN_RESETCOUNTER
