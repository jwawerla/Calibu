#include <memory>

#include <pangolin/pangolin.h>
#include <pangolin/gldraw.h>

#include <HAL/Camera/CameraDevice.h>
#include "GetPot"
#include <time.h>
#include <sys/time.h>


const char* sUriInfo =
  "Usage: "
  "\tcalib - capture <options> video_uri\n"
  "Options:\n"
  "\t - output, -o <dir>       Output directory to write captured images to.\n"
  "\t                       By default this will be ./images\n"

  "e.g.:\n"
  "\tcalibgrid - c leftcam.xml - c rightcaml.xml video_uri\n\n"
  "Video URI's take the following form:\n"
  " scheme:[param1=value1,param2=value2,...]//device\n"
  "\n"
  "scheme = file | dc1394 | v4l | openni | convert | split | mjpeg\n"
  "\n"
  "file/files - read PVN file format (pangolin video) or other formats using ffmpeg\n"
  " e.g. \"file:[realtime=1]///home/user/video/movie.pvn\"\n"
  " e.g. \"file:[stream=1]///home/user/video/movie.avi\"\n"
  " e.g. \"files:///home/user/sequence/foo%03d.jpeg\"\n"
  "\n"
  "dc1394 - capture video through a firewire camera\n"
  " e.g. \"dc1394:[fmt=RGB24,size=640x480,fps=30,iso=400,dma=10]//0\"\n"
  " e.g. \"dc1394:[fmt=FORMAT7_1,size=640x480,pos=2+2,iso=400,dma=10]//0\"\n"
  "\n"
  "v4l - capture video from a Video4Linux (USB) camera (normally YUVY422 format)\n"
  " e.g. \"v4l:///dev/video0\"\n"
  "\n"
  "openni - capture video / depth from an OpenNI streaming device (Kinect / Xtrion etc)\n"
  " e.g. \"openni://'\n"
  " e.g. \"openni:[img1=rgb,img2=depth]//\"\n"
  " e.g. \"openni:[img1=ir]//\"\n"
  "\n"
  "convert - use FFMPEG to convert between video pixel formats\n"
  " e.g. \"convert:[fmt=RGB24]//v4l:///dev/video0\"\n"
  " e.g. \"convert:[fmt=GRAY8]//v4l:///dev/video0\"\n"
  "\n"
  "mjpeg - capture from (possibly networked) motion jpeg stream using FFMPEG\n"
  " e.g. \"mjpeg://http://127.0.0.1/?action=stream\"\n"
  "\n"
  "split - split a single stream video into a multi stream video based on Region of Interest\n"
  " e.g. \"split:[roi1=0+0+640x480,roi2=640+0+640x480]//files:///home/user/sequence/foo%03d.jpeg\"\n"
  " e.g. \" split:[roi1=0+0+640x480,roi2=640+0+640x480]//uvc://\"\n"
  "\n";


int main( int argc, char** argv )
{
  ////////////////////////////////////////////////////////////////////
  // Create command line options. Check if we should print usage.

  GetPot cl( argc, argv );

  if( cl.search( 3, "-help", "-h", "?" ) || argc < 2 ) {
    std::cout << sUriInfo << std::endl;
    return -1;
  }

  ////////////////////////////////////////////////////////////////////
  // Default configuration values

  // Output directory for captured images
  std::string output_directory = "./images";

  ////////////////////////////////////////////////////////////////////
  // Setup Video Source

  // Last argument - Video URI
  std::string video_uri = argv[argc - 1];

  hal::Camera  cam = hal::Camera( cl.follow( video_uri.c_str(), "-video_url" ) );

  // Vector of images (that will point into buffer)
  std::vector<pangolin::Image<unsigned char> > images;
  std::vector<cv::Mat> vImages;

  // For the moment, assume all N cameras have same resolution
  const size_t N = cam.NumChannels();
  const size_t w = cam.Width();
  const size_t h = cam.Height();

  ////////////////////////////////////////////////////////////////////
  // Parse command line

  output_directory = cl.follow( output_directory.c_str(), 2, "-output", "-o" );

  ////////////////////////////////////////////////////////////////////
  // Setup GUI

  const int PANEL_WIDTH = 150;
  pangolin::CreateWindowAndBind( "Main", ( N + 1 )*w / 2.0 + PANEL_WIDTH, h / 2.0 );

  // Make things look prettier...
  glEnable( GL_LINE_SMOOTH );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDepthFunc( GL_LEQUAL );
  glEnable( GL_DEPTH_TEST );
  glLineWidth( 1.7 );

  // Create viewport for video with fixed aspect
  pangolin::CreatePanel( "ui" ).SetBounds( 1.0, 0.0, 0, pangolin::Attach::Pix( PANEL_WIDTH ) );

  pangolin::View& container = pangolin::CreateDisplay()
                              .SetBounds( 1.0, 0.0, pangolin::Attach::Pix( PANEL_WIDTH ), 1.0 )
                              .SetLayout( pangolin::LayoutEqual );

  // Add view for each camera stream
  for( size_t c = 0; c < N; ++c ) {
    container.AddDisplay( pangolin::CreateDisplay().SetAspect( w / ( float )h ) );
  }

  // OpenGl Texture for video frame
  pangolin::GlTexture tex( w, h, GL_LUMINANCE8 );


  // Display Variables
  pangolin::Var<int> disp_frame( "ui.frame" );
  pangolin::Var<float> disp_fps( "ui.fps" );

  ////////////////////////////////////////////////////////////////////
  // create directory
  char dir_name[100];
  struct tm* timeinfo;
  time_t rawtime;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime( dir_name, 100, "/%Y-%m-%d_%H-%M-%S", timeinfo );
  output_directory = output_directory + dir_name;

  if (mkdir(output_directory.c_str(), 0777) != 0) {
    std::cerr << "Failed to create directory " << output_directory << std::endl;
    return -1;
  }

  std::cout << "storing images in " << output_directory << std::endl;


  struct timeval lastTv;
  gettimeofday(&lastTv, NULL);

  ////////////////////////////////////////////////////////////////////
  // Main event loop
  for( int frame = 0; !pangolin::ShouldQuit(); ) {

    //int calib_frame = -1;

    if( cam.Capture( vImages ) ) {
      ++frame;
    }
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    struct timeval tv;
    gettimeofday(&tv, NULL);

    // iterate over all newly captured frames
    for( size_t iI = 0; iI < N; ++iI ) {
      if( vImages.size() != N ) {
        break;
      }

      // draw image on screen
      container[iI].ActivateScissorAndClear();
      glColor3f( 1, 1, 1 );

      tex.Upload( vImages[iI].data, GL_LUMINANCE, GL_UNSIGNED_BYTE );
      tex.RenderToViewportFlipY();

      // safe image to file

      if( iI == 0 ) {
        char filename[100];
        snprintf( filename, 100, "%s/left-%010ld_%06ld.pnm", output_directory.c_str(), tv.tv_sec, tv.tv_usec );
        cv::imwrite( filename, vImages[iI] );
      }
      else {
        if( iI == 1 ) {
          char filename[100];
          snprintf( filename, 100, "%s/right-%010ld_%06ld.pnm", output_directory.c_str(), tv.tv_sec, tv.tv_usec);
          cv::imwrite( filename, vImages[iI] );
        }
      }
    } // for iI
    // show frame counter
    disp_frame = frame;

    disp_fps = 1.0/ ( (tv.tv_sec + tv.tv_usec * 1e-6) - (lastTv.tv_sec + lastTv.tv_usec * 1e-6));

    lastTv = tv;

    // Process window events via GLUT
    pangolin::FinishFrame();

  } // for frame
}
