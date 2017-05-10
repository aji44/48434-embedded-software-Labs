/*! @file
 *
 *  @brief Median filter.
 *
 *  This contains the functions for performing a median filter on byte-sized data.
 *
 *  @author Corey Stidston and Menka Mehta
 *  @date 2015-10-12
 */

 #include "median.h"

 /*! @brief Median filters 3 bytes.
  *
  *  @param n1 is the first  of 3 bytes for which the median is sought.
  *  @param n2 is the second of 3 bytes for which the median is sought.
  *  @param n3 is the third  of 3 bytes for which the median is sought.
  */
  
 uint8_t Median_Filter3(const uint8_t n1, const uint8_t n2, const uint8_t n3)
 {
   if(((n1 >= n2) && (n1 <= n3)) || ((n1 <= n2) && (n1 >= n3))){
       return n1;
   }
   else if ((n2 >=n1 && n2<=n3) || (n2<=n1 && n2 >=n3))
     {
       return n2;
     }
   else {
       return n3;
   }
 }
