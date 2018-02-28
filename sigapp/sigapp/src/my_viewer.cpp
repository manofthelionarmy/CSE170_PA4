
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>


#define M_PI 3.14159f
MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	_nbut=0;
	_animating=false;
	build_ui ();
	//build_scene ();
	build_arm_scene();
}

void MyViewer::build_ui ()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _nbut=new UiCheckButton ( "Normals", EvNormals ) ); 
	}
	p->add ( new UiButton ( "Animate", EvAnimate ) );
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}

void MyViewer::add_model ( SnShape* s, GsVec p )
{
	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation ( p );
	manip->initial_mat ( m );

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);

	rootg()->add(manip);
}

void MyViewer::build_scene ()
{
	SnPrimitive* p;

	p = new SnPrimitive(GsPrimitive::Box,1,3,1);
	p->prim().material.diffuse=GsColor::yellow;
	add_model ( p, GsVec(0,0,0) );

	p = new SnPrimitive(GsPrimitive::Sphere,2);
	p->prim().material.diffuse=GsColor::red;
	add_model ( p, GsVec(-4,0,0) );

	p = new SnPrimitive(GsPrimitive::Cylinder,1.0,1.0,1.5);
	p->prim().material.diffuse=GsColor::blue;
	add_model ( p, GsVec(4,0,0) );

	p = new SnPrimitive(GsPrimitive::Capsule,1,1,3);
	p->prim().material.diffuse=GsColor::red;
	add_model ( p, GsVec(8,0,0) );

	p = new SnPrimitive(GsPrimitive::Ellipsoid,2.0,0.5);
	p->prim().material.diffuse=GsColor::green;
	add_model ( p, GsVec(-8,0,0) );
}



void MyViewer::build_arm_scene() {

	SnGroup *g = new SnGroup;

	SnModel *hand = new SnModel; 
	SnModel *lower_arm = new SnModel;
	SnModel *upper_arm = new SnModel;
	
	
	char *handfile = "C:\\Users\\Armando Leon\\Desktop\\workspace\\PA4\\sigapp\\sigapp\\src\\arm\\rhand.m";
	char *lowerarmfile = "C:\\Users\\Armando Leon\\Desktop\\workspace\\PA4\\sigapp\\sigapp\\src\\arm\\rlowerarm.m";
	char *upperarmfile = "C:\\Users\\Armando Leon\\Desktop\\workspace\\PA4\\sigapp\\sigapp\\src\\arm\\rupperarm.m";
	
	if (hand->model()->load(handfile)) {
		add_model(hand, GsVec(0, 0, 0));
	}
	
	if (upper_arm->model()->load(upperarmfile)) {
		add_model(upper_arm, GsVec(-1, 0, 0));
	}


	
	
	if (lower_arm->model()->load(lowerarmfile)) {
		add_model(lower_arm, GsVec(1, 0, 0));
	}
	
	

}
void MyViewer::update_arm()
{

	SnManipulator* manip = rootg()->get<SnManipulator>(1); //upper arm
	GsMat m = manip->mat();

	SnManipulator* manip1 = rootg()->get<SnManipulator>(2); //lower arm
	GsMat m1 = manip1->mat();

	SnManipulator* manip2 = rootg()->get<SnManipulator>(0); //hand
	GsMat m2 = manip2->mat();

	GsMat translation, rotation, transform;

	translation.translation(0, 0, -0.5f);
	rotation.roty(M_PI / 2.0f);
	transform = rotation * translation;

	//draw the upper arm
	rotation.rotx(upperArmRot);
	transform = transform * rotation;
	m = transform;
	manip->initial_mat(m);

	//draw the lower arm
	translation.translation(0.0f, 0.0f, 26.0f);
	rotation.rotx(lowerArmRot);
	transform = transform * translation * rotation;
	m1 = transform;
	manip1->initial_mat(m1);

	//draw the hand
	translation.translation(0.0f, 0.0f, 24.0f);
	rotation.rotx(handRot);
	transform = transform * translation * rotation;
	m2 = transform;
	manip2->initial_mat(m2);
}

void MyViewer::run_arm_animation() {

	upperArmRot = M_PI / 4.0f;
	lowerArmRot = -M_PI / 3.0f;
	handRot = M_PI / 12.0f;

	if (_animating) return; // avoid recursive calls
	_animating = true;


	double frdt = 1.0 / 30.0; // delta time to reach given number of frames per second
	double t = 0, lt = 0, t0 = gs_time();
	do // run for a while:
	{
		while (t - lt<frdt) { ws_check(); t = gs_time() - t0; } // wait until it is time for next frame
		lt = t;

		if (t < 2) {
			if (t < 1)
				upperArmRot += 0.02f;
			else
				upperArmRot -= 0.02f;
				update_arm();
		}
		if (t > 4 && t < 6) {
			lowerArmRot += 0.04f;
			handRot += 0.04f;
			update_arm();
		}

		if (t > 8) {
			upperArmRot += 0.08f;
			lowerArmRot -= 0.04f;
			handRot -= 0.04f;
			update_arm();
		}
		render(); // notify it needs redraw
		ws_check(); // redraw now
	} while (t < 11);
	_animating = false;

}

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation ()
{
	if ( _animating ) return; // avoid recursive calls
	_animating = true;
	
	int ind = gs_random ( 0, rootg()->size()-1 ); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();

	double frdt = 1.0/30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t=0, lt=0, t0=gs_time();
	do // run for a while:
	{	while ( t-lt<frdt ) { ws_check(); t=gs_time()-t0; } // wait until it is time for next frame
		double yinc = (t-lt)*v;
		if ( t>2 ) yinc=-yinc; // after 2 secs: go down
		lt = t;
		m.e24 += (float)yinc;
		if ( m.e24<0 ) m.e24=0; // make sure it does not go below 0
		manip->initial_mat ( m );
		render(); // notify it needs redraw
		ws_check(); // redraw now
	}	while ( m.e24>0 );
	_animating = false;
}

void MyViewer::show_normals ( bool b )
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for ( int k=0; k<r->size(); k++ )
	{	SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if ( !b ) { l->visible(false); continue; }
		l->visible ( true );
		if ( !l->empty() ) continue; // build only once
		l->init();
		if ( s->instance_name()==SnPrimitive::class_name )
		{	GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face ( fn );
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for ( int i=0; i<m.F.size(); i++ )
			{	const GsVec& a=m.V[m.F[i].a]; l->push ( a, a+(*n++)*f );
				const GsVec& b=m.V[m.F[i].b]; l->push ( b, b+(*n++)*f );
				const GsVec& c=m.V[m.F[i].c]; l->push ( c, c+(*n++)*f );
			}
		}  
	}
}

int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;
	float a = 0.05f;
	switch ( e.key )
	{	case GsEvent::KeyEsc : gs_exit(); return 1;

	case 113: { upperArmRot -= a; rootg()->remove_all(); build_arm_scene(); update_arm(); redraw(); return 1; }
			  //a
	case 97: { upperArmRot += a; rootg()->remove_all(); build_arm_scene(); update_arm(); redraw(); return 1; }
			 //w
	case 119: { lowerArmRot -= a; rootg()->remove_all(); build_arm_scene(); update_arm(); redraw(); return 1; }
			  //w
	case 115: { lowerArmRot += a; rootg()->remove_all(); build_arm_scene(); update_arm(); redraw(); return 1; }
			  //e
	case 101: { handRot -= a; rootg()->remove_all(); build_arm_scene(); update_arm(); redraw(); return 1; }
			  //d

	case 100: { handRot += a; rootg()->remove_all(); build_arm_scene(); update_arm(); redraw(); return 1; }
		case 'n' : { bool b=!_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
		default: gsout<<"Key pressed: "<<e.key<<gsnl;
	}

	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	case EvNormals: show_normals(_nbut->value()); return 1;
		case EvAnimate: run_arm_animation(); return 1;
		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
