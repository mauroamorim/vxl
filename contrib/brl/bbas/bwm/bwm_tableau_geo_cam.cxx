#include "bwm_tableau_geo_cam.h"
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include "bwm_observer_mgr.h"
#include "algo/bwm_utils.h"
#include <vul/vul_file.h>
#include <vcl_sstream.h>

bool bwm_tableau_geo_cam::handle(const vgui_event &e)
{
  return bwm_tableau_cam::handle(e);
}

void bwm_tableau_geo_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  vgui_menu parent_menu;
  bwm_tableau_cam::get_popup(params, menu);
}

vcl_string bwm_tableau_geo_cam::save_camera()
{
#if 0
  vcl_string img_path = this->img_path();
  vcl_string cam_path = my_observer_->camera_path();

  // see if the camera is adjusted
  if (my_observer_->camera_adjusted()) {
    //need to save the new camera
    vcl_string new_cam_path = vul_file::strip_extension(cam_path);
    vcl_string::size_type pos = new_cam_path.find("_v", 0);
    if (pos != vcl_string::npos) {
      new_cam_path.erase(pos, new_cam_path.length()-1);
    }
    vcl_stringstream strm;
    strm << vcl_fixed << timer_.real();
    vcl_string str(strm.str());
    new_cam_path += "_v" + str + vul_file::extension(cam_path);
    vpgl_camera<double>* cam = my_observer_->camera();
    if (!cam) {
      vcl_cout << " Null camera in bwm_tableau_geo_cam::save_camera()\n";
      return "";
    }
    //check for appropriate type
    if (cam->type_name()=="vpgl_proj_camera"||
        cam->type_name()=="vpgl_perspective_camera") {
      vpgl_proj_camera<double>* pcam = static_cast<vpgl_proj_camera<double>*>(cam);
      pcam->save(new_cam_path);
    }
    else {
      vcl_cout << " not projective camera type in bwm_tableau_geo_cam::save_camera()\n";
      return "";
    }
    // camera is saved and no need to save the next time
    my_observer_->set_camera_path(new_cam_path);
    my_observer_->set_camera_adjusted(false);

    return new_cam_path;
  }
  else { //no need to save the camera(not adjusted)just return the current path
    return cam_path;
  }
#else
  return "";
#endif
}
