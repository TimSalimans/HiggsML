/* * * * *
 *  AzSvDataR.hpp 
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

#ifndef _AZ_SV_DATA_S_HPP_
#define _AZ_SV_DATA_S_HPP_

#include "AzUtil.hpp"
#include "AzSmat.hpp"
#include "AzDmat.hpp"
#include "AzStrPool.hpp"
#include "AzSvFeatInfo.hpp"

/* S for separation of features and targets */
class AzSvDataS : public virtual AzSvFeatInfo /* feature template */
{
protected: 
  AzSmat m_feat; 
  AzStrPool sp_f_dic; 
  AzDvect v_y; 

public: 
  inline const AzSmat *feat() const {
    checkIfReady("feat"); 
    return &m_feat; 
  }

  inline const AzDvect *targets() const {
    checkIfReady("targets"); 
    return &v_y;  
  }
  inline const AzSvFeatInfo *featInfo() const { return this; }
  inline int featNum() const { return m_feat.rowNum(); }

  inline int size() const { return m_feat.colNum(); }

  void concatDesc(int ex, AzBytArr *str_desc) const; 

  inline 
  virtual void read(const char *feat_fn, 
                    const char *y_fn=NULL, 
                    const char *fdic_fn=NULL) {
    if (y_fn == NULL || strlen(y_fn) == 0) read_features_only(feat_fn, fdic_fn); 
    else                                  _read(feat_fn, y_fn, fdic_fn); 
  }
  virtual void read_features_only(const char *feat_fn, 
                                  const char *fdic_fn=NULL); 
  virtual void read_targets_only(const char *y_fn); 
  void destroy(); 

  /*---  static tools  ---*/
  static void readMatrix(const char *fn, 
                         AzSmat *m_data) {
    AzSvDataS dataset; 
    dataset.read_features_only(fn); 
    m_data->set(dataset.feat()); 
  }

  static void readVector(const char *fn, 
                         /*---  output  ---*/
                         AzDvect *v_data) {
    read_target(fn, v_data); 
  }
  static void mergeData(int argc, 
                         const char *argv[]); 
  static void mergeData(const char *x_fn, 
                         const char *x_names_fn, 
                         const char *fn_template, 
                         const char *str, 
                         bool doSparse, 
                         int digits, 
                         const char *out_x_fn, 
                         const char *out_n_fn, 
                         int num,
                         const char *names[]); 

  static void mergeData(const AzSmat *m_x, 
                         const AzSvFeatInfo *feat, 
                         const char *fn_template, 
                         const char *str, 
                         bool doSparse, 
                         int digits, 
                         const char *out_x_fn, 
                         const char *out_n_fn,
                         int num,
                         const char *names[]); 

protected:
  virtual void _read(const char *feat_fn, 
                    const char *y_fn, 
                    const char *fdic_fn=NULL);
  void checkIfReady(const char *msg) const; 
  void reset(); 

  static void read_feat(const char *feat_fn, 
                          const char *fdic_fn, 
                          /*---  output  ---*/
                          AzSmat *m_feat, 
                          AzStrPool *sp_f_dic); 
  static void read_target(const char *y_fn, 
                          AzDvect *v_y); 

  static void readData(const char *data_fn, 
                         int expected_f_num, 
                         /*---  output  ---*/
                         AzSmat *m_data) {
    readData_Small(data_fn, expected_f_num, m_data); 
  }
  static void readData_Small(const char *data_fn, 
                         int expected_f_num, 
                         /*---  output  ---*/
                         AzSmat *m_data); 
  static void parseDataLine(const AzByte *inp, 
                            int inp_len, 
                            int f_num, 
                            const char *data_fn, /* for printing error */
                            int line_no, 
                            /*---  output  ---*/
                            AzSmat *m_feat, 
                            int col); 

  static int countFeatures(const AzByte *line, 
                           const AzByte *line_end);

  static void scanData(const char *data_fn, 
                         /*---  output  ---*/
                         int &out_data_num, 
                         int &out_max_len); 
  static void readData_Large(const char *data_fn, 
                         int expected_f_num, 
                         /*---  output  ---*/
                         AzSmat *m_feat); 

  /*---  For the sparse data format  ---*/
  static void parseDataLine_Sparse(const AzByte *inp, 
                              int inp_len, 
                              int f_num, 
                              const char *data_fn, 
                              int line_no, 
                              /*---  output  ---*/
                              AzSmat *m_feat, 
                              int col); 
  static void decomposeFeat(const char *token, 
                            int line_no, 
                            /*---  output  ---*/
                            int *ex, 
                            double *val); 
  static int if_sparse(AzBytArr &s_line, int expected_f_num); 
  inline static double my_atof(const char *str, 
                           const char *eyec, 
                           int line_no) {
    if (*str == '\0' || *str >= '0' && *str <= '9' || 
        *str == '+' || *str == '-') {
      return atof(str); 
    }
    AzBytArr s("Invalid number expression in line# ");
    s.cn(line_no); s.c(" of the input data file: "); s.c(str); 
    throw new AzException(AzInputError, eyec, s.c_str()); 
  }

  inline static int my_fno(const char *str, 
                           const char *eyec, 
                           int line_no) {
    if (*str >= '0' && *str <= '9' || 
        *str == '+') {
      return atol(str); 
    }
    AzBytArr s("Invalid field# expression in line# ");
    s.cn(line_no); s.c(" of the input data file: "); s.c(str); 
    throw new AzException(AzInputError, eyec, s.c_str()); 
  }
}; 

#endif 
