#ifndef BUNDLER_INTERS_H
#define BUNDLER_INTERS_H

#define BUNDLER_DEBUG

#include <vcl_vector.h>

#include <vbl/vbl_smart_ptr.txx>
#include <vbl/vbl_ref_count.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_sparse_matrix.txx>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_resource.h>

#include <mrc/vpgl/vpgl_perspective_camera.h>

// A magic number that indicates an image either has no exif tag, or
// the exif tag does not have a focal length
const int BUNDLER_NO_FOCAL_LEN = -1;

struct bundler_inters_track;
typedef vbl_smart_ptr<bundler_inters_track>
    bundler_inters_track_sptr;

struct bundler_inters_feature_set;
typedef vbl_smart_ptr<bundler_inters_feature_set>
    bundler_inters_feature_set_sptr;


/*-----------------------------------------------------------------------*/
struct bundler_inters_image
{
    vil_image_resource_sptr source;
    double focal_length;

    bool operator== (const bundler_inters_image &that) const {
        return this->source == that.source;
    }
};

/*-----------------------------------------------------------------------*/
// A single feature. Contains a (row, col), which is the location in the
// image of the feature, and a vector of doubles, the feature descriptor.
struct bundler_inters_feature : public vbl_ref_count
{
    vgl_point_2d<double> point;
    vnl_vector<double> descriptor;
    bundler_inters_image source_image;

    // Pointers to other classes that hold this feature.
    bundler_inters_track_sptr track;
    bundler_inters_feature_set_sptr feature_set;

    // General purpose boolean marking this feature as seen. 
    // Useful in a variety of situations, such as depth-first search.
    bool visited;


    // Constructors
    bundler_inters_feature() {}
    bundler_inters_feature(int row, int col):
        point(col, row), visited(false) { }


    // Accessors to avoid confusion with the row == y, col == x problem.
    double& row() {return point.y();}
    double& col() {return point.x();}

    double row() const {return point.y();}
    double col() const {return point.x();}


    //Accessors to the point.
    double& x() {return point.x();}
    double& y() {return point.y();}

    double x() const {return point.x();}
    double y() const {return point.y();}
};


// A smart pointer typedef, to ease up on memory management.
typedef vbl_smart_ptr<bundler_inters_feature>
    bundler_inters_feature_sptr;


/*-----------------------------------------------------------------------*/
// A set of features that all come from the same image
struct bundler_inters_feature_set : public vbl_ref_count
{
    vcl_vector<bundler_inters_feature_sptr> features;
    bundler_inters_image source_image;
};




/*-----------------------------------------------------------------------*/
//  This type is a pairs of sets of images, and the features for
// those images. An instance of this type represents a container that
// holds two images whose features should be matched.
struct bundler_inters_feature_set_pair
{
    bundler_inters_feature_set_sptr f1;
    bundler_inters_feature_set_sptr f2;
};


/*-----------------------------------------------------------------------*/

// A set of matched features between two images.
struct bundler_inters_match_set
{
    bundler_inters_image image1, image2;
    vcl_vector< bundler_inters_feature_sptr > side1, side2;


    void add_match(bundler_inters_feature_sptr f1,
                   bundler_inters_feature_sptr f2) {
        side1.push_back(f1);
        side2.push_back(f2);
    }

    int num_features() const {
        return side1.size();
    }

    void remove_feature(int index) {
        side1.erase(side1.begin() + index);
        side2.erase(side2.begin() + index);
    }

    void clear() {
        side1.clear();
        side2.clear();
    }
};


/*-----------------------------------------------------------------------*/
struct bundler_inters_3d_point; //Forward Declaration

// A list of camera, feature pairs
struct bundler_inters_track : public vbl_ref_count
{
    vcl_vector<bundler_inters_feature_sptr> points;

    //This is only filled if this track has started to be added
    // to the reconstruction. Otherwise, it should be NULL
    bundler_inters_3d_point *corresponding_point;

    bundler_inters_track():
        corresponding_point(NULL){ }
};


// A list of feature sets, match sets, and tracks
struct bundler_inters_track_set
{
    vcl_vector<bundler_inters_feature_set_sptr> feature_sets;
    vcl_vector<bundler_inters_match_set> match_sets;
    vcl_vector<bundler_inters_track_sptr> tracks;
};


/*-----------------------------------------------------------------------*/
struct bundler_inters_camera
{
    vpgl_perspective_camera<double> camera;
    bundler_inters_image image;
    vcl_vector<bundler_inters_track_sptr> observed_tracks;

    //Constructor that provides all the observed tracks
    bundler_inters_camera(
                           const vpgl_perspective_camera<double> &c,
                           const bundler_inters_image &img,
                           vcl_vector<bundler_inters_track_sptr> ts):
        camera(c), image(img), observed_tracks(ts) { }
    
    //Constructor that finds all the observed tracks
    bundler_inters_camera(const vpgl_perspective_camera<double> &c,
                               const bundler_inters_image &img,
                               const bundler_inters_track_set &tracks)
    : camera(c), image(img){

        //Find all tracks this camera observes
        vcl_vector<bundler_inters_track_sptr>::const_iterator i;
        for(i = tracks.tracks.begin(); i != tracks.tracks.end(); i++){

            //Look at every point in the track
            vcl_vector<bundler_inters_feature_sptr>::const_iterator j;
            for(j = (*i)->points.begin(); j != (*i)->points.end(); j++){

                //If this camera comes from the image.
                if((*j)->source_image.source == img.source){
                    observed_tracks.push_back(*i);
                    continue;
                }
            }
        }
    }
};

// This holds the current estimate of the 3d point, as well as
// all the features that are the projections of this point in the
// image.
struct bundler_inters_3d_point
{
    vgl_point_3d<double> point_3d;
    vcl_vector<bundler_inters_feature_sptr> origins;
};

struct bundler_inters_reconstruction
{
    vcl_vector<bundler_inters_camera> cameras;
    vcl_vector<bundler_inters_3d_point> points;
    vnl_sparse_matrix<bool> visibility_matrix;
};

#endif // BUNDLER_INTERS_H