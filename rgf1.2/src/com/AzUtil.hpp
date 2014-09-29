/* * * * *
 *  AzUtil.hpp 
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

#ifndef _AZ_UTIL_HPP_ 
#define _AZ_UTIL_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <new>

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std; 

#include "AzException.hpp"
#include "AzMemTempl.hpp"
#include "AzOut.hpp"

typedef unsigned char AzByte; 

/*---  binary files are written in little-endian  ---*/
#define AzBigEndian_isSwapNeeded    true
#define AzLittleEndian_isSwapNeeded false

#ifndef _AZ_MAIN_
  /*---  not main  ---*/
  extern bool isSwapNeeded; 
  extern AzOut log_out; 
  extern AzOut dmp_out; 
  /*---  end of not main  ---*/
#else
  /*---  main  ---*/
  AzOut log_out(&cout); 
  AzOut dmp_out; 

#ifdef _AZ_BIG_ENDIAN_
  bool isSwapNeeded = AzBigEndian_isSwapNeeded; 
#else
  bool isSwapNeeded = AzLittleEndian_isSwapNeeded; 
#endif

  /**
   *  For making binary files compatible among different machines: 
   *     - Files are always written in little-endian.  This means that big-endian machines  
   *       must swap the byte order before writing and after reading.  
   *     - The byte-swap macros assume certain variable lengths, for speed.  
   */
  void Az_check_system_() {
    if (sizeof(int) != 4 || sizeof(double) != 8 || sizeof(bool) != 1) {
      throw new AzException("_check_system_", 
            "expecting sizeof(int)=sizeof(bool)=1,sizeof(double)=8");
    }
    /*---  if endian is specified through define, respect that  ---*/
    #ifndef _AZ_BIG_ENDIAN_
    #ifndef _AZ_LITTLE_ENDIAN_
    /*---  otherwise, check endian  ---*/
    const int int_val = 1; 
    const unsigned char *ch = (unsigned char *)(&int_val); 
    if (*ch == 1); /* little endian */
    else if (*(ch+3) == 1) { /* big endian */
      const char *msg = "Detected a big-endian machine.  Binary files will not be compatible with little-endian systems.  To remove this warning, compile with \"/D_AZ_BIG_ENDIAN_\""; 
      printf("%s\n", msg); 
    }
    #endif
    #endif
  }
  /*---  end of main  ---*/
#endif 

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define ABS(x) (((x)>=0)?(x):((-1)*(x)))

#define AzNone (-1)

/*-----------------------------------------------------*/

#define AzSigned32Max 2147483647

class Az64 {
public:
  inline static int cstrlen(const char *str, const char *eyec="") {
    size_t len = strlen(str); 
    if (len > AzSigned32Max) {
      throw new AzException("Az64::strlen", eyec, "Internal string is too large (over 2GB).");
    }
    return (int)len; 
  }
  inline static int ptr_diff(ptrdiff_t inp, const char *eyec="") {
    if (inp > AzSigned32Max) {
      throw new AzException("Az64::ptr_diff", eyec, "Internal data is too large (over 2GB).");
    }
    return (int)inp; 
  }
}; 

class AzBytArr; 

/*-----------------------------------------------------*/
/*                      AzFile                         */
/*-----------------------------------------------------*/
//! File I/O handler 
class AzFile {
private: 
  FILE *fp; 
  AzBytArr *str_fn; 

public: 
  AzFile() : fp(NULL), str_fn(NULL) {}
  AzFile(const char *fn); 
  ~AzFile(); 

  void reset(const char *fn); 
  void open(const char *flags); 
  void close(bool doCheckCloseError=false); 
  static bool isExisting(const char *fn); 

  AzByte *readBytes(int len);  
  int writeBytes(const void *buff, int len); 

  int gets(AzByte *buff, int buffsize); 
  void seekReadBytes(int offs, int len, void *buff); 
  void seek(int offs); 

  int getOffset() const; 
  int size(); 

  const char *pointFileName() const; 
   
  inline int writeByte(AzByte byte_val) {
    return writeBytes(&byte_val, sizeof(byte_val)); 
  }

  inline int writeInt(int long_val) {
    swap_long(&long_val); 
    return writeBytes(&long_val, sizeof(long_val)); 
  }
  inline int writeBool(bool bool_val) {
    swap_bool(&bool_val);
    return writeBytes(&bool_val, sizeof(bool_val)); 
  }
  inline int writeDouble(double double_val) {
    swap_double(&double_val); 
    return writeBytes(&double_val, sizeof(double_val)); 
  }

  inline AzByte readByte() {
    AzByte byte_val; 
    seekReadBytes(-1, sizeof(byte_val), &byte_val); 
    return byte_val; 
  }
  inline int readInt() {
    int long_val; 
    seekReadBytes(-1, sizeof(long_val), &long_val); 
    swap_long(&long_val); 
    return long_val; 
  }
  inline bool readBool() {
    bool bool_val; 
    seekReadBytes(-1, sizeof(bool_val), &bool_val); 
    swap_bool(&bool_val); 
    return bool_val; 
  }
  inline double readDouble() {
    double double_val; 
    seekReadBytes(-1, sizeof(double_val), &double_val); 
    swap_double(&double_val); 
    return double_val; 
  }

  inline FILE *ptr() {
    return fp; 
  }

  void flush() {
    if (fp != NULL) {
      fflush(fp); 
    }
  }

  AzFile & operator =(const AzFile &inp) {
    if (this == &inp) return *this; 
    throw new AzException("AzFile =", "copying AzFile is prohibited"); 
  }

  int writeBinMarker(); 
  void checkBinMarker(); 

  /*---  for endian  ---*/
  inline static void swap_long(int *x) {
    if (!isSwapNeeded) return; 
    _swap((AzByte *)x, 0,3); 
    _swap((AzByte *)x, 1,2); 
  }
  inline static void swap_bool(bool *x) {}
  inline static void swap_double(double *x) {
    if (!isSwapNeeded) return; 
    _swap((AzByte *)x, 0,7); 
    _swap((AzByte *)x, 1,6); 
    _swap((AzByte *)x, 2,5); 
    _swap((AzByte *)x, 3,4); 
  }
protected:
  inline static void _swap(AzByte *x, int p, int q) {
    AzByte _b = *(x+p); 
    *(x+p) = *(x+q); *(x+q) = _b; 
  }
}; 

/*-----------------------------------------------------*/
class AzObjIOTools
{
public:
  template<class T>
  static int write(T *t, AzFile *file) 
  {
    int io_len = 0; 
    if (t == NULL) {
      io_len += file->writeInt(0); 
    }
    else {
      io_len += file->writeInt(1); 
      io_len += t->write(file); 
    }
    return io_len; 
  }

  template<class T>
  static T *read(AzFile *file) 
  {
    int val = file->readInt(); 
    T *ptr = NULL; 
    if (val != 0) {
      try {
        ptr = new T(file); 
      }
      catch (std::bad_alloc &ba) {
        throw new AzException(AzAllocError, "AzObjIOTools::read", "new", ba.what());       
      }
      if (ptr == NULL) {
        throw new AzException(AzAllocError, "AzObjIOTools::read", "new"); 
      }
    }
    return ptr; 
  }
}; 

/*-----------------------------------------------------*/
/*               AzIntArr (Integer array)              */
/*-----------------------------------------------------*/
//! Integer array with sort etc. 
class AzIntArr {
public:
  AzIntArr() : num(0), ints(NULL) {}
  AzIntArr(const AzIntArr *inp) : num(0), ints(NULL) {
    if (inp != NULL) reset(inp); 
  }
  AzIntArr(const AzIntArr &inp) : num(0), ints(NULL) {
    reset(&inp); 
  }
  AzIntArr(int inp_num, int initial_value) : num(0), ints(NULL) {
    initialize(inp_num, initial_value); 
  }
  AzIntArr(const int *inp_ints, int inp_ints_num) : num(0), ints(NULL) {
    initialize(inp_ints, inp_ints_num);     
  }
  void reset(const int *inp_ints, int inp_ints_num) {
    reset(); 
    initialize(inp_ints, inp_ints_num);        
  }
  AzIntArr(AzFile *file) : num(0), ints(NULL) {
    initialize(file); 
  }
  ~AzIntArr() {
    a.free(&ints); num = 0; 
  }

  int write(AzFile *file); 
  inline void read(AzFile *file) {
    reset(); 
    initialize(file); 
  }

  inline void reset() {
    a.free(&ints); num = 0; 
  }
  inline void reset_norelease() { num = 0; }
  void reset(int num, int initial_value); 
  inline void reset(const AzIntArr *inp) {
    reset(); 
    concat(inp); 
  }

  /*---  return true if there is any negaitve component.  ---*/
  bool toOnOff(const int *inp, int inp_num); 
  bool toOnOff(AzIntArr *out_iq) const {
    return out_iq->toOnOff(ints, num); 
  }
  bool toCount(AzIntArr *out_iq) const; 

  int *my_bsearch(int key); 

  void fill(int num, int first_value = 0); 
  inline void range(int range_begin, int range_end) {
    fill(range_end - range_begin, range_begin); 
  }
  double average() const; 

  inline void update(int index, int value) {
    checkIndex(index, "AzIntArr::update"); 
    ints[index] = value;
  }

  inline int get(int index) const {
    checkIndex(index, "AzIntArr::get");
    return ints[index];
  }

  void put(int int_val); 
  void increment(int index); 

  void concat(const AzIntArr *intq2); 
  void concat(const int *ints2, int ints2_num); 
  inline const int *point(int *out_num=NULL) const {
    if (out_num != NULL) *out_num = num; 
    return ints; 
  }
  inline int *point_u(int *out_num=NULL) {
    if (out_num != NULL) *out_num = num; 
    return ints;    
  }

  inline int size() const { return num; }

  void sort(bool ascending); 
  void prepare(int prep_num); 
  void unique(); 
  void cut(int new_num); 

  void remove(int idx); 
  void insert(int idx, int val); 

  static int compare(const AzIntArr *iq1, const AzIntArr *iq2, 
                         int first_k = -1); 
  inline int compare(const AzIntArr *iq2, int first_k = -1) const {
    return compare(this, iq2, first_k); 
  }
  inline int sum() const {
    int total = 0; 
    int ix; 
    for (ix = 0; ix < num; ++ix) total += ints[ix];
    return total; 
  }
  inline int max() const {
    int my_max = -1; 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      if (ix == 0 || ints[ix] > my_max) my_max = ints[ix]; 
    }
    return my_max; 
  }
  inline int min() const {
    int my_min = -1; 
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      if (ix == 0 || ints[ix] < my_min) my_min = ints[ix]; 
    }
    return my_min; 
  }

  void changeOrder(const AzIntArr *ia_old2new); 
  void print(const AzOut &out, const char *header) const; 

  int count(int int_val) const; 
  int replace(int from_val, int to_val); 
  void transfer_from(AzIntArr *inp); 

  AzIntArr & operator =(const AzIntArr &inp) {
    if (this == &inp) return *this; 
    reset(&inp); 
    return *this; 
  }

protected:
  int num; 
  int *ints;  
  AzBaseArray<int> a; 

  void _swap(); 
  void initialize(const int *ints2, int ints2_num); 
  void initialize(int num, int initial_value); 
  void initialize(const AzIntArr *inp_intq); 
  void initialize(AzFile *file);   
  void _realloc(); 
  inline void checkIndex(int index, const char *eyec) const {
	  if (index < 0 || index >= num) {
		  throw new AzException(eyec, "out of range"); 
	  }
  }
}; 

/*-----------------------------------------------------*/
/*           AzIIFarr ((Int,Int,Float) array)          */
/*-----------------------------------------------------*/
//! (Int,Int,Float)-array with sort etc. 
typedef struct {
  int int1, int2; 
  double val; 
} AzIIFarrEnt; 

enum AzIIFarrType {
  AzIIFarr_IIF = 0, AzIIFarr_II = 1, AzIIFarr_IF = 2
}; 

class AzIIFarr {
public:
  AzIIFarr() : ent_num(0), ent(NULL), ent_type(AzIIFarr_IIF) {}
  AzIIFarr(AzIIFarrType iifa_type) : ent_num(0), ent(NULL) {
    ent_type = iifa_type; 
  }
  AzIIFarr(const AzIIFarr *iifq2) : ent_num(0), ent(NULL) {
    ent_type = iifq2->ent_type;   
    concat(iifq2); 
  }
  AzIIFarr(const AzIIFarr &iifq2) : ent_num(0), ent(NULL) {
    ent_type = iifq2.ent_type;   
    concat(&iifq2); 
  }

  AzIIFarr(AzFile *file) : ent_num(0), ent(NULL), ent_type(AzIIFarr_IIF) {
    _read(file); 
  }
  ~AzIIFarr() {
    a.free(&ent); ent_num = 0; 
  }
  int write(AzFile *file);

  inline void reset(const AzIIFarr *iifq2) {
    reset(); 
    concat(iifq2); 
  } 
  void reset() {
    /* (NOTE) don't change entry type */
    a.free(&ent); ent_num = 0; 
  } 
  void prepare(int num); 
  void reset(int num, int int1, int int2, double val); 

  void insert(int index, /* inserted at [index] */
              int int1, int int2, double fval); 

  void put(int int1, int int2, double val); 
  double get(int idx, int *int1 = NULL, int *int2 = NULL) const; 
  inline int size() const { return ent_num; }

  int getNum(double req_val) const; 

  void update(int idx, int int1, int int2, double val); 

  void concat(const AzIIFarr *iifq2); 
  void concat(const AzIIFarr *iifq2, double req_val); 
  void squeeze_Sum(); 
  void squeeze_Max(); 
  void squeeze_Int1_Sum(); 
  void squeeze_Int1_Max(); 

  void sort_IntInt(bool isAscending = true); 
  void sort_Int2Int1(bool isAscending = true); 
  void sort_Float(bool isAscending = true); 
  void sort_FloatInt1Int2(bool isAscending = true); 
  int bsearch_Float(double key, bool isAscending = true) const; 

  void cut(int new_num);

  void int1(AzIntArr *iq) const; 
  void int2(AzIntArr *iq) const; 

  int find(int int1, int int2, int first_ix) const; 
  double findMin(int *out_idx = NULL) const; 
  double findMax(int *out_idx = NULL) const; 

  bool isSame(const AzIIFarr *iifq) const; 

  double sum_Fval() const; 

  AzIIFarr & operator =(const AzIIFarr &inp) {
    if (this == &inp) return *this; 
    reset(&inp); 
    return *this; 
  }

  void read(AzFile *file) {
    reset(); 
    _read(file); 
  }

  inline AzIIFarrType get_ent_type() { return ent_type; }

protected:
  AzIIFarrType ent_type; 
  int ent_num; 
  AzIIFarrEnt *ent; 
  AzBaseArray<AzIIFarrEnt> a; 

  void _read(AzFile *file);
}; 

/*-----------------------------------------------------*/
/*           AzIIarr ((Int,Int) array)                 */
/*-----------------------------------------------------*/
//! (Iint,Int)-array with sort etc. 
class AzIIarr {
public:
  AzIIarr() : iifq(AzIIFarr_II) {}
  AzIIarr(const AzIIarr *inp_iiq) : iifq(AzIIFarr_II) {
    if (inp_iiq == NULL) return; 
    iifq.concat(&inp_iiq->iifq); 
  }
  AzIIarr(const AzIIarr &inp_iiq) : iifq(AzIIFarr_II) {
    iifq.concat(&inp_iiq.iifq); 
  }

  AzIIarr(AzFile *file) {
    read(file); 
  }
  ~AzIIarr() {}

  inline void reset(const AzIIarr *iiq2) { iifq.reset(&iiq2->iifq); }
  void read(AzFile *file); 

  void reset(const int pairs[][2], int pair_num); 

  int write(AzFile *file) { return iifq.write(file); }
  void reset() { iifq.reset(); }
  void reset(int num, int int1, int int2) { 
    iifq.reset(num, int1, int2, 0); 
  }
  void prepare(int num) { iifq.prepare(num); }
  void cut(int new_num) { iifq.cut(new_num); }
  inline int size() const { return iifq.size(); }
  void put(int int1, int int2) { iifq.put(int1, int2, 0); }
  void get(int idx, int *int1, int *int2) const { iifq.get(idx, int1, int2); }
  void update(int idx, int int1, int int2) {
    iifq.update(idx, int1, int2, 0); 
  }
  void concat(const AzIIarr *p1) { iifq.concat(&p1->iifq); }
  void unique() { iifq.squeeze_Sum(); }
  void sort(bool isAscending = true) { iifq.sort_IntInt(isAscending); }
  void sort_Int2Int1(bool isAscending = true) { 
    iifq.sort_Int2Int1(isAscending);   
  }
  AzIIarr & operator =(const AzIIarr &inp) {
    if (this == &inp) return *this; 
    reset(&inp); 
    return *this; 
  }

protected:
  AzIIFarr iifq; 
}; 

/*-----------------------------------------------------*/
/*           AzIFarr ((Int,Float) array)               */
/*-----------------------------------------------------*/
//! (Int,Float)-array with sort etc. 
class AzIFarr {
public:
  AzIFarr() : iifq(AzIIFarr_IF) {}; 
  AzIFarr(bool for_iiq); 
  AzIFarr(const AzIFarr *inp_ifq) : iifq(AzIIFarr_IF) {
    if (inp_ifq == NULL) return; 
    iifq.concat(&inp_ifq->iifq); 
  }  
  AzIFarr(const AzIFarr &inp_ifq) : iifq(AzIIFarr_IF) {
    iifq.concat(&inp_ifq.iifq); 
  } 
  AzIFarr(AzFile *file) {
    read(file); 
  }
  ~AzIFarr() {}

  inline void reset(const AzIFarr *if2) { iifq.reset(&if2->iifq); }
  void read(AzFile *file); 

  inline int write(AzFile *file) {
    return iifq.write(file); 
  }
  inline void reset() { iifq.reset(); }
  inline void prepare(int num) { iifq.prepare(num); }
  inline void reset(int num, int int1, double val) { 
    iifq.reset(num, int1, AzNone, val); 
  }
  inline void put(int int1, double val) { iifq.put(int1, AzNone, val); }
  inline double get(int idx, int *int1 = NULL) const {
    return iifq.get(idx, int1, NULL); 
  }
  inline void insert(int index, int int1, double val) {
    iifq.insert(index, int1, AzNone, val); 
  }
  inline int size() const { return iifq.size(); }
  inline void update(int idx, int int1, double val) { 
    iifq.update(idx, int1, AzNone, val); 
  }
  inline void concat(const AzIFarr *if2) { iifq.concat(&if2->iifq); }

  inline void squeeze_Sum() { iifq.squeeze_Sum(); }
  inline void squeeze_Max() { iifq.squeeze_Max(); }
  inline void squeeze_Int_Sum() { iifq.squeeze_Int1_Sum(); }
  inline void squeeze_Int_Max() { iifq.squeeze_Int1_Max(); }

  inline void sort_Int(bool isAscending = true) { iifq.sort_IntInt(isAscending); }
  inline void sort_Float(bool isAscending = true) { iifq.sort_Float(isAscending); }
  inline void sort_FloatInt(bool isAscending = true) { 
    iifq.sort_FloatInt1Int2(isAscending); 
  }

  inline void cut(int new_num) { iifq.cut(new_num); }

  inline void int1(AzIntArr *ia_int1) const { iifq.int1(ia_int1); } 

  inline double findMin(int *out_idx = NULL) const { return iifq.findMin(out_idx); }
  inline double findMax(int *out_idx = NULL) const { return iifq.findMax(out_idx); }

  AzIFarr & operator =(const AzIFarr &inp) {
    if (this == &inp) return *this; 
    reset(&inp); 
    return *this; 
  }

protected:
  AzIIFarr iifq; 
}; 

/*-----------------------------------------------------*/
/*                AzBytArr (Byte array)                */
/*-----------------------------------------------------*/
//! Byte array with string functions. 
class AzBytArr {
public:
  #define AZI_BYTES_SHORT_LEN  64
  AzBytArr(); 
  AzBytArr(const AzByte *bytes, int bytes_len); 
  AzBytArr(const char *cstr); 
  AzBytArr(int number); 
  AzBytArr(const AzBytArr *inp); 
  AzBytArr(const AzBytArr &inp); 
  AzBytArr(AzFile *file); 
  ~AzBytArr(); 

  void transfer_from(AzBytArr *inp); 
  void reset(); 
  void reset(const AzBytArr *inp) {
    reset(); 
    concat(inp); 
  }

  AzByte *reset(int bytes_len, AzByte val); 

  void reset(const char *cstr) {
    reset(); 
    concat(cstr); 
  }

  int write(AzFile *file); 
  void read(AzFile *file); 

  inline int writeText(AzFile *file) const {
    int len = 0; 
    if (file != NULL) {
      len = file->writeBytes(point(), getLen()); 
    }
    return len; 
  }

  void update(int offs, AzByte new_val); 

  int getLen() const; 
  inline int length() const {
    return getLen(); 
  }
  const AzByte *point() const;/* for browsing */
  const AzByte *point(int *out_len) const; 
  AzByte *point_u(); /* for update */
  const char *c_str() const {
    char *ptr = (char *)point(); 
    if (ptr == NULL) return ""; 
    return ptr; 
  }

  inline void clear() { reset(); }
  void prepare_inc(int inc_len); 
  void concat(const AzBytArr *inp) {
    if (inp == NULL) return; 
    int bytes_len = inp->len; 
    const AzByte *bytes = inp->bytes_long; 
    if (bytes == NULL) bytes = inp->bytes_short; 
    concat(bytes, bytes_len); 
  }
  void concat(const AzByte *bytes, int len); 
  void concat(const char *cstr, int len) {
    concat((AzByte *)cstr, len); 
  }
  void concat(AzByte one_byte); 
  inline void concat(const char *cstr) {
    if (cstr == NULL) return; 
    concat((AzByte *)cstr, Az64::cstrlen(cstr)); 
  }
  void concatInt(int number, 
                 int width=-1,
                 bool doFillWithZero=false); 
  void concatFloat(double number, 
                   int precision=-1, 
                   bool doScientific=false); 
  inline void newline() {
    concat("\n"); 
  }

  /*---  short names  ---*/
  inline void c(const char *cstr) { concat(cstr); }
  inline void c(AzByte one_byte) { concat(one_byte); }
  inline void c(const AzBytArr *inp) { concat(inp); }
  inline void c(const AzByte *inp, int len) { concat(inp, len); }
  inline void cn(int number, int width=-1, bool doFillWithZero=false) {
    concatInt(number, width, doFillWithZero); 
  }
  inline void cn(double number, int precision=-1, bool doScientific=false) {
    concatFloat(number, precision, doScientific); 
  }
  inline void c(const char *cstr, int val, int width=-1, bool doFillWithZero=false) {
    concat(cstr); 
    concatInt(val, width, doFillWithZero); 
  }
  inline void c(const char *cstr, double val, int precision=-1, bool doScientific=false) {
    concat(cstr); 
    concatFloat(val, precision, doScientific); 
  }
  inline void nl() { newline(); }
  /*---------------------*/

  inline void inBrackets(int number, int width=-1, bool doFillWithZero=false) {
    concat("["); 
    concatInt(number, width, doFillWithZero); 
    concat("]"); 
  }
  inline void inQuotes(const char *str, const char *quote) {
    concat(quote); 
    concat(str); 
    concat(quote); 
  }

  void strip(); /* assume ASCII.  remove leading and tailing space */

  int compare(const AzBytArr *byteq2) const; 
  int compare(const AzByte *bytes, int bytes_len) const; 
  inline int compare(const char *cstr) const {
    return compare((AzByte *)cstr, Az64::cstrlen(cstr)); 
  }
  bool beginsWith(AzBytArr *byteq2) const;
  bool endsWith(AzBytArr *byteq2) const;
  inline bool beginsWith(const char *str2) const {
    AzBytArr str_temp = AzBytArr(str2); 
    return beginsWith(&str_temp); 
  }
  inline bool endsWith(const char *str2) const {
    AzBytArr str_temp = AzBytArr(str2); 
    return endsWith(&str_temp); 
  }
  bool contains(const char *str) const; 

  void print(const AzOut &out) const; 

  void lwr(); 

  void fill(AzByte my_byte, int len) {
    clear(); 
    int ix; 
    for (ix = 0; ix < len; ++ix) {
      concat(my_byte); 
    }
  }
  void replace(char old_char, char new_char); 
  void replace(const char *old_str, const char *new_str); 

  AzBytArr & operator =(const AzBytArr &inp) {
    if (this == &inp) return *this; 
    reset(&inp); 
    return *this; 
  }

protected:
  int len; 
  AzByte *bytes_long;  
  AzBaseArray<AzByte> a; 
  AzByte bytes_short[AZI_BYTES_SHORT_LEN + 1]; 

  void initialize(); 
  void initialize(const AzByte *bytes, int bytes_len); 
  void initialize(const AzBytArr *inp); 
  void _read(AzFile *file); 

 
  static void print(const AzByte *bytes, int bytes_len, const AzOut &out); 
}; 

/*-----------------------------------------------------*/
class AzOfs {
protected:
  ofstream ofs; 
  AzBytArr s_fn; 
public:
  inline bool is_open() {
    return ofs.is_open(); 
  }
  inline void set_to(AzOut &out) {
    out.reset(&ofs); 
  }
  inline void open(const char *fn, ios_base::openmode mode) {
    s_fn.reset(fn); 
    ofs.open(fn, mode); 
    if (ofs.fail()) {
      throw new AzException(AzFileIOError, "AzOfs::open", "Failed to open:", fn); 
    }
  }
  inline void close() {
    ofs.close(); 
    if (ofs.fail()) {
      throw new AzException(AzFileIOError, "AzOfs::close", "Failed to close:", s_fn.c_str()); 
    }
  }
}; 

/*-----------------------------------------------------*/
class AzTimeLog {
public:
  static void print(const char *msg, const AzOut &out); 
  static void print(const char *msg1, const char *msg2, const AzOut &out); 
  inline static void print(const AzBytArr &s, const AzOut &out) {
    print(s.c_str(), out); 
  }
  inline static void print(const char *msg, int number, const AzOut &out) {
    AzBytArr s; s.cn(number); 
    print(msg, s.c_str(), out); 
  }
  inline static void print(int number, const char *msg, const AzOut &out) {
    AzBytArr s; s.cn(number); 
    print(s.c_str(), msg, out); 
  }
protected:
  static void _printTime(const AzOut &out); 
}; 

/*-----------------------------------------------------*/
/* Don't use this.  Confusing.  */
template<class T>
class AzDataArray
{
public:
  AzDataArray() { 
    array = NULL; 
    cur = 0; 
  }
  AzDataArray(int num) {
    a.alloc(&array, num, "AzDataArray(num)"); 
    cur = 0; 
  }
  AzDataArray(AzFile *file) {
    int num = file->readInt(); 
    a.alloc(&array, num, "AzDataArray(file)");  
    int ix; 
    for (ix = 0; ix < num; ++ix) {
      array[ix].read(file); 
    }
    cur = 0; 
  }

  int write(AzFile *file) {
    int io_len = 0; 
    int array_len = a.size(); 
    io_len += file->writeInt(array_len); 
    int ix; 
    for (ix = 0; ix < array_len; ++ix) {
      io_len += array[ix].write(file); 
    }
    return io_len; 
  }

  void reset(int num=0) {
    a.free(&array); 
    a.alloc(&array, num, "AzDataArray::reset");     
    cur = 0; 
  }
  T *point_u(int index) {
    checkIndex(index, "point_u"); 
    return &array[index]; 
  }
  const T *point(int index) const {
    checkIndex(index, "point"); 
    return &array[index]; 
  }
  int size() const { return a.size(); }

  /*---  ---*/
  int cursor() const { return cur; }
  T *new_slot(int *out_index=NULL) {
    if (cur >= a.size()) {
      int inc = MIN(1024*1024, MAX(1024, cur)); 
      a.realloc(&array, cur+inc, "AzDataArray::new_slot"); 
    }
    if (out_index != NULL) {
      *out_index = cur; 
    }
    return &array[cur++];    
  }

  /*---  Don't use this.  Confusing  ---*/
  void resz(int new_cursor) {
    if (new_cursor < cur) {
      throw new AzException("AzDataArray::resz", "can't shrink, can only expand"); 
    }
    if (new_cursor > a.size()) {
      int new_num = (new_cursor+1023)/1024*1024; 
      a.realloc(&array, new_num, "AzDataArray::resz"); 
    }
    cur = new_cursor; 
  }

  void reset(const AzDataArray<T> *inp) {
    a.free(&array); cur = 0; 
    int num = inp->a.size(); 
    a.alloc(&array, num, "AzDataArray::reset(inp)"); 
    int ix; 
    for (ix = 0; ix < num; ++ix) array[ix] = inp->array[ix]; 
    cur = inp->cur; 
  }

protected:
  int cur; 
  T *array; 
  AzBaseArray<T> a;  

  void checkIndex(int ix, const char *msg) const {
    if (ix < 0 || ix >= a.size()) {
      throw new AzException("AzDataArray", msg, "out of range"); 
    }
  }
}; 

/*-----------------------------------------------------*/
template<class T>
class AzDataPool
{
public:
  AzDataPool() { 
    array = NULL; 
    cur = 0; 
  }
  void reset() {
    a.free(&array); 
    cur = 0; 
  }
  T *point_u(int index) {
    checkIndex(index, "point_u"); 
    return &array[index]; 
  }
  const T *point(int index) const {
    checkIndex(index, "point"); 
    return &array[index]; 
  }
  int size() const { return cur; }

  T *new_slot(int *out_index=NULL) {
    if (cur >= a.size()) {
      int inc = MIN(1024*1024, MAX(1024, cur)); 
      a.realloc(&array, cur+inc, "AzDataArray::new_slot"); 
    }
    if (out_index != NULL) {
      *out_index = cur; 
    }
    return &array[cur++];    
  }

protected:
  int cur; 
  T *array; 
  AzBaseArray<T> a;  

  void checkIndex(int ix, const char *msg) const {
    if (ix < 0 || ix >= cur) {
      throw new AzException("AzDataPool", msg, "out of range"); 
    }
  }
}; 

/*-----------------------------------------------------*/
template<class T>
class AzPtrPool
{
public:
  AzPtrPool() { 
    array = NULL; 
    cur = 0; 
  }
  void reset() {
    a.free(&array); 
    cur = 0; 
  }
  const T *point(int index) const {
    checkIndex(index, "point"); 
    return array[index]; 
  }
  T *point_u(int index) const {
    checkIndex(index, "point"); 
    return array[index]; 
  }
  int size() const { return cur; }

  T *new_slot(int *out_index=NULL) {
    if (cur >= a.size()) {
      int inc = MIN(1024*1024, MAX(1024, cur)); 
      a.realloc(&array, cur+inc, "AzObjPtrPool::new_slot"); 
    }
    if (out_index != NULL) {
      *out_index = cur; 
    }
    array[cur] = new T(); 
    return array[cur++];    
  }

protected:
  int cur; 
  T **array; 
  AzObjPtrArray<T> a;  

  void checkIndex(int ix, const char *msg) const {
    if (ix < 0 || ix >= size()) {
      throw new AzException("AzPtrArray", msg, "out of range"); 
    }
  }
}; 

/*--------------------------------------*/
class AzCursor {
public:
  AzCursor() { i=0; }
  inline int set(int v) { i=v; return i;}
  inline int get() { return i; }
  inline int rewind() { i=0; return i;}
  inline int inc() {++i; return i;}
  inline int dec() {--i; return i;}
protected: 
  int i; 
};
#endif
