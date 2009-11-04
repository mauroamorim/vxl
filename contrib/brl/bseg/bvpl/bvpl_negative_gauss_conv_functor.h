// This is brl/bseg/bvpl/bvpl_negative_gauss_conv_functor.h
#ifndef bvpl_negative_gauss_conv_functor_h
#define bvpl_negative_gauss_conv_functor_h
//:
// \file
// \brief A functor that convolves a kernel with gaussian distributions and ignores positive responses
// \author Isabel Restrepo mir@lems.brown.edu
// \date  October 1st, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_iterator.h"
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>

//: This class convolves a kernel with Gaussian distributions.
// Therefore, the response of the kernel is a Gaussian distribution obtained
// as a linear combination of input Gaussians weighted by the kernel value.
class bvpl_negative_gauss_conv_functor
{
public:
  //: Default constructor
  bvpl_negative_gauss_conv_functor();
  
  //: Destructor
  ~bvpl_negative_gauss_conv_functor() {}
  
  //: Multiply the dispatch and the input gaussians together
  void apply(bsta_gauss_f1& val, bvpl_kernel_dispatch& d);
  
  //: Returns the final operation of this functor
  bsta_gauss_f1 result();
  
private:
  float mean_;
  float var_;
  
  //: Initializes class variables
  void init();
};

#endif
