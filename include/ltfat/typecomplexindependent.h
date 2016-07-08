#include "dgt_long.h"
#include "idgt_long.h"
#include "dgt_fb.h"
#include "idgt_fb.h"
#include "wavelets.h"
#include "goertzel.h"
#include "ciutils.h"
#include "gabdual_painless.h"
#include "ci_windows.h"

/*   Walnut factorization    */

typedef struct LTFAT_NAME(wfac_plan) LTFAT_NAME(wfac_plan);

LTFAT_EXTERN int
LTFAT_NAME(wfac)(const LTFAT_TYPE *g, const ltfatInt L, const ltfatInt R,
                 const ltfatInt a, const ltfatInt M, LTFAT_COMPLEX *gf);

LTFAT_EXTERN int
LTFAT_NAME(wfac_init)(const ltfatInt L, const ltfatInt a, const ltfatInt M,
                      unsigned flags, LTFAT_NAME(wfac_plan)** pout);

LTFAT_EXTERN int
LTFAT_NAME(wfac_execute)(LTFAT_NAME(wfac_plan)* plan, const LTFAT_TYPE *g,
                         const ltfatInt R, LTFAT_COMPLEX *gf);

LTFAT_EXTERN int
LTFAT_NAME(wfac_done)(LTFAT_NAME(wfac_plan)** pout);

/*  Inverse Walnut factorization  */

typedef struct LTFAT_NAME(iwfac_plan) LTFAT_NAME(iwfac_plan);

LTFAT_EXTERN int
LTFAT_NAME(iwfac)(const LTFAT_COMPLEX *gf, const ltfatInt L, const ltfatInt R,
                  const ltfatInt a, const ltfatInt M, LTFAT_TYPE *g);

LTFAT_EXTERN int
LTFAT_NAME(iwfac_init)(const ltfatInt L, const ltfatInt a, const ltfatInt M,
                       unsigned flags, LTFAT_NAME(iwfac_plan)** pout);

LTFAT_EXTERN int
LTFAT_NAME(iwfac_execute)(LTFAT_NAME(iwfac_plan)* plan, const LTFAT_COMPLEX* gf,
                          const ltfatInt R, LTFAT_TYPE* g);

LTFAT_EXTERN int
LTFAT_NAME(iwfac_done)(LTFAT_NAME(iwfac_plan)** pout);


LTFAT_EXTERN void
LTFAT_NAME(col2diag)(const LTFAT_TYPE *cin, const ltfatInt L,
                     LTFAT_TYPE *cout);

/*  Dual and tight  */

/** \addtogroup gabdual
 * @{
 */


/** Compute canonical dual window for Gabor system
 *
 * The Gabor system must be a frame i.e. M>=a.
 *
 * \param[in]   g    Original window(s), size L x R
 * \param[in]   L    Length of the system
 * \param[in]   R    Number of windows
 * \param[in]   a    Hop factor
 * \param[in]   M    Number of channels
 * \param[out] gd    Canonical dual window
 *
 * \returns Status code
 *
 * Versions
 * --------
 *
 * <tt>
 * gabdual_long_d(const double* g, const ltfatInt L, const ltfatInt R,
 *                const ltfatInt a, const ltfatInt M, double* gd);
 *
 * gabdual_long_s(const float* g, const ltfatInt L, const ltfatInt R,
 *                const ltfatInt a, const ltfatInt M, float* gd);
 *
 * gabdual_long_cd(const complex double* g, const ltfatInt L, const ltfatInt R,
 *                 const ltfatInt a, const ltfatInt M, complex double* gd);
 *
 * gabdual_long_cs(const complex float* g, const ltfatInt L, const ltfatInt R,
 *                 const ltfatInt a, const ltfatInt M, complex float* gd);
 * </tt>
 */
LTFAT_EXTERN int
LTFAT_NAME(gabdual_long)(const LTFAT_TYPE *g,
                         const ltfatInt L, const ltfatInt R, const ltfatInt a,
                         const ltfatInt M, LTFAT_TYPE *gd);

/** Compute canonical tight window for Gabor system
 *
 * The Gabor system must be a frame i.e. M>=a.
 *
 * \param[in]   g    Original window(s), size L x R
 * \param[in]   L    Length of the system
 * \param[in]   R    Number of windows
 * \param[in]   a    Hop factor
 * \param[in]   M    Number of channels
 * \param[out] gt    Canonical tight window
 *
 * \returns Status code
 *
 * Versions
 * --------
 *
 * <tt>
 * gabtight_long_d(const double* g, const ltfatInt L, const ltfatInt R,
 *                 const ltfatInt a, const ltfatInt M, double* gt);
 *
 * gabtight_long_s(const float* g, const ltfatInt L, const ltfatInt R,
 *                 const ltfatInt a, const ltfatInt M, float* gt);
 *
 * gabtight_long_cd(const complex double* g, const ltfatInt L, const ltfatInt R,
 *                  const ltfatInt a, const ltfatInt M, complex double* gt);
 *
 * gabtight_long_cs(const complex float* g, const ltfatInt L, const ltfatInt R,
 *                  const ltfatInt a, const ltfatInt M, complex float* gt);
 * </tt>
 */
LTFAT_EXTERN int
LTFAT_NAME(gabtight_long)(const LTFAT_TYPE *g,
                          const ltfatInt L, const ltfatInt R, const ltfatInt a,
                          const ltfatInt M, LTFAT_TYPE *gd);

/** Compute FIR canonical dual window for Gabor system
 *
 * The Gabor system must be a frame i.e. M>=a.
 *
 * \param[in]    g    Original window
 * \param[in]   gl    Length of the window
 * \param[in]    L    Length of the system
 * \param[in]    a    Hop factor
 * \param[in]    M    Number of channels
 * \param[in]  gdl    Length of the dual window
 * \param[out]  gd    Canonical dual window
 *
 * \returns Status code
 *
 * Versions
 * --------
 *
 * <tt>
 * gabdual_fir_d(const double* g, const ltfatInt gl, const ltfatInt L,
 *               const ltfatInt a, const ltfatInt M, const ltfatInt dgl,
 *               double* gd);
 *
 * gabdual_fir_s(const float* g, const ltfatInt gl, const ltfatInt L,
 *               const ltfatInt a, const ltfatInt M, const ltfatInt dgl,
 *               float* gd);
 *
 * gabdual_fir_cd(const complex double* g, const ltfatInt gl, const ltfatInt L,
 *                const ltfatInt a, const ltfatInt M, const ltfatInt dgl,
 *                complex double* gd);
 *
 * gabdual_fir_cs(const complex float* g, const ltfatInt gl, const ltfatInt L,
 *                const ltfatInt a, const ltfatInt M, const ltfatInt dgl,
 *                complex float* gd);
 * </tt>
 */
LTFAT_EXTERN int
LTFAT_NAME(gabdual_fir)(const LTFAT_TYPE* g, const ltfatInt gl,
                        const ltfatInt L, const ltfatInt a,
                        const ltfatInt M, const ltfatInt gdl, LTFAT_TYPE* gd);

/** Compute FIR canonical tight window for Gabor system
 *
 * The Gabor system must be a frame i.e. M>=a.
 *
 * \param[in]    g    Original window
 * \param[in]   gl    Length of the window
 * \param[in]    L    Length of the system
 * \param[in]    a    Hop factor
 * \param[in]    M    Number of channels
 * \param[in]  gtl    Length of the dual window
 * \param[out]  gt    Canonical dual window
 *
 * \returns Status code
 *
 * Versions
 * --------
 *
 * <tt>
 * gabtight_fir_d(const double* g, const ltfatInt gl, const ltfatInt L,
 *                const ltfatInt a, const ltfatInt M, const ltfatInt dtl,
 *                double* gt);
 *
 * gabtight_fir_s(const float* g, const ltfatInt gl, const ltfatInt L,
 *                const ltfatInt a, const ltfatInt M, const ltfatInt dtl,
 *                float* gt);
 *
 * gabtight_fir_cd(const complex double* g, const ltfatInt gl, const ltfatInt L,
 *                 const ltfatInt a, const ltfatInt M, const ltfatInt dtl,
 *                 complex double* gt);
 *
 * gabtight_fir_cs(const complex float* g, const ltfatInt gl, const ltfatInt L,
 *                  const ltfatInt a, const ltfatInt M, const ltfatInt dtl,
 *                  complex float* gt);
 * </tt>
 */
LTFAT_EXTERN int
LTFAT_NAME(gabtight_fir)(const LTFAT_TYPE* g, const ltfatInt gl,
                         const ltfatInt L, const ltfatInt a,
                         const ltfatInt M, const ltfatInt gtl, LTFAT_TYPE* gt);

/** @} */

/* --------- Wilson and WMDCT bases ---------*/
LTFAT_EXTERN void
LTFAT_NAME(dwilt_long)(const LTFAT_TYPE *f,
                       const LTFAT_TYPE *g,
                       const ltfatInt L, const ltfatInt W, const ltfatInt M,
                       LTFAT_TYPE *cout);

LTFAT_EXTERN void
LTFAT_NAME(dwilt_fb)(const LTFAT_TYPE *f, const LTFAT_TYPE *g,
                     const ltfatInt L, const ltfatInt gl, const ltfatInt W, const ltfatInt M,
                     LTFAT_TYPE *cout);


LTFAT_EXTERN void
LTFAT_NAME(dwiltiii_long)(const LTFAT_TYPE *f,
                          const LTFAT_TYPE *g,
                          const ltfatInt L, const ltfatInt W, const ltfatInt M,
                          LTFAT_TYPE *cout);

LTFAT_EXTERN void
LTFAT_NAME(dwiltiii_fb)(const LTFAT_TYPE *f, const LTFAT_TYPE *g,
                        const ltfatInt L, const ltfatInt gl, const ltfatInt W, const ltfatInt M,
                        LTFAT_TYPE *cout);


/* --------- Wilson and WMDCT inverses ---------*/


LTFAT_EXTERN void
LTFAT_NAME(idwilt_long)(const LTFAT_TYPE *cin,
                        const LTFAT_TYPE *g,
                        const ltfatInt L, const ltfatInt W, const ltfatInt M,
                        LTFAT_TYPE *f);

LTFAT_EXTERN void
LTFAT_NAME(idwilt_fb)(const LTFAT_TYPE *cin, const LTFAT_TYPE *g,
                      const ltfatInt L, const ltfatInt gl, const ltfatInt W, const ltfatInt M,
                      LTFAT_TYPE *f);

LTFAT_EXTERN void
LTFAT_NAME(idwiltiii_long)(const LTFAT_TYPE *cin,
                           const LTFAT_TYPE *g,
                           const ltfatInt L, const ltfatInt W, const ltfatInt M,
                           LTFAT_TYPE *f);

LTFAT_EXTERN void
LTFAT_NAME(idwiltiii_fb)(const LTFAT_TYPE *cin, const LTFAT_TYPE *g,
                         const ltfatInt L, const ltfatInt gl, const ltfatInt W, const ltfatInt M,
                         LTFAT_TYPE *f);

/* --------------- DCT -------------------*/

LTFAT_EXTERN LTFAT_FFTW(plan)
LTFAT_NAME(dct_init)( const ltfatInt L, const ltfatInt W, LTFAT_TYPE *cout,
                      const dct_kind kind);


LTFAT_EXTERN void
LTFAT_NAME(dct)(const LTFAT_TYPE *f, const ltfatInt L, const ltfatInt W,
                LTFAT_TYPE *cout, const dct_kind kind);

LTFAT_EXTERN void
LTFAT_NAME(dct_execute)(const LTFAT_FFTW(plan) p, const LTFAT_TYPE *f,
                        const ltfatInt L, const ltfatInt W,
                        LTFAT_TYPE *cout, const dct_kind kind);

/* --------------- DST -------------------*/

LTFAT_EXTERN LTFAT_FFTW(plan)
LTFAT_NAME(dst_init)( const ltfatInt L, const ltfatInt W, LTFAT_TYPE *cout,
                      const dst_kind kind);

LTFAT_EXTERN void
LTFAT_NAME(dst)(const LTFAT_TYPE *f, const ltfatInt L, const ltfatInt W,
                LTFAT_TYPE *cout, const dst_kind kind);

LTFAT_EXTERN void
LTFAT_NAME(dst_execute)(LTFAT_FFTW(plan) p, const LTFAT_TYPE *f,
                        const ltfatInt L, const ltfatInt W, LTFAT_TYPE *cout,
                        const dst_kind kind);

/* --------------- Reassignment -----------*/

LTFAT_EXTERN void
LTFAT_NAME(gabreassign)(const LTFAT_TYPE *s, const LTFAT_REAL *tgrad,
                        const LTFAT_REAL *fgrad, const ltfatInt L, const ltfatInt W,
                        const ltfatInt a, const ltfatInt M, LTFAT_TYPE *sr);

LTFAT_EXTERN void
LTFAT_NAME(filterbankreassign)(const LTFAT_TYPE*     s[],
                               const LTFAT_REAL* tgrad[],
                               const LTFAT_REAL* fgrad[],
                               const ltfatInt        N[],
                               const double          a[],
                               const double      cfreq[],
                               const ltfatInt          M,
                               LTFAT_TYPE*          sr[],
                               fbreassHints        hints,
                               fbreassOptOut*      repos);