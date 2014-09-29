/* * * * *
 *  AzTools.hpp 
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

#ifndef _AZ_TOOLS_HPP_
#define _AZ_TOOLS_HPP_

#include "AzUtil.hpp"
#include "AzSmat.hpp"
#include "AzDmat.hpp"
#include "AzStrPool.hpp"
#include "AzPrint.hpp"

class AzTools 
{
public: 
  static int writeList(const char *fn, 
                       const AzStrArray *sp_list); 
  static void readList(const char *fn, 
                       AzStrPool *sp_list); 
  static void flatten(const AzIFarr *ifa_dx_val, 
                       /*---  output  ---*/
                       AzIntArr *ia_index, 
                       AzDvect *v_value); 

  /*--- (NOTE) This skips preceding white characters.  ---*/
  static const AzByte *getString(const AzByte **wpp, 
                                   const AzByte *data_end, 
                                   int *byte_len); 

  static void getString(const AzByte **wpp, 
                        const AzByte *data_end, 
                        AzBytArr *string)
  {
    int str_len; 
    const AzByte *str = getString(wpp, data_end, &str_len); 
    string->concat(str, str_len); 
  }

  /*-----  -----*/
  static char *chomp(AzByte *inp, int inp_len); 

  /*---  (NOTE) This doesn't skip preceding white characters.  ---*/
  static const AzByte *getString(const AzByte **wpp, const AzByte *data_end, 
                                   AzByte wdlm, int *byte_len); 
  static void getString(const AzByte **wpp, const AzByte *data_end, 
                        AzByte wdlm, AzBytArr *string) 
  {
    int str_len; 
    const AzByte *str = getString(wpp, data_end, wdlm, &str_len); 
    string->concat(str, str_len); 
  }

  static void getStrings(const AzByte *data, int data_len, 
                         AzStrPool *sp_tokens); 

  static void getStrings(const AzByte *data, int len, 
                         AzByte dlm, 
                         AzStrPool *sp_out); /* output */
  inline static void getStrings(const char *str, AzByte dlm, AzStrPool *sp_out) {
    if (str == NULL) return; 
    getStrings((AzByte *)str, Az64::cstrlen(str), dlm, sp_out); 
  }
  static void getStrings(const AzByte *data, const AzByte *data_end, 
                         AzByte dlm, 
                         AzDataArray<AzBytArr> *aStr_out); /* output */
  inline static void getStrings(const char *str, AzByte dlm, 
                                AzDataArray<AzBytArr> *aStr_out) /* output */
  {
    if (str == NULL) return; 
    getStrings((AzByte *)str, (AzByte *)(str + strlen(str)), dlm, aStr_out); 
  }

  static int getNumber(const AzByte **wpp, const AzByte *data_end, 
                        AzByte wdlm); 

  static void shuffle(int rand_seed, 
                      AzIntArr *iq, 
                      bool withReplacement = false); 

  static void shuffleFile(const char *fn, 
                         int buff_size,               
                         int random_seed, 
                         const char *out_fn); 

  static inline int big_rand() {
    return (rand() % 32768) * 32768 + (rand() % 32768); 
  }

  static bool isSpecified(const char *input) {
    if (input == NULL || strlen(input) == 0) {
      return false; 
    }
    if (*input == '_' && strlen(input) == 1) return false; 
    return true;   
  }

  static void strip(const AzByte *inp_str, int inp_len, 
                    AzBytArr *str_out) { /* output */
    int out_len; 
    const AzByte *out_str = strip(inp_str, inp_str + inp_len, &out_len);
    str_out->concat(out_str, out_len); 
  }

  static const AzByte *strip(const AzByte *data, const AzByte *data_end,  
                               int *byte_len); 

  inline static int rand_large() {
    int rand_max = RAND_MAX; 
    if (rand_max <= 0x7fff) {
      return rand() * (rand_max + 1) + rand(); 
    }
    return rand(); 
  }

  /*--------*/
  inline static void turnOn(bool *swch, const char *param, char mark) {
    if (param == NULL) return; 
    if (strchr(param, mark) != NULL) {
      *swch = true; 
    }
  }
  inline static void turnOff(bool *swch, const char *param, char mark) {
    if (param == NULL) return; 
    if (strchr(param, mark) != NULL) {
      *swch = false; 
    }
  }

  inline static void parseFloat(const char *param, 
                                char mark, 
                                double *out_value) {
    if (param == NULL) return; 
    const char *mark_str = strchr(param, mark); 
    if (mark_str != NULL) {
      *out_value = atof(mark_str+1); 
    }
  }
  inline static void parseInt(const char *param, 
                              char mark, 
                              int *out_value) {
    if (param == NULL) return; 
    const char *mark_str = strchr(param, mark); 
    if (mark_str != NULL) {
      *out_value = atol(mark_str+1); 
    }
  }

  static void formatRvector(const AzSvect *v, 
                            const char *dlm, 
                            AzBytArr *s, /* appended */
                            int exclude=-1); 
  static void formatRvector(const AzDvect *v, 
                            const char *dlm, 
                            AzBytArr *s);  /* appended */

  /*-----*/
  static void filter_exclude(const AzIntArr *ia_fxs, 
                              AzSmat *m_x);  /* inout: shape doesn't change  */
  static void filter_include(const AzIntArr *ia_fxs, 
                             AzSmat *m_x);  /* inout: shape doesn't change */
  static void filter_include(const AzIntArr *ia_fxs, 
                             const AzSmat *m_x, 
                             AzSmat *m_out);  /* output: includes the chosen rows only */

  static void pickData(AzSmat *m_inout, 
                       const AzIntArr *ia_cols); 
  static void pickData(AzDvect *v_inout, 
                       const AzIntArr *ia_cols);
}; 

#endif 
