// This is vxl/vgl/io/vgl_io_box_2d.txx
#ifndef vgl_io_box_2d_txx_
#define vgl_io_box_2d_txx_

#include <vgl/io/vgl_io_box_2d.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_box_2d<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.get_min_x());
  vsl_b_write(os, p.get_min_y());
  vsl_b_write(os, p.get_max_x());
  vsl_b_write(os, p.get_max_y());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_box_2d<T> & p)
{
  short v;
  T min_pos[2];
  T max_pos[2];
  vsl_b_read(is, v);
  switch(v)
  {
  case 1:

    vsl_b_read(is, min_pos[0]);
    vsl_b_read(is, min_pos[1]);
    vsl_b_read(is, max_pos[0]);
    vsl_b_read(is, max_pos[1]);

    p.set_min_x(min_pos[0]);
    p.set_min_y(min_pos[1]);
    p.set_max_x(max_pos[0]);
    p.set_max_y(max_pos[1]);

    break;

  default:
    vcl_cerr << "vgl_box_2d<T>::vsl_b_read()";
    vcl_cerr << " Unknown version number "<< v << vcl_endl;
    vcl_abort();
  }
}

//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream& os,const vgl_box_2d<T> & p)
{
  os<<"2d Box with opposite corners at (" <<p.get_min_x() << "," <<
    p.get_min_y() <<vcl_endl;
  os<<"and (" << p.get_max_x() << "," << p.get_max_y() << ")" <<vcl_endl;
}

#define VGL_IO_BOX_2D_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vgl_box_2d<T> &); \
template void vsl_b_read(vsl_b_istream &, vgl_box_2d<T> &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_box_2d<T> &)

#endif // vgl_io_box_2d_txx_
