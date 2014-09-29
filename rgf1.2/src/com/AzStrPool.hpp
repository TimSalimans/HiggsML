/* * * * *
 *  AzStrPool.hpp 
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

#ifndef _AZ_STR_POOL_HPP_
#define _AZ_STR_POOL_HPP_

#include "AzUtil.hpp"
#include "AzStrArray.hpp"

typedef struct {
public:
  int offs; 
  int len; 
  int count; 
  int value; 
  const AzByte *bytes; /* we need this for qsort */
} AzSpEnt; 

typedef struct {
  int begin; 
  int end; 
  int min_len; 
  int max_len; 
} AzSpIndex; 

//! Store byte arrays or strings.  Searchable after committed. 
class AzStrPool : public virtual AzStrArray {
public:
  AzStrPool(); 
  AzStrPool(int init_num, int avg_data_len); 
  AzStrPool(const AzStrPool *inp_sp); /* copy */
  AzStrPool(const AzStrArray *inp_sp); /* copy */
  AzStrPool(AzFile *file); 
  ~AzStrPool() {}

  void reset(); 
  inline void reset(int init_num, int avg_data_len) {
    reset(); 
    init_ent_num = MAX(init_num, 64); 
    init_data_len = init_ent_num * MAX(1,avg_data_len); 
  }

  void read(AzFile *file) {
    reset(); 
    _read(file); 
  }

  int write(AzFile *file); 

  void reset(const AzStrPool *inp) {
    reset(); 
    _copy(inp); 
  }

  void copy(AzStrArray *sp2) {
    reset(); 
    _copy(sp2); 
  }

  int put(const AzByte *bytes, int bytes_len, 
          int count=1, 
          int value=-1); 
  int put(const char *str, int count=1) {
    return put((AzByte *)str, Az64::cstrlen(str), count);     
  }
  int put(const AzBytArr *byteq, int count=1) {
    return put(byteq->point(), byteq->getLen(), count); 
  }

  inline int putv(const AzBytArr *bq, int value) {
    int count = 1; 
    return put(bq->point(), bq->getLen(), count, value); 
  }        
  inline int putv(const char *str, int value) {
    int count = 1; 
    return put((AzByte *)str, Az64::cstrlen(str), count, value); 
  }  
  inline int getValue(int ent_no) const {
    checkRange(ent_no, "AzStrPool::getValue"); 
    return ent[ent_no].value; 
  }
  inline void setValue(int ent_no, int value) {
    checkRange(ent_no, "AzStrPool::setValue"); 
    ent[ent_no].value = value; 
  }

  void setCount(int ent_no, int count) {
    checkRange(ent_no, "AzStrPool::setCount"); 
    ent[ent_no].count = count; 
  }

  void put(AzDataArray<AzBytArr> *aStr) {
    int num = aStr->size(); 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      put(aStr->point(ix)); 
    }
  }
  void put(const AzStrPool *inp) {
    int num = inp->size(); 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      int len; 
      const AzByte *str = inp->point(ix, &len); 
      int count = inp->getCount(ix); 
      int value = inp->getValue(ix); 
      put(str, len, count, value); 
    }
  }

  void commit(); 
  void build_index(); 
  int size() const { return ent_num; }

  const AzByte *point(int ent_no, int *out_len) const; 
  const AzByte *point(int ent_no) const; 
  const char *c_str(int ent_no) const {
    return (char *)point(ent_no); 
  }
  int getLen(int ent_no) const; 
  int getCount(int str_no) const; 

  void removeEntry(int ent_no); 
  void removeEntries(const AzIntArr *ia_rmv_ex) {
    if (ia_rmv_ex == NULL) return; 
    int num; 
    const int *rmv_ex = ia_rmv_ex->point(&num); 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      removeEntry(rmv_ex[ix]); 
    }
  }

  int find(const AzByte *bytes, int bytes_len) const;  
  int find(const AzBytArr *byteq) const {
    return find(byteq->point(), byteq->getLen()); 
  }
  int find(const char *str) const {
    return find((AzByte *)str, Az64::cstrlen(str)); 
  }

  void dump(const AzOut &file, const char *header) const; 

  void get(int ent_no, AzBytArr *byteq) const 
  {
    int len; 
    const AzByte *bytes = point(ent_no, &len); 
    byteq->concat(bytes, len); 
  }

  bool isThisSearchReady() const {
    return isCommitted; 
  }
  inline bool isThisCommitted() const {
    return isCommitted; 
  }

  void reduce(int min_count); 

  /*---  prohibit assign operator  ---*/
  AzStrPool(const AzStrPool &) {
    throw new AzException("AzStrPool =", "no support"); 
  }
  AzStrPool & operator =(const AzStrPool &inp) {
    if (this == &inp) return *this; 
    throw new AzException("AzStrPool =", "no support"); 
  }

protected:
  AzSpEnt *ent; 
  AzBaseArray<AzSpEnt> a_ent; 
  int ent_num; 

  AzByte *data; 
  AzBaseArray<AzByte> a_data; 
  int data_len; 

  bool isCommitted;  

  AzSpIndex *my_idx; 
  AzBaseArray<AzSpIndex> a_index; 

  int init_ent_num, init_data_len; 

  void _swap(); 
  void initialize(); 
  void _read(AzFile *file); 
  void _copy(const AzStrPool *sp2); 
  void _copy(const AzStrArray *sp2); 
  int genIndexKey(const AzByte *bytes, int bytes_len) const; 

  int inc_ent(); 
  int inc_data(int min_inc); 

  inline void checkRange(int ent_no, const char *eyec) const 
  {
    if (ent_no < 0 || ent_no >= ent_num) {
      throw new AzException(eyec, "out of range"); 
    }
  }


}; 
#endif 
