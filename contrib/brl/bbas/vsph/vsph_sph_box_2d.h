#ifndef vsph_sph_box_2d_h_
#define vsph_sph_box_2d_h_
//:
// \file
// \brief An axis-aligned box on the unit sphere.
// \author J.L. Mundy
// \date February 1, 2013
// \verbatim
//  Modifications none
// \endverbatim

#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vsph/vsph_sph_point_2d.h>

// theta is elevation, phi is azimuth
// 
// Note that the creation of intervals on the azimuth circle is not
// well-defined. In contrast to intervals on the real line,
// there is an ambiguity as to which portion of the circle is 
// the bounded set of angles. That is two points divide the circle
// into two arcs and the bounded set could be either one. Thus it is 
// necesssary to have three points. Points a and b define the two arcs and 
// point c determines which arc containins the bounded set.
//
// To infer the bounded set from a sequence of points it is necesary to assume 
// the inital three points are "compact," that is that they all lie in the 
// smaller of the two arcs. That is |b - a|<180 and a<c<b . In this case,
// the predicate < means counter clockwise, i.e., a<b indicates that  
// rotation to go from a to b is less than 180 and is counter-clockwise.
//

class vsph_sph_box_2d
{
 public:
  //: Default constructor
  vsph_sph_box_2d();
  //: Specify units
  vsph_sph_box_2d(bool in_radians);

  //: Three points are needed to define an unambiguous order on the phi circle
  // two of the points will bound an interval and the third defines
  // which complementary arc of the circle is "in" the box
  vsph_sph_box_2d(vsph_sph_point_2d const& pa, vsph_sph_point_2d const& pb,
		  vsph_sph_point_2d const& pc);

  ~vsph_sph_box_2d() {}
  
  void set(double min_theta, double max_theta, double a_phi, double b_phi,
	   double c_phi, bool in_radians = true);

  bool in_radians() const {return in_radians_;}

  //: bounds on azimuth and elevation
  double min_phi(bool in_radians = true) const;
  double min_theta(bool in_radians = true) const;
  double max_phi(bool in_radians = true) const;
  double max_theta(bool in_radians = true) const;
  double c_phi(bool in_radians = true) const;

  vsph_sph_point_2d min_point(bool in_radians = true) const;
  vsph_sph_point_2d max_point(bool in_radians = true) const;

  //: is box empty, i.e. no points have been added
  bool is_empty() const;

  //: add point to update box bounds
  void add( double theta, double phi, bool in_radians = true);
  void add( vsph_sph_point_2d const& pt){
    add(pt.theta_, pt.phi_, pt.in_radians_);}
  //: does the box have enough added points to uniquely define interval bounds
  bool defined() const;
  //: is an azimuth angle contained in the current azimuth interval
  bool in_interval(double phi, bool in_radians=true) const;

  bool contains(double const& theta, double const& phi, bool in_radians = true) const;
  bool contains(vsph_sph_point_2d const& p) const;
  bool contains(vsph_sph_box_2d const& b) const;

  //: area on the surface of unit sphere
  double area() const;

  //: support for binary I/O
  void print(vcl_ostream& os, bool in_radians = true) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os);

  short version() const {return 1;}
 private:
  double pye() const;
  void update_theta(double th);
  bool extend_interval(double ph);
  bool in_radians_;
  double a_phi_, b_phi_, c_phi_;
  double min_th_, max_th_;
};
vsph_sph_box_2d intersection(vsph_sph_box_2d const& b1,
			     vsph_sph_box_2d const& b2);

vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_box_2d const& p);

#endif