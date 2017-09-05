/** \defgroup dgtrealwrapper Discrete Gabor Transform analysis-synthesis;
*/

#ifndef _ltfat_dgtrealwrapper_h
#define _ltfat_dgtrealwrapper_h
typedef struct ltfat_dgtreal_params ltfat_dgtreal_params;

/** \addtogroup dgtrealwrapper
 * @{ */
typedef enum
{
    ltfat_dgtreal_auto,
    ltfat_dgtreal_long,
    ltfat_dgtreal_fb
} ltfat_dgtreal_hint;

/** Allocate dgtreal_params structure and initialize to default values
 *
 * \warning The structure must be freed using ltfat_dgtreal_params_free()
 *
 * \returns Allocated struct (or NULL if the memory allocation failed
 * \see ltfat_dgtreal_params_free
 */
LTFAT_API ltfat_dgtreal_params*
ltfat_dgtreal_params_allocdef();

/** Set DGT phase convention
 *
 * \returns
 * Status code          |  Description
 * ---------------------|----------------
 * LTFATERR_SUCESS      |  No error occured
 * LTFATERR_NULLPOINTER |  \a params was NULL
 */
LTFAT_API int
ltfat_dgtreal_params_set_phaseconv(ltfat_dgtreal_params* params, ltfat_phaseconvention ptype);

/** Set FFTW flags
 *
 * \returns
 * Status code          |  Description
 * ---------------------|----------------
 * LTFATERR_SUCESS      |  No error occured
 * LTFATERR_NULLPOINTER |  \a params was NULL
 */
LTFAT_API int
ltfat_dgtreal_params_set_fftwflags(ltfat_dgtreal_params* params, unsigned fftw_flags);

/** Set algorithm hint
 *
 * \returns
 * Status code          |  Description
 * ---------------------|----------------
 * LTFATERR_SUCESS      |  No error occured
 * LTFATERR_NULLPOINTER |  \a params was NULL
 */
LTFAT_API int
ltfat_dgtreal_params_set_hint(ltfat_dgtreal_params* params, ltfat_dgtreal_hint hint);

/** Destroy struct
 *
 * \returns
 * Status code          |  Description
 * ---------------------|----------------
 * LTFATERR_SUCESS      |  No error occured
 * LTFATERR_NULLPOINTER |  \a params was NULL
 */
LTFAT_API int
ltfat_dgtreal_params_free(ltfat_dgtreal_params* params);

/** @} */

// The following function is not part of API
int
ltfat_dgtreal_params_defaults(ltfat_dgtreal_params* params);

#endif

typedef struct LTFAT_NAME(dgtreal_plan) LTFAT_NAME(dgtreal_plan);

/** \addtogroup dgtrealwrapper
 * @{
 *
 */

/**
 * Note c can be NULL if FFTW_ESTIMATE is used in flags
 *
 * \returns
 * Status code             | Description
 * ------------------------|--------------------------
 * LTFATERR_SUCCESS        | No error occurred
 * LTFATERR_NULLPOINTER    | \a g or \a p was NULL or \a c was NULL and flags != FFTW_ESTIMATE
 * LTFATERR_BADSIZE        | Signal length L is less or equal to 0.
 * LTFATERR_NOTPOSARG      | At least one of \f W, \f a, \f M, \f gl was less or equal to zero.
 * LTFATERR_NOTAFRAME      | System does not form a frame
 * LTFATERR_BADTRALEN      | \a L is not divisible by both \a a and \a M.
 * LTFATERR_INITFAILED     | The FFTW plan creation failed
 * LTFATERR_NOTSUPPORTED   | This is a non-painless system but its support was not compiled
 * LTFATERR_CANNOTHAPPEN   | \a hint does not have a valid value from \a ltfat_dgtreal_hint or \a ptype is not valid value from \a ltfat_phaseconvention enum
 * LTFATERR_NOMEM          | Signalizes memory allocation error
 */
LTFAT_API int
LTFAT_NAME(dgtreal_init)(const LTFAT_REAL g[], ltfat_int gl,
                         ltfat_int L, ltfat_int W, ltfat_int a, ltfat_int M,
                         LTFAT_REAL f[], LTFAT_COMPLEX c[],
                         ltfat_dgtreal_params* params, LTFAT_NAME(dgtreal_plan)** p);


LTFAT_API int
LTFAT_NAME(dgtreal_init_gen)(const LTFAT_REAL ga[], ltfat_int gal,
                             const LTFAT_REAL gs[], ltfat_int gsl,
                             ltfat_int L, ltfat_int W, ltfat_int a, ltfat_int M,
                             LTFAT_REAL f[], LTFAT_COMPLEX c[],
                             ltfat_dgtreal_params* params, LTFAT_NAME(dgtreal_plan)** p);

/** Perform DGTREAL synthesis followed by analysis
 *
 * \note This function CAN work inplace.
 *
 * M2 = M/2 + 1, N = L/a
 *
 * \param[in]    p  Transform plan
 * \param[in]  cin  Input coefficients, size M2 x N x W
 * \param[out]   c  Coefficients after projection, size M2 x N x W
 *
 * #### Versions #
 * <tt>
 * ltfat_dgtreal_execute_proj_d(ltfat_dgtreal_plan_d* p,
 *                                        const ltfat_complex_d cin[],
 *                                        ltfat_complex_d c[]);
 *
 * ltfat_dgtreal_execute_proj_s(ltfat_dgtreal_plan_s* p,
 *                                        const ltfat_complex_s cin[],
 *                                        ltfat_complex_s c[]);
 * </tt>
 *
 * \returns
 */
LTFAT_API int
LTFAT_NAME(dgtreal_execute_proj)(LTFAT_NAME(dgtreal_plan)* p,
                                 const LTFAT_COMPLEX cin[], LTFAT_REAL fbuffer[], LTFAT_COMPLEX c[]);

/** Perform DGTREAL synthesis
 *
 * M2 = M/2 + 1, N = L/a
 *
 * \param[in]    p  Transform plan
 * \param[in]    c  Input coefficients, size M2 x N x W
 * \param[out]   f  Reconstructed signal, size L x W
 *
 * #### Versions #
 * <tt>
 * ltfat_dgtreal_execute_syn_d(ltfat_dgtreal_plan_d* p,
 *                                       const ltfat_complex_d c[],
 *                                       double f[]);
 *
 * ltfat_dgtreal_execute_syn_s(ltfat_dgtreal_plan_s* p,
 *                                       const ltfat_complex_s c[],
 *                                       float f[]);
 * </tt>
 *
 * \returns
 */
LTFAT_API int
LTFAT_NAME(dgtreal_execute_syn_newarray)(LTFAT_NAME(dgtreal_plan)* p,
                                const LTFAT_COMPLEX c[], LTFAT_REAL f[]);

LTFAT_API int
LTFAT_NAME(dgtreal_execute_syn)(LTFAT_NAME(dgtreal_plan)* p);

/** Perform DGTREAL analysis
 *
 * M2 = M/2 + 1, N = L/a
 *
 * \param[in]    p  Transform plan
 * \param[in]    f  Input signal, size L x W
 * \param[out]   c  Coefficients, size M2 x N x W
 *
 * #### Versions #
 * <tt>
 * ltfat_dgtreal_execute_ana_d(ltfat_dgtreal_plan_d* p,
 *                                       const double f[], ltfat_complex_d c[]);
 *
 * ltfat_dgtreal_execute_ana_s(ltfat_dgtreal_plan_s* p,
 *                                       const float f[], ltfat_complex_s c[]);
 * </tt>
 * \returns
 */
LTFAT_API int
LTFAT_NAME(dgtreal_execute_ana_newarray)(LTFAT_NAME(dgtreal_plan)* p,
                                const LTFAT_REAL f[], LTFAT_COMPLEX c[]);

LTFAT_API int
LTFAT_NAME(dgtreal_execute_ana)(LTFAT_NAME(dgtreal_plan)* p);

/** Destroy transform plan
 *
 * \param[in]   p  Transform plan
 *
 * #### Versions #
 * <tt>
 * ltfat_dgtreal_done_d(ltfat_dgtreal_plan_d** p);
 *
 * ltfat_dgtreal_done_s(ltfat_dgtreal_plan_s** p);
 * </tt>
 * \returns
 */
LTFAT_API int
LTFAT_NAME(dgtreal_done)(LTFAT_NAME(dgtreal_plan)** p);

/** @} */


