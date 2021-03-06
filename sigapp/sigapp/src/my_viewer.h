# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	bool _animating;
	float upperArmRot; 
	float lowerArmRot; 
	float handRot; 
   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void build_ui ();
	void add_model ( SnShape* s, GsVec p );
	void build_scene ();
	void build_arm_scene();
	void show_normals ( bool b );
	void run_animation ();
	void run_arm_animation();
	void update_arm();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
};

