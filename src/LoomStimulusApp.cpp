#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Display.h"
#include "cinder/ImageIo.h"
#include "cinder/audio/Voice.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LoomStimulusApp : public AppNative {
  public:
    void setup();
    void keyDown( KeyEvent event );
	void update();
	void draw();
    
    Vec2f centre;
    float r_h;
    float r_v;
    float ar;
    float size;
    float dist;
    float speed;
    float transparency;
    float timestep;
    float volume_past;
    float volume_present;
    float t1;
    float t2;
    
    int   lag;
    int   time_interval;
    
    bool  timer;
    bool  trigger;
    bool  opacity;
    bool  ar_vertical;
    bool  ar_horizontal;
};

void LoomStimulusApp::setup()
{
    gl::disableVerticalSync();
    gl::enableAlphaBlending();
    
    centre = Vec2f(getWindowCenter().x, getWindowCenter().y);
    ar = 1.0f;              // aspect ratio (> 1 -> vertical ellipse; <1 -> horizontal ellipse)
    size = 10000.0f;        // size of stimulus (stays constant)
    dist = 10000.0f;        // distance of stimulus from eye
    speed = 30.0f;          // movement speed of stimulus
    transparency = 1.0f;    // transparency of stimulus (0.0 -> invisible; 1.0 -> opaque)
    timestep = 0.0f;        // timestep number
    timer = true;           // boolean to set app on timer
    trigger = false;        // trigger stimuls (irrelevant for timer setting)
    opacity = true;         // boolean to switch transparency (default = 1.0 or 0.5 | irrelevant for timer settings)
    ar_vertical = false;    // boolean to switch ar (default = 1.0 or 2.0 | irrelevant for timer setting)
    ar_horizontal = false;  // boolean to switch ar (default = 1.0 or 0.5 | irrelevant for timer setting)
    lag = -1;               // time lag before first trigger (relevant only for timer setting)
    time_interval = 60;     // time in seconds for intervals between triggers (relevant only for timer setting)
    
    if (!timer) showKeyboard();
}

void LoomStimulusApp::keyDown( KeyEvent event )
{
    if (!timer)
    {
        // w - trigger event
        // e - ar vertical
        // r - ar horizontal
        // t - transparency
        if ( event.getChar() == 'w' || event.getChar() == 'e' || event.getChar() == 'r' || event.getChar() == 't' )
        {
            if ( event.getChar() == 't' )
            {
                if (opacity) opacity = false;
                else opacity = true;
                
                if (opacity) transparency = 1.0f;
                else transparency = 0.5f;
            }
            
            if ( event.getChar() == 'e' )
            {
                if (ar_vertical) ar_vertical = false;
                else ar_vertical = true;
                
                if (ar_vertical)
                {
                    ar_horizontal = false;
                    ar = 2.0f;
                }
                else ar = 1.0f;
            }
            
            if ( event.getChar() == 'r' )
            {
                if (ar_horizontal) ar_horizontal = false;
                else ar_horizontal = true;
                
                if (ar_horizontal == true)
                {
                    ar_vertical = false;
                    ar = 0.5f;
                }
                else ar = 1.0f;
            }
            
            trigger = true;
        }
    }
}

void LoomStimulusApp::update()
{
    if (timer && getElapsedSeconds() > lag && (int)getElapsedSeconds() % time_interval == 0) trigger = true;
    
    if (trigger)
    {
        if (dist > FLT_EPSILON)
        {
            dist -= speed;
            if (dist <= FLT_EPSILON) dist = FLT_EPSILON;
        }
        
        if (dist <= FLT_EPSILON)
        {
            dist = 10000.0f;
            trigger = false;
        }
        
        if (ar <= 1.0f)
        {
            r_h = size / dist;
            r_v = r_h * ar;
        }
        else
        {
            r_v = size / dist;
            r_h = r_v / ar;
        }
        
        ++timestep;
    }
}

void LoomStimulusApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 1.0f, 1.0f, 1.0f ) );
    
    // draw circle in centre of screen (RGBA)
    gl::color( ColorA( 0.0f, 0.0f, 0.0f, transparency ) );
    
    if (trigger)
    {
        if (ar != 1.0f) gl::drawSolidEllipse(centre, r_h, r_v);
        else gl::drawSolidCircle(centre, r_h);
    }
}

CINDER_APP_NATIVE( LoomStimulusApp, RendererGl )
