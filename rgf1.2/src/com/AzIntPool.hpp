/* * * * *
 *  AzIntPool.hpp 
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

#ifndef _AZ_INT_POOL_HPP_
#define _AZ_INT_POOL_HPP_

#include "AzUtil.hpp"

class AzIpEnt {
public:
  int offs; 
  const int *ints; 
  int num; 
  int count; 
  int value; 
  AzIpEnt() {
    offs = 0; 
    ints = NULL; 
    num = count = 0; 
    value = -1; 
  }
}; 

//! Store integer arrays.  Searchable after committed.  
class AzIntPool {
protected: 
  AzIpEnt *ent; 
  AzBaseArray<AzIpEnt> a_ent; 
  int ent_num; 

  AzBaseArray<int> a_data; 
  int *data; 
  int data_num; 

  bool isCommitted;  

public:
  AzIntPool() : ent(NULL), ent_num(0), data(NULL), data_num(0), isCommitted(true) {}
  AzIntPool(AzFile *file)
              : ent(NULL), ent_num(0), data(NULL), data_num(0), isCommitted(true) {
    _read(file); 
  }
  AzIntPool(const AzIntPool *inp) /* copy */
              : ent(NULL), ent_num(0), data(NULL), data_num(0), isCommitted(true) {
    reset(inp); 
  }
  ~AzIntPool() {}

  void reset() {
    a_ent.free(&ent);   ent_num = 0; 
    a_data.free(&data); data_num = 0; 
    isCommitted = true; 
  }
  void reset(const AzIntPool *ip); 

  void read(AzFile *file) {
    reset(); 
    _read(file); 
  }

  int write(AzFile *file); 

  inline void update(int ex, 
              const AzIntArr *iq, 
              int count=1,
              int val=-1) {
    update(ex, iq->point(), iq->size(), count, val); 
  }
  void update(int ex, 
              const int *ints,         
              int ints_num, 
              int count=1,
              int val=-1); 

  int put(const int *ints, int ints_num, 
          int count=1, 
          int value=-1); 
  int put(const AzIntArr *intq, int 
          count=1) {
    return this->put(intq->point(), intq->size(), count); 
  }

  /*---  put with a value  ---*/
  int putv(const AzIntArr *intq, 
           int value) {
    int count = 1; 
    return this->put(intq->point(), intq->size(), count, value); 
  }

  inline int getValue(int idx) const {
    checkRange(idx, "AzIntPool::getValue"); 
    return ent[idx].value; 
  }

  void commit();    
  int size() const {return ent_num;}
  const int *point(int ent_no, int *out_ints_num=NULL) const; 
  int getCount(int ent_no) const; 
  void setCount(int ent_no, int new_count); 
  int find(const int *ints, int ints_num) const; 
  int find(const AzIntArr *intq) const {
    return find(intq->point(), intq->size()); 
  }

  void get(int ent_no, AzIntArr *intq) const
  {
    int num; 
    const int *ints = point(ent_no, &num); 
    intq->concat(ints, num); 
  }

  inline void erase(int ent_no) {
    shorten(ent_no, 0); 
  }
  void shorten(int ent_no, int new_len); 

  void dump(const AzOut &, const char *header) const; 

  bool isThisCommitted() const {
    return isCommitted; 
  }

  inline void clear() {
    reset(); 
  }

  void concat(const AzIntPool *inp) {
    int num = inp->size(); 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      int len; 
      const int *ints = inp->point(ix, &len); 
      int count = inp->getCount(ix); 
      int value = inp->getValue(ix); 
      put(ints, len, count, value); 
    }
  }

  /*---  prohibit assign operator  ---*/
  AzIntPool(const AzIntPool &) {
    throw new AzException("AzIntPool =", "no support"); 
  }
  AzIntPool & operator =(const AzIntPool &inp) {
    if (this == &inp) return *this; 
    throw new AzException("AzIntPool =", "no support"); 
  }


protected:
  void _swap(); 
  void _read(AzFile *file); 

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
