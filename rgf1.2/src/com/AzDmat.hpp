/* * * * *
 *  AzDmat.hpp 
 *  Copyright (C) 2011, 2012 Rie Johnson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * * * * */

#ifndef _AZ_DMAT_HPP_
#define _AZ_DMAT_HPP_

#include "AzUtil.hpp"
#include "AzStrArray.hpp"
#include "AzSmat.hpp"
#include "AzReadOnlyMatrix.hpp"

//! dense vector 
class AzDvect : /* implements */ public virtual AzReadOnlyVector {
protected:
  int num; 
  double *elm; /* updated only through AzBaseArray functions */
               /* must not be NULL no matter what ... */
  AzBaseArray<double> a; 

  void _release() {
    a.free(&elm); num = 0; 
  }

public:
  AzDvect() : num(0), elm(NULL) {} 
  AzDvect(int inp_num) : num(0), elm(NULL) {
    initialize(inp_num); 
  }
  AzDvect(const double inp[], int inp_num) : num(0), elm(NULL) {
    initialize(inp_num); 
    int ix; 
    for (ix = 0; ix < num; ++ix) elm[ix] = inp[ix]; 
  }

  AzDvect(const AzDvect *inp) : num(0), elm(NULL) {
    initialize(inp); 
  }
  void set(const AzDvect *inp, double coeff=1) {
    _release(); 
    initialize(inp); 
    if (coeff != 1) multiply(coeff); 
  }

  AzDvect(const AzDvect &inp) : num(0), elm(NULL) {
    initialize(&inp); 
  }
  AzDvect & operator =(const AzDvect &inp) {
    if (this == &inp) return *this; 
    _release(); 
    initialize(&inp); 
    return *this; 
  }
  AzDvect(const AzReadOnlyVector *inp) /* copy */
    : num(0), elm(NULL) {
    initialize(inp); 
  }
  void set(const AzReadOnlyVector *inp) {
    _release(); 
    initialize(inp); 
  }

  ~AzDvect() {}

  AzDvect(AzFile *file) : num(0), elm(NULL) {
    _read(file); 
  }

  void reset() {
    _release(); 
  }

  void reform(int new_row_num); 
  void resize(int new_row_num); 

  int write(AzFile *file); 

  int write(const char *fn) {
    AzFile file(fn); 
    file.open("wb"); 
    int len = write(&file); 
    file.close(true); 
    return len; 
  }
  void read(const char *fn) {
    _release(); 
    AzFile file(fn); 
    file.open("rb"); 
    read(&file); 
    file.close(); 
  }
  void read(AzFile *file) {
    _release(); 
    _read(file); 
  }

  inline int size() const { return num; }
  inline int rowNum() const { return num; }  

  inline void destroy() {
    _release();  
  }

  bool isZero() const; 

  void load(const AzIFarr *ifa_row_val); 

  void cut(double min_val); 

  void binarize(); 
  void binarize1(); 

  void values(const int exs[], 
                 int ex_num, 
                 AzIFarr *ifa_ex_value); /* output */
  void nonZero(AzIFarr *ifq) const; 
  void nonZeroRowNo(AzIntArr *iq) const; 
  int nonZeroRowNum() const; 

  inline void set(int row, double val) {
    checkIndex(row, "AzDvect::set"); 
    elm[row] = val; 
  }
  void set(const double *inp, int inp_num); 
  void set(double val); 

  inline double get(int row) const {
    checkIndex(row, "AzDvect::get"); 
    return elm[row]; 
  }

  inline void add(int row, double val) {
    checkIndex(row, "AzDvect::add"); 
    if (val != 0) {
      elm[row] += val; 
    }
  }
  inline void add(double val, const AzIntArr *ia_rows) {
    if (ia_rows == NULL) return; 
    add(val, ia_rows->point(), ia_rows->size()); 
  }
  void add_nochk(double val, const AzIntArr *ia_rows) {
    add_nochk(val, ia_rows->point(), ia_rows->size()); 
  }
  void add(double val, const int *rows, int rows_num); 
  void add_nochk(double val, const int *rows, int rows_num); 

  inline void multiply(int row, double val) {
    checkIndex(row, "AzDvect::multiply"); 
    if (val != 1) {
      elm[row] *= val; 
    }
  }

  double sum() const; 
  double absSum() const; 
  double sum(const int *row, int row_num) const; 
  double absSum(const int *row, int row_num) const; 
  inline double sum(const AzIntArr *ia_rows) const {
    if (ia_rows == NULL) return sum(); 
    return sum(ia_rows->point(), ia_rows->size()); 
  }
  inline double absSum(const AzIntArr *ia_rows) const {
    if (ia_rows == NULL) return absSum(); 
    return absSum(ia_rows->point(), ia_rows->size()); 
  }
  inline double average(const AzIntArr *ia_rows) const {
    if (num == 0) return 0; 
    if (ia_rows == NULL) return sum() / (double)num; 
    int pop = ia_rows->size(); 
    return sum(ia_rows->point(), pop) / (double)pop; 
  }

  void add(const AzReadOnlyVector *vect1, double coefficient=1); 
  void add(const AzDvect *vect1, double coefficient=1); 

  void multiply(double val); 

  void scale(const AzReadOnlyVector *vect1, bool isInverse=false); 
  void scale(const AzDvect *dbles1, bool isInverse=false); 

  double innerProduct(const AzReadOnlyVector *vect1) const; 
  double innerProduct(const AzDvect *dbles1) const; 
  double selfInnerProduct() const; 

  double normalize(); 
  double normalize1(); 

  void zeroOut(); 

  int next(AzCursor &cursor, double &out_val) const; 

  double max(int *out_row = NULL) const;  
  double maxAbs(int *out_row = NULL, double *out_real_val = NULL) const; 
  double min(int *out_row = NULL) const;  
  double max(const AzIntArr *ia_dx, int *out_row = NULL) const;  
  double min(const AzIntArr *ia_dx, int *out_row = NULL) const;  

  void dump(const AzOut &out, const char *header, 
            const AzStrArray *sp_row = NULL, 
            int cut_num = -1) const; 

  inline double *point_u() {
    return elm; 
  }
  inline const double *point() const {
    return elm; 
  }

  void square(); 

  static inline double sum(const double val[], int num) {
    double sum = 0; 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      sum += val[ix]; 
    }
    return sum; 
  }
  static inline bool isNull(const AzDvect *v) {
    if (v == NULL) return true; 
    if (v->rowNum() == 0) return true; 
    return false; 
  }

  void mysqrt() {
    if (elm == NULL) return; 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      if (elm[ix]<0) throw new AzException("AzDvect:;sqrt", "Negative component"); 
      elm[ix]=sqrt(elm[ix]); 
    }
  }

  static void sdev(const AzDvect *v_avg, const AzDvect *v_avg2, AzDvect *v_sdev) {
    v_sdev->add(v_avg2); 
    AzDvect v(v_avg); v.scale(v_avg); 
    v_sdev->add(&v, -1); /* avg(x^2)-avg(x)^2 */
    v_sdev->mysqrt(); 
  }

protected:
  /*---  call _release() before calling initialize(...)  ---*/
  /*---  dimensionality may be changed  ---*/
  void initialize(int inp_num); 
  void initialize(const AzDvect *inp); 
  void initialize(const AzReadOnlyVector *inp); 
  void _read(AzFile *file); 

  void _swap(); 

  void _dump(const AzOut &out, const AzStrArray *sp_row, 
             int cut_num = -1) const; 
  inline void checkIndex(int row, const char *eyec) const {
    if (row < 0 || row >= num) {
      throw new AzException(eyec, "index is out of range"); 
    }
  }
}; 

//! dense matrix 
class AzDmat : /* implements */ public virtual AzReadOnlyMatrix {
protected:
  bool isLocked; 
  int col_num, row_num; 
  AzDvect **column;  /* updated only through AzPtrArray functions */
                     /* must not be NULL no matter what ... */
  AzObjPtrArray<AzDvect> a; 
  AzDvect dummy_zero; 

  void _release() {
    checkLock("_release"); 
    a.free(&column); col_num = 0; 
    row_num = 0; 
    dummy_zero.reform(0); 
  }
public: 
  AzDmat() : row_num(0), col_num(0), column(NULL), isLocked(false) {}
  AzDmat(int row_num, int col_num) 
           : row_num(0), col_num(0), column(NULL), isLocked(false)  {
    initialize(row_num, col_num); 
  }
  AzDmat(const AzDmat *inp) 
           : row_num(0), col_num(0), column(NULL), isLocked(false)  {
    initialize(inp); 
  }
  void set(const AzDmat *inp) {
    _release(); 
    initialize(inp); 
  }
  AzDmat(const AzDmat &inp) 
           : row_num(0), col_num(0), column(NULL), isLocked(false)  {
    initialize(&inp); 
  }
  AzDmat & operator =(const AzDmat &inp) {
    if (this == &inp) return *this; 
    _release(); 
    initialize(&inp); 
    return *this; 
  }

  AzDmat(const AzReadOnlyMatrix *inp) /* copy */
           : row_num(0), col_num(0), column(NULL), isLocked(false)  {
    initialize(inp); 
  }
  void set(const AzReadOnlyMatrix *inp) {
    _release(); 
    initialize(inp); 
  }

  AzDmat(AzFile *file) 
           : row_num(0), col_num(0), column(NULL), isLocked(false)  {
    _read(file); 
  }
  ~AzDmat() {}

  void reset() {
    _release(); 
  }

  inline void lock() {
    isLocked = true; 
  }
  inline void unlock() {
    isLocked = false; 
  }

  void resize(int new_col_num); 
  void resize(int new_row_num, int new_col_num); 
  void reform(int row_num, int col_num); 

  void destroy() {
    _release(); 
  }

  /*--------------------------------------------------*/
  int write(AzFile *file); 
  void read(AzFile *file) {
    _release(); 
    _read(file); 
  }

  void convert(AzSmat *m_out); 

  void transpose(AzDmat *m_out, int col_begin = -1, int col_end = -1); 
  void transpose_from(const AzSmat *m_inp); 

  void cut(double min_val); 

  void set(int row, int col, double val); 
  void add(int row, int col, double val); 
  void add(const AzDmat *inp, double coeff=1); 
  void add(const AzSmat *inp, double coeff=1); 

  void multiply(int row, int col, double val); 
  void multiply(double val); 

  double get(int row, int col) const; 

  /*---  this never returns NULL  ---*/
  inline AzDvect *col_u(int col) {
    if (col < 0 || col >= col_num) {
      throw new AzException("AzDmat::col_u", "col# is out of range"); 
    }
    if (column[col] == NULL) {
      column[col] = new AzDvect(row_num); 
    }
    return column[col]; 
  }

  /*---  this never returns NULL ---*/
  inline const AzDvect *col(int col) const {
    if (col < 0 || col >= col_num) {
      throw new AzException("AzDmat::col", "col# is out of range"); 
    }
    if (column[col] == NULL) {
      if (dummy_zero.rowNum() != row_num) {
        throw new AzException("AzDmat::col", "wrong dummy_zero"); 
      }
      return &dummy_zero; 
    }
    return column[col]; 
  }

  inline int rowNum() const { return row_num; }
  inline int colNum() const { return col_num; }

  void normalize(); 
  void normalize1(); 

  void binarize(); 
  void binarize1(); 

  bool isZero() const; 
  bool isZero(int col) const; 

  void zeroOut(); 

  double max(int *out_row, int *out_col) const; 

  void dump(const AzOut &out, const char *header, 
            const AzStrArray *sp_row = NULL, const AzStrArray *sp_col = NULL, 
            int cut_num = -1) const; 

  void scale(const AzSvect *vect1, bool isInverse=false); 
  void scale(const AzDvect *vect1, bool isInverse=false); 

  void square(); 

  inline void load(int col, const AzIFarr *ifa_row_val) {
    col_u(col)->load(ifa_row_val); 
  }

protected:
  inline void checkLock(const char *who) {
    if (isLocked) {
      throw new AzException("AzDmat::checkLock", 
            "Illegal attempt to change the pointers of a locked matrix by", who); 
    }
  }
  void _read(AzFile *file); 

  /*---  call _release() before calling initialize(...)  ---*/
  /*---  dimensionality may be changed  ---*/
  void initialize(int row_num, int col_num); 
  void initialize(const AzDmat *inp); 
  void initialize(const AzReadOnlyMatrix *inp); 

  void _transpose(AzDmat *m_out, int col_begin, int col_end); 
}; 

#endif 


