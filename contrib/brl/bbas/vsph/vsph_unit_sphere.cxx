
#include "vsph_unit_sphere.h"
#include "vsph_sph_point_2d.h"
#include <vsl/vsl_vector_io.h>
#include "vsph_utils.h"
#include <vnl/vnl_math.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vcl_algorithm.h>

#include <bvrml/bvrml_write.h>
#include <vcl_limits.h>

bool operator < (vsph_edge const& a, vsph_edge const& b)
{
  if (a.vs_!=b.vs_)
    return a.vs_ < b.vs_;
  return a.ve_ < b.ve_;
}

vsph_unit_sphere::vsph_unit_sphere(double point_angle,
                                   double min_theta, double max_theta) :
  neighbors_valid_(false), point_angle_(point_angle),
  min_theta_(min_theta), max_theta_(max_theta)
{
  add_uniform_views();
  remove_top_and_bottom();
}

vgl_vector_3d<double> vsph_unit_sphere::cart_coord(vsph_sph_point_2d const& sp) const
{
  double x = vcl_sin(sp.theta_)*vcl_cos(sp.phi_);
  double y = vcl_sin(sp.theta_)*vcl_sin(sp.phi_);
  double z = vcl_cos(sp.theta_);
  return vgl_vector_3d<double>(x, y, z);
}

vsph_sph_point_2d vsph_unit_sphere::spher_coord(vgl_vector_3d<double> const& cp) const
{
  double x = cp.x(), y = cp.y(), z = cp.z();
  double phi = vcl_atan2(y,x);
  double theta = vcl_acos(z);
  return vsph_sph_point_2d(theta, phi);
}

void vsph_unit_sphere::add_uniform_views()
{
  double pt_angle_rad = point_angle_/vnl_math::deg_per_rad;
  double cap_angle_rad = vnl_math::pi; //historical reasons (fix me)

  // create a octahedron on the sphere, define 6 points for the vertices of the triangles
  vcl_vector<vgl_vector_3d<double> > verts;
  vgl_vector_3d<double> v1(0.0,0.0, 1.0); verts.push_back(v1);
  vgl_vector_3d<double> v2(0.0,0.0,-1.0); verts.push_back(v2);
  vgl_vector_3d<double> v3( 1.0,0.0,0.0); verts.push_back(v3);
  vgl_vector_3d<double> v4(-1.0,0.0,0.0); verts.push_back(v4);
  vgl_vector_3d<double> v5(0.0, 1.0,0.0); verts.push_back(v5);
  vgl_vector_3d<double> v6(0.0,-1.0,0.0); verts.push_back(v6);

  // vector of triangles (vector of 3 points, only indices of the vertices kept)
  vcl_vector<vcl_vector<int> > triangles;

  vcl_vector<int> tri1;
  tri1.push_back(0); tri1.push_back(2); tri1.push_back(4); triangles.push_back(tri1);

  vcl_vector<int> tri2;
  tri2.push_back(0); tri2.push_back(4); tri2.push_back(3); triangles.push_back(tri2);

  vcl_vector<int> tri3;
  tri3.push_back(0); tri3.push_back(3); tri3.push_back(5); triangles.push_back(tri3);

  vcl_vector<int> tri4;
  tri4.push_back(0); tri4.push_back(5); tri4.push_back(2); triangles.push_back(tri4);

  vcl_vector<int> tri5;
  tri5.push_back(1); tri5.push_back(2); tri5.push_back(4); triangles.push_back(tri5);

  vcl_vector<int> tri6;
  tri6.push_back(1); tri6.push_back(3); tri6.push_back(4); triangles.push_back(tri6);

  vcl_vector<int> tri7;
  tri7.push_back(1); tri7.push_back(5); tri7.push_back(3); triangles.push_back(tri7);

  vcl_vector<int> tri8;
  tri8.push_back(1); tri8.push_back(2); tri8.push_back(5); triangles.push_back(tri8);

  // iteratively refine the triangles
  // check the angle between two vertices (of the same triangle),
  // use the center of the spherical coordinate system
  // vgl_vector_3d<double> vector1=verts[triangles[0][0]]-center;
  // vgl_vector_3d<double> vector2=verts[triangles[0][1]]-center;

  bool done=false;
  while (!done) {
    vcl_vector<vcl_vector<int> >  new_triangles;
    int ntri=triangles.size();
    for (int i=0; i<ntri; i++) {
      vcl_vector<int> points;
      for (int j=0; j<3; j++) {
        // find the mid points of edges
        int next=j+1; if (next == 3) next=0;
        vgl_vector_3d<double> v0 = verts[triangles[i][j]];
        vgl_vector_3d<double> vn = verts[triangles[i][next]];
        vgl_point_3d<double> p0(v0.x(), v0.y(), v0.z());
        vgl_point_3d<double> pn(vn.x(), vn.y(), vn.z());
        vgl_line_segment_3d<double> edge1(p0,pn);
        vgl_point_3d<double> midp=edge1.point_t(0.5);
        // convert to a unit vector on the sphere surface
        vgl_vector_3d<double> mid(midp.x(), midp.y(), midp.z());
        mid  = normalized(mid);
        // add a new vertex for mid points of the edges of the triangle
        int idx = verts.size();
        verts.push_back(mid);

        points.push_back(triangles[i][j]);  // existing vertex of the bigger triangle
        points.push_back(idx);              // new mid-point vertex
      }

      // add new samller 4 triangles instead of the old big one
      /******************************
                   /\
                  /  \
                 /    \
                /      \
               /--------\
              / \      / \
             /   \    /   \
            /     \  /     \
           /       \/       \
           -------------------
      *******************************/
      done=true;
      vcl_vector<int> list(3); list[0]=points[0]; list[1]=points[5]; list[2]=points[1];
      new_triangles.push_back(list);
      // check for point_angles
      vcl_vector<vgl_vector_3d<double> > triangle;
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[1]; list[1]=points[3]; list[2]=points[2];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[3]; list[1]=points[5]; list[2]=points[4];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[1]; list[1]=points[5]; list[2]=points[3];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;
    }
    // check the angle again to see if the threashold is met
    //vgl_vector_3d<double> vector1=verts[new_triangles[0][0]]-center;
    //vgl_vector_3d<double> vector2=verts[new_triangles[0][1]]-center;
    triangles.clear();
    triangles=new_triangles;
#if 0
    vcl_cout << "sus:" << verts.size() << ' ' << vcl_flush;
#endif
  }
  // refine the vertices to points, eliminate duplicate ones and also eliminate the ones below given elevation
  // note that the relationship between vertex id and the id of the
  // cart and sphere containers is changed by this filter
  int ntri = triangles.size();
  for (int i=0; i<ntri; i++) {
    for (int j=0; j<3; j++) {
      int vidx = triangles[i][j];
      vgl_vector_3d<double>& cv = verts[vidx];
      vsph_sph_point_2d sv = spher_coord(cv);
      //is cv already in cart_pts?
      int id = -1;
      bool equal = this->find_near_equal(cv, id);
      if (equal)
        equivalent_ids_[vidx]=id;// keep track of map between old and new ids
      // if not add it
      if (!equal&&(sv.theta_ <= cap_angle_rad)) {
        cart_pts_.push_back(cv);
        sph_pts_.push_back(sv);
      }
    }
#if 1
    if (i%1000 ==0)
      vcl_cout << '.' << vcl_flush;
      //vcl_cout << "susN:" << sph_pts_.size() << ' ' << vcl_flush;
#endif
  }
  vcl_cout << '\n' << vcl_flush;
#if 0
  vcl_cout << "finished refine\n" << vcl_flush;
#endif
  // step through the triangles and construct unique edges
  // two edges are equal if their end points are equal regardless
  // of order.
  vcl_vector<vsph_edge>::iterator eit;
  for (int i=0; i<ntri; i++) {
    int v[3];// triangle vertices
    v[0] = equivalent_ids_[triangles[i][0]];//construct edges with current
    v[1] = equivalent_ids_[triangles[i][1]];//cart and sphere vertex ids
    v[2] = equivalent_ids_[triangles[i][2]];//updated from initial "verts" id
    //traverse the edges of the triangle
    for (int j = 0; j<3; ++j) {
      vsph_edge e(v[j],v[(j+1)%3]);//wrap around to 0
      eit = vcl_find(edges_.begin(), edges_.end(), e);
      if (eit == edges_.end())
        edges_.push_back(e);
    }
  }
#if 0
  vcl_cout << "finished find edges\n" << vcl_flush;
#endif
  neighbors_valid_ = false;
}

void vsph_unit_sphere::remove_top_and_bottom()
{
#if 0
  vcl_cout << "entering top and bottom" << sph_pts_.size() << vcl_endl;
#endif
  double margin = 0.00035;
  equivalent_ids_.clear();
  double min_theta_rad = min_theta_/vnl_math::deg_per_rad;
  double max_theta_rad = max_theta_/vnl_math::deg_per_rad;
  vcl_vector<vgl_vector_3d<double> > cart_pts_new;
  vcl_vector<vsph_sph_point_2d> sph_pts_new;
  vcl_vector<vsph_sph_point_2d>::iterator pit = sph_pts_.begin();
  int indx = 0;
  for (; pit!=sph_pts_.end();++pit, ++indx) {
    vsph_sph_point_2d& sp = (*pit);
    if (sp.theta_ > (min_theta_rad-margin) &&
        sp.theta_ < (max_theta_rad+margin)) {
      int ns = cart_pts_new.size();
      equivalent_ids_[indx] = ns;
      sph_pts_new.push_back(sp);
      cart_pts_new.push_back(cart_pts_[indx]);
    }
    else { equivalent_ids_[indx] = -1; }
  }

  sph_pts_.clear();
  sph_pts_ = sph_pts_new;
  cart_pts_.clear();
  cart_pts_ = cart_pts_new;
#if 0
  vcl_cout << "starting to remap edges\n" << vcl_flush;
#endif
  vcl_vector<vsph_edge> new_edges;
  for (vcl_vector<vsph_edge>::iterator eit = edges_.begin();
       eit != edges_.end(); ++eit) {
    int is = equivalent_ids_[(*eit).vs_],
        ie = equivalent_ids_[(*eit).ve_];
    if (is == -1 || ie == -1)
      continue;
    new_edges.push_back(vsph_edge(is, ie));
  }
#if 0
  vcl_cout << "finished remap edges\n" << vcl_flush;
#endif
  edges_.clear();
  edges_ = new_edges;
  neighbors_valid_ = false;
}

void vsph_unit_sphere::find_neighbors()
{
  int nv = this->size();
  neighbors_.clear();
  neighbors_.resize(nv);
  int ne = edges_.size();
  for (int iv = 0; iv<nv; ++iv)
    for (int ie =0; ie<ne; ++ie) {
      vsph_edge& e = edges_[ie];
      if (e.vs_ == iv) neighbors_[iv].push_back(e.ve_);
      if (e.ve_ == iv) neighbors_[iv].push_back(e.vs_);
    }
  neighbors_valid_ = true;
}

bool vsph_unit_sphere::
find_near_equal(vgl_vector_3d<double>const& p, int& id, double tol)
{
  vcl_vector<vgl_vector_3d<double> >::iterator it = cart_pts_.begin();
  id = 0;
  for (;it != cart_pts_.end(); it++, ++id) {
    vgl_vector_3d<double>& cp = *it;
    //    double dist = 1.0 - dot_product(p, cp);
    vgl_vector_3d<double> dif = cp-p;
    double dist = length(dif);
    if (dist<=tol)
      return true;
  }
  id = -1;
  return false;
}

// construct Cartesian vectors from spherical points
void vsph_unit_sphere::set_cart_points()
{
  for (vcl_vector<vsph_sph_point_2d>::iterator sit = sph_pts_.begin();
       sit != sph_pts_.end(); ++sit)
    cart_pts_.push_back(cart_coord(*sit));
}


void vsph_unit_sphere::print(vcl_ostream& os) const
{
  os << "vsph_unit_sphere: " << size() << vcl_endl;
  unsigned idx = 0;
  for (vcl_vector<vsph_sph_point_2d>::const_iterator sit = sph_pts_.begin();
       sit != sph_pts_.end(); ++sit, ++idx)
    os << '(' << idx << ") " << *sit << vcl_endl;
  os << vcl_endl;
}


// point_angle must be in radians
bool vsph_unit_sphere::min_angle(vcl_vector<vgl_vector_3d<double> > list,
                                 double angle_rad)
{
  if (list.size() < 2)
    return false;

  for (unsigned i=0; i<list.size(); i++) {
    unsigned next = i+1;
    if (next == list.size()) next = 0;
    vgl_vector_3d<double>& vector1=list[i];
    vgl_vector_3d<double>& vector2=list[next];
    if (angle(vector1, vector2) > angle_rad)
      return false;
  }
  return true;
}


void vsph_unit_sphere::display_vertices(vcl_string const & path) const
{
  vcl_ofstream os(path.c_str());
  if (!os.is_open())
    return;
  os << "VRML V2.0 utf8\n"
     << "Shape {\n"
     << "   appearance Appearance {\n"
     << "      material Material {\n"
     << "         emissiveColor 1.0 1.0 1.0\n"
     <<        "}\n"
     <<    "}\n"
     << "  geometry PointSet {\n"
     << "     coord Coordinate {\n"
     << "      point [\n";
  int cnt = 0;
  int np = cart_pts_.size()-1;
  for (vcl_vector<vgl_vector_3d<double> >::const_iterator cit = cart_pts_.begin();
       cit != cart_pts_.end(); ++cit, ++cnt) {
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if (cnt != np) os << ',';
    os << '\n';
  }
  os <<"    ]\n"
     <<"   }\n"
     << " }\n"
     <<"}\n";
}

void vsph_unit_sphere::display_edges(vcl_string const & path) const
{
  vcl_ofstream os(path.c_str());
  if (!os.is_open())
    return;
  os << "VRML V2.0 utf8\n"
     << "Shape {\n"
     << "   appearance Appearance {\n"
     << "      material Material {\n"
     << "         emissiveColor 1.0 1.0 1.0\n"
     <<        "}\n"
     <<    "}\n"
     << "  geometry IndexedLineSet {\n"
     << "     coord Coordinate {\n"
     << "      point [\n";
  int cnt = 0;
  int np = cart_pts_.size()-1;
  for (vcl_vector<vgl_vector_3d<double> >::const_iterator cit = cart_pts_.begin();
       cit != cart_pts_.end(); ++cit, ++cnt) {
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if (cnt != np) os << ',';
    os << '\n';
  }
  os <<             "]\n"
     <<      "}\n"
     << "     coordIndex [\n";

  cnt = 0;
  int ne = edges_.size()-1;
  for (vcl_vector<vsph_edge>::const_iterator eit = edges_.begin();
       eit != edges_.end(); ++eit, ++cnt) {
    const vsph_edge& e = *eit;
    os << e.vs_ << ',' << e.ve_;
    if (cnt != ne)
      os << ",-1,\n";
    else os << '\n';
  }
  os <<        "]\n"
     << "    }\n"
     << "  }\n";
  os.close();
}

void vsph_unit_sphere::
display_region_data(vcl_string const & path,
                    vcl_vector<double> const& data) const
{
  vcl_ofstream os(path.c_str());
  if (!os.is_open())
    return;
  // find range of data
  double minv = vcl_numeric_limits<double>::max();
  double maxv = -vcl_numeric_limits<double>::max();
  int nd = data.size();
  for (int i=0; i<nd; ++i) {
    double d = data[i];
    if (d < minv) minv = d;
    if (d > maxv) maxv = d;
  }
  double dif = maxv-minv;
  if (dif == 0.0)
    dif = 1.0;
  bvrml_write::write_vrml_header(os);
  // write a world center and world axis
  double rad = 1.0;
  vgl_point_3d<float> cent(0.0,0.0,0.0);
  vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), (float)rad);
  bvrml_write::write_vrml_sphere(os, sp, 1.0f, 0.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_x, (float)rad*20, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_y, (float)rad*20, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_z, (float)rad*20, 0.0f, 1.0f, 1.0f);
  vgl_sphere_3d<float> sp2((float)cent.x(), (float)cent.y(), (float)cent.z()+20, (float)rad);
  bvrml_write::write_vrml_sphere(os, sp2, 0.0f, 0.0f, 1.0f, 0.0f);

  // write the voxel structure
  float disc_radius = static_cast<float>(point_angle_/vnl_math::deg_per_rad);
  vgl_point_3d<double> orig(0.0,0.0,0.0);
  for (unsigned i = 0; i < cart_pts_.size(); i++) {
    vgl_vector_3d<double> ray = cart_pts_[i];
    float val = static_cast<float>((data[i]-minv)/dif);
    bvrml_write::write_vrml_disk(os, orig+10*ray, ray, disc_radius,
                                 val, val, 0.0f);
  }
  os.close();

  int cnt = 0;
  int np = cart_pts_.size()-1;
  for (vcl_vector<vgl_vector_3d<double> >::const_iterator cit = cart_pts_.begin();
       cit != cart_pts_.end(); ++cit, ++cnt) {
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if (cnt != np) os << ',';
    os << '\n';
  }
  os <<"    ]\n"
     <<"   }\n"
     << " }\n"
     <<"}\n";
}

void vsph_unit_sphere::
display_region_color(vcl_string const & path,
                     vcl_vector<vcl_vector<float> > const& cdata,
                     vcl_vector<float> const& skip_color) const
{
  vcl_ofstream os(path.c_str());
  if (!os.is_open())
    return;
  bvrml_write::write_vrml_header(os);
  // write a world center and world axis
  double rad = 1.0;
  vgl_point_3d<float> cent(0.0,0.0,0.0);
  vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), (float)rad);
  bvrml_write::write_vrml_sphere(os, sp, 1.0f, 0.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_x, (float)rad*20, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_y, (float)rad*20, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_z, (float)rad*20, 0.0f, 1.0f, 1.0f);
  vgl_sphere_3d<float> sp2((float)cent.x(), (float)cent.y(), (float)cent.z()+20, (float)rad);
  bvrml_write::write_vrml_sphere(os, sp2, 0.0f, 0.0f, 1.0f, 0.0f);

  // write the voxel structure
  float disc_radius = static_cast<float>(point_angle_/vnl_math::deg_per_rad);
  vgl_point_3d<double> orig(0.0,0.0,0.0);
  for (unsigned i = 0; i < cart_pts_.size(); i++) {
    vgl_vector_3d<double> ray = cart_pts_[i];
    const vcl_vector<float>& cl = cdata[i];
    if (cl[0]==skip_color[0]&&cl[1]==skip_color[1]&&cl[2]==skip_color[2])
      continue;
    bvrml_write::write_vrml_disk(os, orig+10*ray, ray, disc_radius,
                                 cl[0], cl[1], cl[2]);
  }
  os.close();

  int cnt = 0;
  int np = cart_pts_.size()-1;
  for (vcl_vector<vgl_vector_3d<double> >::const_iterator cit = cart_pts_.begin();
       cit != cart_pts_.end(); ++cit, ++cnt) {
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if (cnt != np) os << ',';
    os << '\n';
  }
  os <<"    ]\n"
     <<"   }\n"
     << " }\n"
     <<"}\n";
}

bool vsph_unit_sphere::operator==(const vsph_unit_sphere &other) const
{
  if (point_angle_ != other.point_angle() ||
      min_theta_   != other.min_theta() ||
      max_theta_   != other.max_theta() ||
      this->size() != other.size())
    return false;
  const vcl_vector<vsph_sph_point_2d>& osph_pts = other.sph_points();
  for (unsigned i = 0; i < sph_pts_.size(); ++i) {
    if (!(sph_pts_[i]==osph_pts[i]))
      return false;
  }
  return true;
}

// =================   binary I/O ==========================
void vsph_unit_sphere::b_read(vsl_b_istream& is)
{
  neighbors_valid_ = false;
  short version;
  vsl_b_read(is, version);
  switch (version) {
   case 1:
    {
      vsl_b_read(is, point_angle_);
      vsl_b_read(is, min_theta_);
      vsl_b_read(is, max_theta_);
      vsl_b_read(is, sph_pts_);
      vsl_b_read(is, edges_);
      this->set_cart_points();
      break;
    }
   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vsph_unit_sphere&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

void vsph_unit_sphere::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, point_angle_);
  vsl_b_write(os, min_theta_);
  vsl_b_write(os, max_theta_);
  vsl_b_write(os, sph_pts_);
  vsl_b_write(os, edges_);
}

void vsl_b_read(vsl_b_istream& is, vsph_unit_sphere& usph)
{
  usph.b_read(is);
}

void vsl_b_write(vsl_b_ostream& os, vsph_unit_sphere const& usph)
{
  usph.b_write(os);
}

void vsph_edge::b_read(vsl_b_istream& is)
{
  short version;
  vsl_b_read(is, version);
  switch (version) {
   case 1:
    {
      vsl_b_read(is, vs_);
      vsl_b_read(is, ve_);
      break;
    }
   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vsph_edge&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

void vsph_edge::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, (short)version());
  vsl_b_write(os, vs_);
  vsl_b_write(os, ve_);
}

void vsl_print_summary(vcl_ostream& os, vsph_edge const& e)
{
  e.print(os);
}

void vsl_b_read(vsl_b_istream& is, vsph_edge& e)
{
  e.b_read(is);
}

void vsl_b_write(vsl_b_ostream& os, vsph_edge const& e)
{
  e.b_write(os);
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_unit_sphere const& vs)
{
  vs.print(os);
  return os;
}

void vsl_b_write(vsl_b_ostream &os, vsph_unit_sphere const* usph_ptr)
{
  if (usph_ptr==0)
    vsl_b_write(os, false);
  else {
    vsl_b_write(os, true);
    vsl_b_write(os, *usph_ptr);
  }
}

void vsl_b_read(vsl_b_istream &is, vsph_unit_sphere*& usph_ptr)
{
  delete usph_ptr; usph_ptr = 0;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    usph_ptr = new vsph_unit_sphere();
    usph_ptr->b_read(is);
  }
}

void vsl_b_write(vsl_b_ostream &os, vsph_unit_sphere_sptr const&  usph_sptr)
{
  vsl_b_write(os, usph_sptr.ptr());
}

void vsl_b_read(vsl_b_istream &is, vsph_unit_sphere_sptr& usph_sptr)
{
  vsph_unit_sphere* usph_ptr = 0;
  vsl_b_read(is, usph_ptr);
  usph_sptr = usph_ptr;
}