//:
// \file
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_algs.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vgl/vgl_box_2d.h>

//: Destructor
bsol_algs::~bsol_algs()
{
}
//-----------------------------------------------------------------------------
// :Compute a bounding box for a set of vsol_point_2ds.
//-----------------------------------------------------------------------------

vbl_bounding_box<double,2> bsol_algs::
bounding_box(vcl_vector<vsol_point_2d_sptr> const& points)
{
  vbl_bounding_box<double, 2> b;
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
       pit != points.end(); pit++)
    b.update((*pit)->x(), (*pit)->y());
  return b;
}

//-----------------------------------------------------------------------------
// :Compute a bounding box for a set of vsol_line_2ds.
//-----------------------------------------------------------------------------
vbl_bounding_box<double,2>  bsol_algs::
bounding_box(vcl_vector<vsol_line_2d_sptr> const & lines)
{
  vbl_bounding_box<double, 2> b;
  for (vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
  {
    vsol_point_2d_sptr p0 = (*lit)->p0();
    vsol_point_2d_sptr p1 = (*lit)->p1();
    b.update(p0->x(), p0->y());
    b.update(p1->x(), p1->y());
  }
  return b;
}
//-----------------------------------------------------------------------------
// :Compute a bounding box for a set of vsol_point_3ds.
//-----------------------------------------------------------------------------
vbl_bounding_box<double,3> bsol_algs::
bounding_box(vcl_vector<vsol_point_3d_sptr> const& points)
{
  vbl_bounding_box<double, 3> b;
  for (vcl_vector<vsol_point_3d_sptr>::const_iterator pit = points.begin();
       pit != points.end(); pit++)
    b.update((*pit)->x(), (*pit)->y(), (*pit)->z());
  return b;
}
//-----------------------------------------------------------------------------
// :Determine if a point is inside a bounding box
//-----------------------------------------------------------------------------
bool bsol_algs::in(vsol_box_2d_sptr const & b, const double x, const double y)
{
  if (!b)
    return false;
  double xmin = b->get_min_x(), ymin = b->get_min_y();
  double xmax = b->get_max_x(), ymax = b->get_max_y();
  if (x<xmin||x>xmax)
    return false;
  if (y<ymin||y>ymax)
    return false;
  return true;
}

//:returns true if the a and b intersect
bool bsol_algs::meet(vsol_box_2d_sptr const & a, vsol_box_2d_sptr const & b)
{
  double min_x_a = a->get_min_x(), max_x_a = a->get_max_x();
  double min_y_a = a->get_min_y(), max_y_a = a->get_max_y();
  double min_x_b = b->get_min_x(), max_x_b = b->get_max_x();
  double min_y_b = b->get_min_y(), max_y_b = b->get_max_y();
  if (min_x_b>max_x_a||min_x_a>max_x_b)
    return false;
  if (min_y_b>max_y_a||min_y_a>max_y_b)
    return false;
  return true;
}

//:find the intersection of two boxes. Return false if no intersection
bool bsol_algs::intersection(vsol_box_2d_sptr const & a,
                             vsol_box_2d_sptr const & b,
                             vsol_box_2d_sptr& a_int_b)
{
  vgl_point_2d<double> a_min(a->get_min_x(), a->get_min_y());
  vgl_point_2d<double> a_max(a->get_max_x(), a->get_max_y());
  vgl_box_2d<double> vga(a_min, a_max);

  vgl_point_2d<double> b_min(b->get_min_x(), b->get_min_y());
  vgl_point_2d<double> b_max(b->get_max_x(), b->get_max_y());
  vgl_box_2d<double> vgb(b_min, b_max);
  vgl_box_2d<double> temp = intersect(vga, vgb);
  if (temp.is_empty())
    return false;
  a_int_b = new vsol_box_2d();
  a_int_b->add_point(temp.min_x(), temp.min_y());
  a_int_b->add_point(temp.max_x(), temp.max_y());
  return true;
}

bool bsol_algs::box_union(vsol_box_2d_sptr const & a,
                          vsol_box_2d_sptr const & b,
                          vsol_box_2d_sptr& a_union_b)
{
  if (!a||!b)
    return false;
  double x_min_a = a->get_min_x(), y_min_a = a->get_min_y();
  double x_max_a = a->get_max_x(), y_max_a = a->get_max_y();
  double x_min_b = b->get_min_x(), y_min_b = b->get_min_y();
  double x_max_b = b->get_max_x(), y_max_b = b->get_max_y();
  double x_min=x_min_a, y_min = y_min_a;
  double x_max=x_max_a, y_max = y_max_a;
  if (x_min_b<x_min)
    x_min = x_min_b;
  if (y_min_b<y_min)
    y_min = y_min_b;
  if (x_max_b>x_max)
    x_max = x_max_b;
  if (y_max_b>y_max)
    y_max = y_max_b;
  a_union_b = new vsol_box_2d();
  a_union_b->add_point(x_min, y_min);
  a_union_b->add_point(x_max, y_max);
  return true;
}

//-----------------------------------------------------------------------------
// :Determine if a point is inside a bounding box
//-----------------------------------------------------------------------------
bool bsol_algs::in(vsol_box_3d_sptr const & b, const double x, const double y,
                   const double z)
{
  if (!b)
    return false;
  double xmin = b->get_min_x(), ymin = b->get_min_y(), zmin = b->get_min_z();
  double xmax = b->get_max_x(), ymax = b->get_max_y(), zmax = b->get_max_z();
  if (x<xmin||x>xmax)
    return false;
  if (y<ymin||y>ymax)
    return false;
  if (z<zmin||z>zmax)
    return false;
  return true;
}


#ifdef VCL_VC60
//Get rid of warning generated by fault deep inside MS supplied library
# pragma warning(disable:4018 )
#endif


vsol_polygon_2d_sptr bsol_algs::poly_from_box(vsol_box_2d_sptr const& box)
{
  vcl_vector<vsol_point_2d_sptr> pts;
  vsol_point_2d_sptr pa = new vsol_point_2d(box->get_min_x(), box->get_min_y());
  vsol_point_2d_sptr pb = new vsol_point_2d(box->get_max_x(), box->get_min_y());
  vsol_point_2d_sptr pc = new vsol_point_2d(box->get_max_x(), box->get_max_y());
  vsol_point_2d_sptr pd = new vsol_point_2d(box->get_min_x(), box->get_max_y());
  pts.push_back(pa);   pts.push_back(pb);
  pts.push_back(pc);   pts.push_back(pd);
  return new vsol_polygon_2d(pts);
}
//:find the closest point to p in a set of points
vsol_point_2d_sptr
bsol_algs::closest_point(vsol_point_2d_sptr const& p,
                         vcl_vector<vsol_point_2d_sptr> const& point_set,
                         double& d)
{
  vsol_point_2d_sptr cp;
  int n = point_set.size();
  if (!p||!n)
    return cp;
  double dmin_sq = vnl_numeric_traits<double>::maxval;
  double x = p->x(), y = p->y();
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = point_set.begin();
       pit!=point_set.end(); pit++)
  {
    double xs = (*pit)->x(), ys = (*pit)->y();
    double dsq = (x-xs)*(x-xs)+(y-ys)*(y-ys);
    if (dsq<dmin_sq)
    {
      dmin_sq = dsq;
      cp = *pit;
    }
  }
  d = vcl_sqrt(dmin_sq);
  return cp;
}

vsol_point_3d_sptr bsol_algs::
closest_point(vsol_point_3d_sptr const& p,
              vcl_vector<vsol_point_3d_sptr> const& point_set,
              double& d)
{
  d = 0;
  vsol_point_3d_sptr cp;
  int n = point_set.size();
  if (!p||!n)
    return cp;
  double dmin_sq = vnl_numeric_traits<double>::maxval;
  double x = p->x(), y = p->y(), z = p->z();
  for (vcl_vector<vsol_point_3d_sptr>::const_iterator pit = point_set.begin();
       pit!=point_set.end(); pit++)
  {
    double xs = (*pit)->x(), ys = (*pit)->y(), zs = (*pit)->z();
    double dsq = (x-xs)*(x-xs) + (y-ys)*(y-ys) + (z-zs)*(z-zs);
    if (dsq<dmin_sq)
    {
      dmin_sq = dsq;
      cp = *pit;
    }
  }
  d = vcl_sqrt(dmin_sq);
  return cp;
}
//: Transform a vsol_polygon_2d with a general homography.
//  Return false if any of the points are turned into ideal points
//  since vsol geometry is assumed finite.
bool bsol_algs::homography(vsol_polygon_2d_sptr const& p,
                           vgl_h_matrix_2d<double> const& H,
                           vsol_polygon_2d_sptr& Hp)
{
  const int n = p->size();
  vcl_vector<vsol_point_2d_sptr> pts;
  const double tol = 1e-06;
  for (int i = 0; i<n; i++)
  {
    vsol_point_2d_sptr v = p->vertex(i);
    vgl_homg_point_2d<double> hp(v->x(), v->y());
    vgl_homg_point_2d<double> Hhp = H(hp);
    if (Hhp.ideal(tol))
      return false;
    vgl_point_2d<double> q(Hhp);
    vsol_point_2d_sptr qs = new vsol_point_2d(q.x(), q.y());
    pts.push_back(qs);
  }
  Hp = new vsol_polygon_2d(pts);
  return true;
}

bool bsol_algs::homography(vsol_box_2d_sptr const& b,
                           vgl_h_matrix_2d<double> const& H,
                           vsol_box_2d_sptr& Hb)
{
  vsol_polygon_2d_sptr p = bsol_algs::poly_from_box(b);
  vsol_polygon_2d_sptr Hp;
  if (!homography(p, H, Hp))
    return false;
  Hb = Hp->get_bounding_box();
  return true;
}

void bsol_algs::print(vsol_box_2d_sptr const& b)
{
  if (!b)
    return;
  double xmin = b->get_min_x(), ymin = b->get_min_y();
  double xmax = b->get_max_x(), ymax = b->get_max_y();
  vcl_cout << "vsol_box_2d[(" << xmin << ' ' << ymin << ")<("
           << xmax << ' ' << ymax << ")]\n";
}

void bsol_algs::print(vsol_box_3d_sptr const& b)
{
  if (!b)
    return;
  double xmin = b->get_min_x(), ymin = b->get_min_y(), zmin = b->get_min_z();
  double xmax = b->get_max_x(), ymax = b->get_max_y(), zmax = b->get_max_z();
  vcl_cout << "vsol_box_2d[(" << xmin << ' ' << ymin << ' ' << zmin << ")<("
           << xmax << ' ' << ymax << ' ' << zmax << ")]\n";
}

void bsol_algs::print(vsol_point_2d_sptr const& p)
{
  if (!p)
    return;
  vcl_cout << "vsol_point_2d[ " << p->x() << ' ' << p->y() <<  " ]\n";
}

void bsol_algs::print(vsol_point_3d_sptr const& p)
{
  if (!p)
    return;
  vcl_cout << "vsol_point_3d[ " << p->x() << ' ' << p->y() << ' '
           << p->z() <<  " ]\n";
}
