/**
 * @brief implement for openspblas_sparse_?_trsm intelface
 * @author Zhuoqiang Guo <gzq9425@qq.com>
 */

#include "openspblas/util.h"
#include "openspblas/opt.h"
#include "openspblas/spapi.h"
#include "openspblas/kernel.h"

static openspblas_sparse_status_t (*trsm_csr_diag_fill_layout_operation[])(const OPENSPBLAS_Number alpha,
                                                                     const OPENSPBLAS_SPMAT_CSR *A,
                                                                     const OPENSPBLAS_Number *x,
                                                                     const OPENSPBLAS_INT columns,
                                                                     const OPENSPBLAS_INT ldx,
                                                                     OPENSPBLAS_Number *y,
                                                                     const OPENSPBLAS_INT ldy) = {
    trsm_csr_n_lo_row,
    trsm_csr_u_lo_row,
    trsm_csr_n_hi_row,
    trsm_csr_u_hi_row,
    trsm_csr_n_lo_col,
    trsm_csr_u_lo_col,
    trsm_csr_n_hi_col,
    trsm_csr_u_hi_col,
    trsm_csr_n_lo_row_trans,
    trsm_csr_u_lo_row_trans,
    trsm_csr_n_hi_row_trans,
    trsm_csr_u_hi_row_trans,
    trsm_csr_n_lo_col_trans,
    trsm_csr_u_lo_col_trans,
    trsm_csr_n_hi_col_trans,
    trsm_csr_u_hi_col_trans,
#ifdef COMPLEX
    trsm_csr_n_lo_row_conj, 
    trsm_csr_u_lo_row_conj, 
    trsm_csr_n_hi_row_conj, 
    trsm_csr_u_hi_row_conj, 
    trsm_csr_n_lo_col_conj, 
    trsm_csr_u_lo_col_conj, 
    trsm_csr_n_hi_col_conj, 
    trsm_csr_u_hi_col_conj, 
#endif
};

static openspblas_sparse_status_t (*diagsm_csr_diag_layout[])(const OPENSPBLAS_Number alpha,
                                                        const OPENSPBLAS_SPMAT_CSR *A,
                                                        const OPENSPBLAS_Number *x,
                                                        const OPENSPBLAS_INT columns,
                                                        const OPENSPBLAS_INT ldx,
                                                        OPENSPBLAS_Number *y,
                                                        const OPENSPBLAS_INT ldy) = {
    diagsm_csr_n_row,
    diagsm_csr_u_row,
    diagsm_csr_n_col,
    diagsm_csr_u_col,
};

static openspblas_sparse_status_t (*trsm_csc_diag_fill_layout_operation[])(const OPENSPBLAS_Number alpha,
                                                                     const OPENSPBLAS_SPMAT_CSC *A,
                                                                     const OPENSPBLAS_Number *x,
                                                                     const OPENSPBLAS_INT columns,
                                                                     const OPENSPBLAS_INT ldx,
                                                                     OPENSPBLAS_Number *y,
                                                                     const OPENSPBLAS_INT ldy) = {
    trsm_csc_n_lo_row,
    trsm_csc_u_lo_row,
    trsm_csc_n_hi_row,
    trsm_csc_u_hi_row,
    trsm_csc_n_lo_col,
    trsm_csc_u_lo_col,
    trsm_csc_n_hi_col,
    trsm_csc_u_hi_col,
    trsm_csc_n_lo_row_trans,
    trsm_csc_u_lo_row_trans,
    trsm_csc_n_hi_row_trans,
    trsm_csc_u_hi_row_trans,
    trsm_csc_n_lo_col_trans,
    trsm_csc_u_lo_col_trans,
    trsm_csc_n_hi_col_trans,
    trsm_csc_u_hi_col_trans,
#ifdef COMPLEX
    trsm_csc_n_lo_row_conj, 
    trsm_csc_u_lo_row_conj, 
    trsm_csc_n_hi_row_conj, 
    trsm_csc_u_hi_row_conj, 
    trsm_csc_n_lo_col_conj, 
    trsm_csc_u_lo_col_conj, 
    trsm_csc_n_hi_col_conj, 
    trsm_csc_u_hi_col_conj, 
#endif
};

static openspblas_sparse_status_t (*diagsm_csc_diag_layout[])(const OPENSPBLAS_Number alpha,
                                                        const OPENSPBLAS_SPMAT_CSC *A,
                                                        const OPENSPBLAS_Number *x,
                                                        const OPENSPBLAS_INT columns,
                                                        const OPENSPBLAS_INT ldx,
                                                        OPENSPBLAS_Number *y,
                                                        const OPENSPBLAS_INT ldy) = {
    diagsm_csc_n_row,
    diagsm_csc_u_row,
    diagsm_csc_n_col,
    diagsm_csc_u_col,
};

static openspblas_sparse_status_t (*trsm_coo_diag_fill_layout_operation[])(const OPENSPBLAS_Number alpha,
                                                                     const OPENSPBLAS_SPMAT_COO *A,
                                                                     const OPENSPBLAS_Number *x,
                                                                     const OPENSPBLAS_INT columns,
                                                                     const OPENSPBLAS_INT ldx,
                                                                     OPENSPBLAS_Number *y,
                                                                     const OPENSPBLAS_INT ldy) = {
    trsm_coo_n_lo_row,
    trsm_coo_u_lo_row,
    trsm_coo_n_hi_row,
    trsm_coo_u_hi_row,
    trsm_coo_n_lo_col,
    trsm_coo_u_lo_col,
    trsm_coo_n_hi_col,
    trsm_coo_u_hi_col,
    trsm_coo_n_lo_row_trans,
    trsm_coo_u_lo_row_trans,
    trsm_coo_n_hi_row_trans,
    trsm_coo_u_hi_row_trans,
    trsm_coo_n_lo_col_trans,
    trsm_coo_u_lo_col_trans,
    trsm_coo_n_hi_col_trans,
    trsm_coo_u_hi_col_trans,
#ifdef COMPLEX
    trsm_coo_n_lo_row_conj, 
    trsm_coo_u_lo_row_conj, 
    trsm_coo_n_hi_row_conj, 
    trsm_coo_u_hi_row_conj, 
    trsm_coo_n_lo_col_conj, 
    trsm_coo_u_lo_col_conj, 
    trsm_coo_n_hi_col_conj, 
    trsm_coo_u_hi_col_conj, 
#endif
};

static openspblas_sparse_status_t (*diagsm_coo_diag_layout[])(const OPENSPBLAS_Number alpha,
                                                        const OPENSPBLAS_SPMAT_COO *A,
                                                        const OPENSPBLAS_Number *x,
                                                        const OPENSPBLAS_INT columns,
                                                        const OPENSPBLAS_INT ldx,
                                                        OPENSPBLAS_Number *y,
                                                        const OPENSPBLAS_INT ldy) = {
    diagsm_coo_n_row,
    diagsm_coo_u_row,
    diagsm_coo_n_col,
    diagsm_coo_u_col,
};

static openspblas_sparse_status_t (*trsm_bsr_diag_fill_layout_operation[])(const OPENSPBLAS_Number alpha,
                                                                     const OPENSPBLAS_SPMAT_BSR *A,
                                                                     const OPENSPBLAS_Number *x,
                                                                     const OPENSPBLAS_INT columns,
                                                                     const OPENSPBLAS_INT ldx,
                                                                     OPENSPBLAS_Number *y,
                                                                     const OPENSPBLAS_INT ldy) = {
    trsm_bsr_n_lo_row,
    trsm_bsr_u_lo_row,
    trsm_bsr_n_hi_row,
    trsm_bsr_u_hi_row,
    trsm_bsr_n_lo_col,
    trsm_bsr_u_lo_col,
    trsm_bsr_n_hi_col,
    trsm_bsr_u_hi_col,
    trsm_bsr_n_lo_row_trans,
    trsm_bsr_u_lo_row_trans,
    trsm_bsr_n_hi_row_trans,
    trsm_bsr_u_hi_row_trans,
    trsm_bsr_n_lo_col_trans,
    trsm_bsr_u_lo_col_trans,
    trsm_bsr_n_hi_col_trans,
    trsm_bsr_u_hi_col_trans,
#ifdef COMPLEX
    trsm_bsr_n_lo_row_conj, 
    trsm_bsr_u_lo_row_conj, 
    trsm_bsr_n_hi_row_conj, 
    trsm_bsr_u_hi_row_conj, 
    trsm_bsr_n_lo_col_conj, 
    trsm_bsr_u_lo_col_conj, 
    trsm_bsr_n_hi_col_conj, 
    trsm_bsr_u_hi_col_conj, 
#endif
};

static openspblas_sparse_status_t (*diagsm_bsr_diag_layout[])(const OPENSPBLAS_Number alpha,
                                                        const OPENSPBLAS_SPMAT_BSR *A,
                                                        const OPENSPBLAS_Number *x,
                                                        const OPENSPBLAS_INT columns,
                                                        const OPENSPBLAS_INT ldx,
                                                        OPENSPBLAS_Number *y,
                                                        const OPENSPBLAS_INT ldy) = {
    diagsm_bsr_n_row,
    diagsm_bsr_u_row,
    diagsm_bsr_n_col,
    diagsm_bsr_u_col,
};

static openspblas_sparse_status_t (*trsm_sky_diag_fill_layout_operation[])(const OPENSPBLAS_Number alpha,
                                                                     const OPENSPBLAS_SPMAT_SKY *A,
                                                                     const OPENSPBLAS_Number *x,
                                                                     const OPENSPBLAS_INT columns,
                                                                     const OPENSPBLAS_INT ldx,
                                                                     OPENSPBLAS_Number *y,
                                                                     const OPENSPBLAS_INT ldy) = {
    trsm_sky_n_lo_row,
    trsm_sky_u_lo_row,
    trsm_sky_n_hi_row,
    trsm_sky_u_hi_row,
    trsm_sky_n_lo_col,
    trsm_sky_u_lo_col,
    trsm_sky_n_hi_col,
    trsm_sky_u_hi_col,
    trsm_sky_n_lo_row_trans,
    trsm_sky_u_lo_row_trans,
    trsm_sky_n_hi_row_trans,
    trsm_sky_u_hi_row_trans,
    trsm_sky_n_lo_col_trans,
    trsm_sky_u_lo_col_trans,
    trsm_sky_n_hi_col_trans,
    trsm_sky_u_hi_col_trans,
#ifdef COMPLEX
    trsm_sky_n_lo_row_conj, 
    trsm_sky_u_lo_row_conj, 
    trsm_sky_n_hi_row_conj, 
    trsm_sky_u_hi_row_conj, 
    trsm_sky_n_lo_col_conj, 
    trsm_sky_u_lo_col_conj, 
    trsm_sky_n_hi_col_conj, 
    trsm_sky_u_hi_col_conj, 
#endif
};

static openspblas_sparse_status_t (*diagsm_sky_diag_layout[])(const OPENSPBLAS_Number alpha,
                                                        const OPENSPBLAS_SPMAT_SKY *A,
                                                        const OPENSPBLAS_Number *x,
                                                        const OPENSPBLAS_INT columns,
                                                        const OPENSPBLAS_INT ldx,
                                                        OPENSPBLAS_Number *y,
                                                        const OPENSPBLAS_INT ldy) = {
    diagsm_sky_n_row,
    diagsm_sky_u_row,
    diagsm_sky_n_col,
    diagsm_sky_u_col,
};

static openspblas_sparse_status_t (*trsm_dia_diag_fill_layout_operation[])(const OPENSPBLAS_Number alpha,
                                                                     const OPENSPBLAS_SPMAT_DIA *A,
                                                                     const OPENSPBLAS_Number *x,
                                                                     const OPENSPBLAS_INT columns,
                                                                     const OPENSPBLAS_INT ldx,
                                                                     OPENSPBLAS_Number *y,
                                                                     const OPENSPBLAS_INT ldy) = {
    trsm_dia_n_lo_row,
    trsm_dia_u_lo_row,
    trsm_dia_n_hi_row,
    trsm_dia_u_hi_row,
    trsm_dia_n_lo_col,
    trsm_dia_u_lo_col,
    trsm_dia_n_hi_col,
    trsm_dia_u_hi_col,
    trsm_dia_n_lo_row_trans,
    trsm_dia_u_lo_row_trans,
    trsm_dia_n_hi_row_trans,
    trsm_dia_u_hi_row_trans,
    trsm_dia_n_lo_col_trans,
    trsm_dia_u_lo_col_trans,
    trsm_dia_n_hi_col_trans,
    trsm_dia_u_hi_col_trans,
#ifdef COMPLEX
    trsm_dia_n_lo_row_conj, 
    trsm_dia_u_lo_row_conj, 
    trsm_dia_n_hi_row_conj, 
    trsm_dia_u_hi_row_conj, 
    trsm_dia_n_lo_col_conj, 
    trsm_dia_u_lo_col_conj, 
    trsm_dia_n_hi_col_conj, 
    trsm_dia_u_hi_col_conj, 
#endif
};

static openspblas_sparse_status_t (*diagsm_dia_diag_layout[])(const OPENSPBLAS_Number alpha,
                                                        const OPENSPBLAS_SPMAT_DIA *A,
                                                        const OPENSPBLAS_Number *x,
                                                        const OPENSPBLAS_INT columns,
                                                        const OPENSPBLAS_INT ldx,
                                                        OPENSPBLAS_Number *y,
                                                        const OPENSPBLAS_INT ldy) = {
    diagsm_dia_n_row,
    diagsm_dia_u_row,
    diagsm_dia_n_col,
    diagsm_dia_u_col,
};

openspblas_sparse_status_t ONAME(const openspblas_sparse_operation_t operation,
                                            const OPENSPBLAS_Number alpha,
                                            const openspblas_sparse_matrix_t A,
                                            const struct openspblas_matrix_descr descr, /* openspblas_sparse_matrix_type_t + openspblas_sparse_fill_mode_t + openspblas_sparse_diag_type_t */
                                            const openspblas_sparse_layout_t layout,    /* storage scheme for the dense matrix: C-style or Fortran-style */
                                            const OPENSPBLAS_Number *x,
                                            const OPENSPBLAS_INT columns,
                                            const OPENSPBLAS_INT ldx,
                                            OPENSPBLAS_Number *y,
                                            const OPENSPBLAS_INT ldy)
{
    check_null_return(A->mat, OPENSPBLAS_SPARSE_STATUS_NOT_INITIALIZED);
    check_null_return(x, OPENSPBLAS_SPARSE_STATUS_NOT_INITIALIZED);
    check_null_return(y, OPENSPBLAS_SPARSE_STATUS_NOT_INITIALIZED);

    check_return(A->datatype != OPENSPBLAS_SPARSE_DATATYPE, OPENSPBLAS_SPARSE_STATUS_INVALID_VALUE);

#ifndef COMPLEX
    if(operation == OPENSPBLAS_SPARSE_OPERATION_CONJUGATE_TRANSPOSE)
        return OPENSPBLAS_SPARSE_STATUS_INVALID_VALUE;
#endif

    check_return(!check_equal_row_col(A),OPENSPBLAS_SPARSE_STATUS_INVALID_VALUE);

    if(A->format == OPENSPBLAS_SPARSE_FORMAT_CSR)
    {
        if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_TRIANGULAR)
        {
            check_null_return(trsm_csr_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return trsm_csr_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_DIAGONAL)
        {
            check_null_return(diagsm_csr_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return diagsm_csr_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else
        {
            return OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED;
        }
    }
    else if(A->format == OPENSPBLAS_SPARSE_FORMAT_CSC)
    {
        if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_TRIANGULAR)
        {
            check_null_return(trsm_csc_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return trsm_csc_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_DIAGONAL)
        {
            check_null_return(diagsm_csc_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return diagsm_csc_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else
        {
            return OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED;
        }
    }
    else if(A->format == OPENSPBLAS_SPARSE_FORMAT_COO)
    {
        if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_TRIANGULAR)
        {
            check_null_return(trsm_coo_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return trsm_coo_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_DIAGONAL)
        {
            check_null_return(diagsm_coo_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return diagsm_coo_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else
        {
            return OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED;
        }
    }
    else if(A->format == OPENSPBLAS_SPARSE_FORMAT_SKY)
    {
        if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_TRIANGULAR)
        {
            check_null_return(trsm_sky_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return trsm_sky_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_DIAGONAL)
        {
            check_null_return(diagsm_sky_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return diagsm_sky_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else
        {
            return OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED;
        }
    }
    else if(A->format == OPENSPBLAS_SPARSE_FORMAT_BSR)
    {
        if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_TRIANGULAR)
        {
            check_null_return(trsm_bsr_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return trsm_bsr_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_DIAGONAL)
        {
            check_null_return(diagsm_bsr_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return diagsm_bsr_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else
        {
            return OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED;
        }
    }
    else if(A->format == OPENSPBLAS_SPARSE_FORMAT_DIA)
    {
        if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_TRIANGULAR)
        {
            check_null_return(trsm_dia_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return trsm_dia_diag_fill_layout_operation[index4(operation, layout, descr.mode, descr.diag, OPENSPBLAS_SPARSE_LAYOUT_NUM, OPENSPBLAS_SPARSE_FILL_MODE_NUM, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else if (descr.type == OPENSPBLAS_SPARSE_MATRIX_TYPE_DIAGONAL)
        {
            check_null_return(diagsm_dia_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)], OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED);
            return diagsm_dia_diag_layout[index2(layout, descr.diag, OPENSPBLAS_SPARSE_DIAG_TYPE_NUM)](alpha, A->mat, x, columns, ldx, y, ldy);
        }
        else
        {
            return OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED;
        }
    }
    else
    {
        return OPENSPBLAS_SPARSE_STATUS_NOT_SUPPORTED;
    }
}