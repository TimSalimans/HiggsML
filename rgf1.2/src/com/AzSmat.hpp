/* * * * *
 *  AzSmat.hpp 
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

#ifndef _AZ_SMAT_HPP_
#define _AZ_SMAT_HPP_

#include "AzUtil.hpp"
#include "AzStrArray.hpp"
#include "AzReadOnlyMatrix.hpp"

/* Changed AZ_MTX_FLOAT from single-precision to double-precision  */
typedef double AZ_MTX_FLOAT; 
#define _checkVal(x) 
/* static double _too_large_ = 16777216; */
/* static double _too_small_ = -16777216; */

typedef struct {
  int no; 
  AZ_MTX_FLOAT val; 
} AZI_VECT_ELM; 

class AzSmat; 

//! sparse vector 
class AzSvect : /* implements */ public virtual AzReadOnlyVector {
protected:
  int row_num; 
  AZI_VECT_ELM *elm; 
  AzBaseArray<AZI_VECT_ELM> a; 
  int elm_num; 

  void _release() {
    a.free(&elm); elm_num = 0; 
  }

public:
  AzSvect() : row_num(0), elm(NULL), elm_num(0) {}
  AzSvect(int inp_row_num, bool asDense=false) : row_num(0), elm(NULL), elm_num(0) {
    initialize(inp_row_num, asDense); 
  }
  AzSvect(const AzSvect *inp) : row_num(0), elm(NULL), elm_num(0) {
    row_num = inp->row_num;  
    set(inp); 
  }
  AzSvect(const AzSvect &inp) : row_num(0), elm(NULL), elm_num(0) {
    row_num = inp.row_num; 
    set(&inp); 
  }
  AzSvect & operator =(const AzSvect &inp) {
    if (this == &inp) return *this; 
    _release(); 
    row_num = inp.row_num; 
    set(&inp); 
    return *this; 
  }
  AzSvect(const AzReadOnlyVector *inp) : row_num(0), elm(NULL), elm_num(0) {
    row_num = inp->rowNum();  
    set(inp); 
  }
  AzSvect(AzFile *file) : row_num(0), elm(NULL), elm_num(0) {
    _read(file); 
  }
  ~AzSvect() {}

  void read(AzFile *file) {
    _release(); 
    _read(file); 
  }

  void resize(int new_row_num); /* new #row must be greater than #row */
  void reform(int new_row_num, bool asDense=false); 

  int write(AzFile *file); 

  inline int rowNum() const { return row_num; }  
  void load(const AzIntArr *ia_row, double val); 
  void load(AzIFarr *ifa_row_val); 
  bool isZero() const; 

  void cut(double min_val); 
  void nonZero(AzIFarr *ifq, const AzIntArr *ia_sorted_filter) const; 

  void filter(const AzIntArr *ia_sorted, /* must be sorted; can have duplications */
              /*---  output  ---*/
              AzIFarr *ifa_nonzero, 
              AzIntArr *ia_zero) const; 

  int nonZeroRowNo() const; /* returns the first one */
  void nonZero(AzIFarr *ifq) const; 
  void nonZeroRowNo(AzIntArr *intq) const; 
  int nonZeroRowNum() const; 

  void set_inOrder(int row_no, double val); 
  void set(int row_no, double val); 
  void set(double val); 
  void set(const AzSvect *vect1, double coefficient=1); 
  void set(const AzReadOnlyVector *vect1, double coefficient=1);  

  double get(int row_no) const; 

  double sum() const; 
  double absSum() const; 

  void add(int row_no, double val); 

  void multiply(int row_no, double val); 
  void multiply(double val); 

  void scale(const double *vect1); 

  double selfInnerProduct() const; 

  double normalize(); 
  double normalize1(); 

  void clear(); 
  void zeroOut(); 

  int next(AzCursor &cursor, double &out_val) const; 

  double minPositive(int *out_row_no = NULL) const; 
  double min(int *out_row_no = NULL, 
                     bool ignoreZero=false) const; 
  double max(int *out_row_no = NULL, 
                    bool ignoreZero=false) const; 
  double maxAbs(int *out_row_no = NULL, double *out_real_val = NULL) const; 

  void dump(const AzOut &out, const char *header, 
            const AzStrArray *sp_row = NULL, 
            int cut_num = -1) const; 

  void clear_prepare(int num); 
  bool isSame(const AzSvect *inp) const; 
  void cap(double cap_val); 

protected:
  void _read(AzFile *file); 
  void _swap(); 

  void initialize(int inp_row_num, bool asDense); 
  int to_insert(int row_no); 
  int find(int row_no, int from_this = -1) const; 
  int find_forRoom(int row_no, 
                   int from_this = -1, 
                    bool *out_isFound = NULL) const; 

  void _dump(const AzOut &out, const AzStrArray *sp_row, 
             int cut_num = -1) const; 

  inline int inc() const {
    return MIN(4096, MAX(32, elm_num)); 
  }
}; 

//! sparse matrix 
class AzSmat : /* implements */ public virtual AzReadOnlyMatrix {
protected:
  int col_num, row_num; 
  AzSvect **column; /* NULL if and only if col_num=0 */
  AzObjPtrArray<AzSvect> a; 
  AzSvect dummy_zero; 
  void _release() {
    a.free(&column); col_num = 0; 
    row_num = 0; 
  }
public: 
  AzSmat() : col_num(0), row_num(0), column(NULL) {}
  AzSmat(int inp_row_num, int inp_col_num, bool asDense=false)
    : col_num(0), row_num(0), column(NULL) {
    initialize(inp_row_num, inp_col_num, asDense); 
  }
  AzSmat(const AzSmat *inp) : col_num(0), row_num(0), column(NULL) {
    initialize(inp); 
  }
  AzSmat(const AzSmat &inp) : col_num(0), row_num(0), column(NULL) {
    initialize(&inp); 
  }
  AzSmat & operator =(const AzSmat &inp) {
    if (this == &inp) return *this; 
    _release(); 
    initialize(&inp); 
    return *this; 
  }

  AzSmat(AzFile *file) : col_num(0), row_num(0), column(NULL) {
    _read(file); 
  }
  ~AzSmat() {}
  void read(AzFile *file) {
    _release(); 
    _read(file); 
  }
  inline void reset() {
    _release(); 
  }
  void resize(int new_col_num); 
  void resize(int new_row_num, int new_col_num); /* new #row must be greater than #row */
  void reform(int row_num, int col_num, bool asDense=false); 

  int write(AzFile *file); 

  bool isZero() const; 
  bool isZero(int col_no) const; 

  int nonZeroColNum() const; 
  double nonZeroNum(double *ratio) const; 

  void transpose(AzSmat *m_out, int col_begin = -1, int col_end = -1) const; 
  void cut(double min_val); 

  void set(const AzSmat *inp); 
  void set(const AzReadOnlyMatrix *inp);  
  void set(int row_no, int col_no, double val); 
  void set(double val); 

  void add(int row_no, int col_no, double val); 

  void multiply(int row_no, int col_no, double val); 
  void multiply(double val); 

  double get(int row_no, int col_no) const; 

  /*---  this never returns NULL  ---*/
  inline const AzSvect *col(int col_no) const {
    if (col_no < 0 || col_no >= col_num) {
      throw new AzException("AzSmat::col", "col# is out of range"); 
    }
    if (column[col_no] == NULL) {
      if (dummy_zero.rowNum() != row_num) {
        throw new AzException("AzSmat::col", "#col of dummy_zero is wrong"); 
      }
      return &dummy_zero; 
    }
    return column[col_no]; 
  }

  /*---  this never returns NULL  ---*/
  inline AzSvect *col_u(int col_no) {
    if (col_no < 0 || col_no >= col_num) {
      throw new AzException("AzSmat::col_u", "col# is out of range"); 
    }
    if (column[col_no] == NULL) {
      column[col_no] = new AzSvect(row_num); 
    }
    return column[col_no]; 
  }
  inline int rowNum() const { return row_num; }
  inline int colNum() const { return col_num; }

  void normalize(); 
  void normalize1(); 

  inline void destroy() {
    reform(0,0); 
  }
  inline void destroy(int col) {
    if (col >= 0 && col < col_num && column != NULL) {
      delete column[col]; 
      column[col] = NULL; 
    } 
  }

  inline void load(int col, AzIFarr *ifa_row_val) {
    col_u(col)->load(ifa_row_val); 
  }

  void clear(); 
  void zeroOut(); 

  int next(AzCursor &cursor, int col, double &out_val) const; 

  double max(int *out_row, int *out_col, 
                    bool ignoreZero=false) const;
  double min(int *out_row, int *out_col, 
                     bool ignoreZero=false) const; 

  void dump(const AzOut &out, const char *header, 
            const AzStrArray *sp_row = NULL, const AzStrArray *sp_col = NULL, 
            int cut_num = -1) const; 

  bool isSame(const AzSmat *inp) const; 

  static void concat(AzSmat *m_array[], 
                     int m_num, 
                     int col_num, 
                     /*---  output  ---*/
                     AzSmat *m_out,  /* not initialized */
                     bool destroyInput=false); 

  inline static bool isNull(const AzSmat *inp) {
    if (inp == NULL) return true; 
    if (inp->col_num == 0) return true; 
    if (inp->row_num == 0) return true; 
    return false; 
  }
  void cap(double cap_val); 

  void rbind(const AzSmat *m1); 
  void cbind(const AzSmat *m1); 

protected:
  void _read(AzFile *file); 
  void initialize(int row_num, int col_num, bool asDense); 
  void initialize(const AzSmat *inp); 
  void _transpose(AzSmat *m_out, int col_begin, int col_end) const; 
}; 

#endif 

