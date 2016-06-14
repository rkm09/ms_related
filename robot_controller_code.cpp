// raaRobotController.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgUtil/UpdateVisitor>
#include <osgViewer/ViewerEventHandlers>

osg::Node *g_pModel = 0;

//Class 'nodePrinter': prints all the node related info in a hierarchical manner . 
class nodePrinter : public osg::NodeVisitor
{
public:
	nodePrinter() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {};

	virtual void apply(osg::Node &node)
	{
		std::string sSpace = "";
		for (int i = 0; i < getNodePath().size(); i++)sSpace += "--/";

		std::cout << sSpace << "Name : " << "[ " << node.getName() << " ]" << " || ";
		std::cout << "Lib Object Name : " << "[ " << node.className() << " ]" << " || " << "Reference Count : " << "[ " << node.referenceCount() << " ]" << std::endl;
		std::cout << std::endl;
		traverse(node);
	}

protected:
};

//Class 'nodeFinder' : used to find the matrix transforms of various components and return the address if found.
class nodeFinder : public osg::NodeVisitor
{
public:
	nodeFinder(std::string sName, std::string sType) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		m_sName = sName;
		pTransform = 0;
	};

	virtual void apply(osg::Node &node)
	{
		traverse(node);
	}

	virtual void apply(osg::MatrixTransform &matrixTransform)
	{
		//traverse(matrixTransform);

		if (m_sName == matrixTransform.getName())
		{
			std::cout << "Found" << matrixTransform.getName() << std::endl;
			//			m_sFound = true;
			pTransform = &matrixTransform;

		}
		traverse(matrixTransform);
	}

	osg::MatrixTransform* get()
	{
		return pTransform;
	}

protected:
	std::string m_sName;
	osg::MatrixTransform *pTransform;
};


//Class 'myNodeUpdator':inherits from NodeCallback class and carries out rotation/translation of the desired part.
class myNodeUpdator : public osg::NodeCallback
{
public:
	bool key_up = false;
	bool key_down = false;
	bool key_arrow_up = false;
	bool key_arrow_down = false;
	bool key_arrow_right = false;
	bool key_arrow_left = false;

	myNodeUpdator() // constructor
	{
		m_bRotate = false;
		m_fAngle = 0.0f;
		m_ftransl = 0.0f;
		m_ftransl_x = 0.0f;
		m_ftransl_y = 0.0f;
		max_rot = 0.0f;
		min_rot = 0.0f;
	}


	myNodeUpdator(osg::MatrixTransform *pTrans, osg::Vec3f vAxis, osg::Switch* pSwitch1) // constructor
	{
		if (pTrans)
		{
			m_vAxis = vAxis;
			m_pMT = pTrans;
			m_mInitialMatrix = m_pMT->getMatrix();
			m_bRotate = false;
			pSwitch = pSwitch1;

		}
		else
			m_pMT = 0;
		m_fAngle = 0.0f;
		m_ftransl = 0.0f;
		m_ftransl_x = 0.0f;
		m_ftransl_y = 0.0f;
		max_rot = 0.0f;
		min_rot = 0.0f;
	};

	virtual void operator()(osg::Node *pNode, osg::NodeVisitor *pNV)
	{

		if (m_pMT && (m_bRotate || m_btran))
		{
			pSwitch->setSingleChildOn(1);

			if (m_pMT && m_bRotate)
			{
				std::cout << "Operator Called..." << std::endl;
				//pSwitch->setSingleChildOn(1);
				std::cout << "max rot>>>>>>..." << max_rot << " min rot : " << min_rot << std::endl;
				osg::Matrixf rotMatrix;

				if (key_down)
				{
					if (m_fAngle >= min_rot)rotMatrix.makeRotate(osg::DegreesToRadians(m_fAngle -= 0.3f), m_vAxis);
					else rotMatrix.makeRotate(osg::DegreesToRadians(m_fAngle), m_vAxis);
				}
				else if (key_up)
				{
					if (m_fAngle <= max_rot)rotMatrix.makeRotate(osg::DegreesToRadians(m_fAngle += 0.3f), m_vAxis);
					else rotMatrix.makeRotate(osg::DegreesToRadians(m_fAngle), m_vAxis);
				}

				std::cout << m_fAngle << std::endl;

				m_pMT->setMatrix(m_mInitialMatrix * rotMatrix);

				std::cout << pSwitch << std::endl;

			}
			else if (m_pMT && m_btran)
			{
				pSwitch->setSingleChildOn(1);
				osg::Matrixf translateMatrix;
				std::cout << "translate body....val :( " << m_ftransl_x << "," << m_ftransl_y << ",0.0)" << std::endl;
				if (key_arrow_up){
					if (m_ftransl_x < 9.90)
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x += 0.02f, m_ftransl_y, 0.0f));
					else {
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x, m_ftransl_y, 0.0f));
					}
				}
				else if (key_arrow_down)
				{
					if (m_ftransl_x > -9.20)
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x += -0.02f, m_ftransl_y, 0.0f));
					else {
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x, m_ftransl_y, 0.0f));
					}
				}
				else if (key_arrow_left){
					if (m_ftransl_y < 7.30)
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x, m_ftransl_y += 0.02f, 0.0f));
					else {
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x, m_ftransl_y, 0.0f));
					}
				}
				else if (key_arrow_right){
					if (m_ftransl_y > -9.20)
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x, m_ftransl_y += -0.02f, 0.0f));
					else {
						translateMatrix.makeTranslate(osg::Vec3f(m_ftransl_x, m_ftransl_y, 0.0f));
					}
				}
				m_pMT->setMatrix(m_mInitialMatrix * translateMatrix);
			}
		}
		else
		{
			pSwitch->setSingleChildOn(0);
			pNV->traverse(*pNode);
		}

	}


	void rotate(bool bState)
	{
		m_bRotate = bState;
	};
	void translate(bool btran){
		m_btran = btran;

	}
	void set_zAxis(bool body){

		m_vAxis = osg::Vec3f(0.0f, 0.0f, 1.0f);
	}
	void set_maxRot(float angle){
		max_rot = angle;
	}
	void set_minRot(float angle){
		min_rot = angle;
	}
protected:
	osg::MatrixTransform *m_pMT;
	osg::Matrixf m_mInitialMatrix;
	osg::Vec3f m_vAxis;
	float m_fAngle;
	bool m_bRotate;
	osg::Switch* pSwitch;
	bool m_btran;
	float m_ftransl;
	float m_ftransl_y;
	float m_ftransl_x;
	float max_rot;
	float min_rot;
};


std::vector<std::string> v = { "LowerArmRotator", "UpperArmRotator", "HandRotator", "BodyRotator" };
myNodeUpdator *g_pUpdators[] = { 0, 0, 0, 0 };
osg::MatrixTransform *g_pMT_oh;

//Class 'myNodeUpdator_objects': is the updator class for table objects collisions.
class myNodeUpdator_objects : public osg::NodeCallback
{
public:

	myNodeUpdator_objects(){
		m_ftransl = 0.0f;
		m_collided = false;
		m_object_held = false;
	}

	myNodeUpdator_objects(osg::MatrixTransform *pTrans, osg::Switch* pSwitch1) // constructor
	{

		nodeFinder finder1("HandRotator", "MatrixTranform");
		finder1.traverse(*g_pModel);
		if (finder1.get()){
			m_pMT_h = finder1.get();
		}
		else
		{
			m_pMT_h = 0;
		}
		nodeFinder finder2("Table1TopLocator", "MatrixTranform");
		finder2.traverse(*g_pModel);
		if (finder2.get()){
			std::cout << "Found .. Table--" << std::endl;
			m_pMT_t = finder2.get();
		}
		else
		{
			m_pMT_t = 0;
			m_ftransl = 0.0f;
		}


		if (pTrans)
		{

			m_pMT = pTrans;
			m_mInitialMatrix = m_pMT->getMatrix();
			pSwitch = pSwitch1;
		}
		else
			m_pMT = 0;
		m_ftransl = 0.0f;


	};

	virtual void operator()(osg::Node *pNode, osg::NodeVisitor *pNV)
	{

		if (m_pMT && m_pMT_h)
		{

			osg::BoundingSphere bound_o = pNode->computeBound();
			osg::BoundingSphere bound_h = m_pMT_h->computeBound();


			osg::Matrix m_o = osg::computeLocalToWorld(pNV->getNodePath());
			osg::NodePathList npl_h = m_pMT_h->getParentalNodePaths(g_pModel);
			osg::Matrix m_h = osg::computeLocalToWorld(npl_h.front());

			//osg::BoundingSphere g_bound_o(m_o.preMult(bound_o.center(), bound_o.radius()));
			//osg::BoundingSphere  g_bound_h(m_h.preMult(bound_h.center(), bound_h.radius()));


			osg::BoundingSphere g_bound_o;
			g_bound_o.set(m_o.preMult(bound_o.center()), bound_o.radius());

			osg::BoundingSphere g_bound_h;
			g_bound_h.set(m_h.preMult(bound_h.center()), bound_h.radius());

			if (g_bound_o.intersects(g_bound_h))
			{
				std::cout << "collision occurred ..........." << std::endl;
				pSwitch->setSingleChildOn(1);
				m_collided = true;
			}
			else
			{
				pSwitch->setSingleChildOn(0);
				//pNV->traverse(*pNode);
				m_collided = false;
			}
		}


	}

	void setPickUp(bool pick){
		if (pickup == false){

			pickup = pick;
			setTree();
			pickup = false;
		}
	}
	void setPutDown(bool put){

		if (putdown == false){

			putdown = put;
			setTree();
			putdown = false;
		}
	}
	void setTree(){
		if (pickup && m_collided){
			std::cout << "Did we reach here for pickup change..." << std::endl;
			m_pMT_h->addChild(m_pMT);
			m_pMT_t->removeChild(m_pMT);
			std::cout << "changing tree structure..." << std::endl;
			m_object_held = true;
		}
		else if (putdown && m_object_held) {
			std::cout << "Did we reach here for putdown change..." << std::endl;
			m_pMT_t->addChild(m_pMT);
			m_pMT_h->removeChild(m_pMT);
			std::cout << "revert tree structure..." << std::endl;
			//m_collided = false;
			m_object_held = false;
		}
	}

protected:
	osg::MatrixTransform *m_pMT;
	osg::Switch* pSwitch;
	osg::MatrixTransform *m_pMT_h;
	osg::MatrixTransform *m_pMT_t;
	osg::Matrixf m_mInitialMatrix;
	float m_ftransl;
	bool pickup;
	bool putdown;
	bool m_collided;
	bool m_object_held;
};
std::vector<std::string> g_vec_obj = { "Object1Locator", "Object2Locator", "Object3Locator" };
myNodeUpdator_objects *g_pUpdators_objects[] = { 0, 0, 0 };

//Class 'myEventHandler' : inherits from GUIEventHandler class of osgGA .It defines the user generated interaction handling mechanism.
class myEventHandler : public osgGA::GUIEventHandler
{
public:

	myEventHandler(){}
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *pObject, osg::NodeVisitor *pNodeVisitor)
	{
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
		{
			switch (ea.getKey())
			{
			case 'k':
				std::cout << "lower arm up key pressed...." << std::endl;
				if (g_pUpdators[0]) {
					g_pUpdators[0]->rotate(true); g_pUpdators[0]->set_maxRot(10.0); g_pUpdators[0]->key_up = true;
				} break;
			case 'K':
				std::cout << "lower arm down key pressed...." << std::endl;
				if (g_pUpdators[0]) {
					g_pUpdators[0]->rotate(true); g_pUpdators[0]->set_minRot(-50.0); g_pUpdators[0]->key_down = true;
				} break;
			case 'u':
				std::cout << "upper arm up key pressed...." << std::endl;
				if (g_pUpdators[1]) {
					g_pUpdators[1]->rotate(true); g_pUpdators[1]->set_maxRot(45.0);  g_pUpdators[1]->key_up = true;
				} break;
			case 'U':
				std::cout << "upper arm down key pressed...." << std::endl;
				if (g_pUpdators[1]) {
					g_pUpdators[1]->rotate(true); g_pUpdators[1]->set_minRot(-10.0); g_pUpdators[1]->key_down = true;
				} break;
			case 'h':
				std::cout << "hand up key pressed...." << std::endl;
				if (g_pUpdators[2]) {
					g_pUpdators[2]->rotate(true); g_pUpdators[2]->set_maxRot(30.0); g_pUpdators[2]->key_up = true;
				} break;
			case 'H':
				std::cout << "hand down key pressed...." << std::endl;
				if (g_pUpdators[2]) {
					g_pUpdators[2]->rotate(true); g_pUpdators[2]->set_minRot(-30.0); g_pUpdators[2]->key_down = true;
				} break;
			case 'b':
				std::cout << "body up key pressed...." << std::endl;
				if (g_pUpdators[3]) {
					g_pUpdators[3]->rotate(true); g_pUpdators[3]->set_maxRot(40.0); g_pUpdators[3]->key_up = true; g_pUpdators[3]->set_zAxis(true);
				} break;
			case 'B':
				std::cout << "body down key pressed...." << std::endl;
				if (g_pUpdators[3]) {
					g_pUpdators[3]->rotate(true); g_pUpdators[3]->set_minRot(-40.0); g_pUpdators[3]->key_down = true; g_pUpdators[3]->set_zAxis(true);
				} break;
			case osgGA::GUIEventAdapter::KEY_Up:
				std::cout << "body translated forward...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(true);
					g_pUpdators[3]->key_arrow_up = true;
				}break;
			case osgGA::GUIEventAdapter::KEY_Down:
				std::cout << "body translated backward...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(true);
					g_pUpdators[3]->key_arrow_down = true;
				}break;
			case osgGA::GUIEventAdapter::KEY_Left:
				std::cout << "body translated left...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(true);
					g_pUpdators[3]->key_arrow_left = true;
				}break;
			case osgGA::GUIEventAdapter::KEY_Right:
				std::cout << "body translated right...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(true);
					g_pUpdators[3]->key_arrow_right = true;
				}break;
			case 'p':
				std::cout << "Pick up object 1...." << std::endl;
				if (g_pUpdators_objects[1]) {
					g_pUpdators_objects[1]->setPickUp(true);
					
				}
				break;
			case 'P':
				std::cout << "Put down object 1...." << std::endl;
				if (g_pUpdators_objects[1]) {
					g_pUpdators_objects[1]->setPutDown(true);
					//myNodeUpdator_objects::pickup = false;
				}
				break;
			case 'r':
				std::cout << "Pick up object 2...." << std::endl;
				if (g_pUpdators_objects[0]) {
					g_pUpdators_objects[0]->setPickUp(true);
					
				}
				break;
			case 'R':
				std::cout << "Put down object 2...." << std::endl;
				if (g_pUpdators_objects[0]) {
					g_pUpdators_objects[0]->setPutDown(true);
					//myNodeUpdator_objects::pickup = false;
				}
				break;
			case 't':
				std::cout << "Pick up object 3...." << std::endl;
				if (g_pUpdators_objects[2]) {
					g_pUpdators_objects[2]->setPickUp(true);
					
				}
				break;
			case 'T':
				std::cout << "Put down object 3...." << std::endl;
				if (g_pUpdators_objects[2]) {
					g_pUpdators_objects[2]->setPutDown(true);
					
				}
				break;
			default: std::cout << ".." << std::endl;

			}

		}
		break;
		case osgGA::GUIEventAdapter::KEYUP:
		{
			switch (ea.getKey())
			{
			case 'k':
				std::cout << "lower arm up key released...." << std::endl;
				if (g_pUpdators[0]) {
					g_pUpdators[0]->rotate(false); g_pUpdators[0]->key_up = false;
				} break;
			case 'K':
				std::cout << "lower arm down key released...." << std::endl;
				if (g_pUpdators[0]) {
					g_pUpdators[0]->rotate(false); g_pUpdators[0]->key_down = false;
				} break;
			case 'u':
				std::cout << "upper arm up key released...." << std::endl;
				if (g_pUpdators[1]) {
					g_pUpdators[1]->rotate(false); g_pUpdators[1]->key_up = false;
				} break;
			case 'U':
				std::cout << "upper arm down key released...." << std::endl;
				if (g_pUpdators[1]) {
					g_pUpdators[1]->rotate(false); g_pUpdators[1]->key_down = false;
				} break;
			case 'h':
				std::cout << "hand up key released...." << std::endl;
				if (g_pUpdators[2]) {
					g_pUpdators[2]->rotate(false); g_pUpdators[2]->key_up = false;
				} break;
			case 'H':
				std::cout << "hand down key released...." << std::endl;
				if (g_pUpdators[2]) {
					g_pUpdators[2]->rotate(false); g_pUpdators[2]->key_down = false;
				} break;
			case 'b':
				std::cout << "body up key released...." << std::endl;
				if (g_pUpdators[3]) {
					g_pUpdators[3]->rotate(false); g_pUpdators[3]->key_up = false;
				} break;
			case 'B':
				std::cout << "body down key released...." << std::endl;
				if (g_pUpdators[3]) {
					g_pUpdators[3]->rotate(false); g_pUpdators[3]->key_down = false;
				} break;
			case osgGA::GUIEventAdapter::KEY_Up:
				std::cout << "body translated forward...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(false);
					g_pUpdators[3]->key_arrow_up = false;
				}break;
			case osgGA::GUIEventAdapter::KEY_Down:
				std::cout << "body translated backward...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(false);
					g_pUpdators[3]->key_arrow_down = false;
				}break;
			case osgGA::GUIEventAdapter::KEY_Left:
				std::cout << "body translated left...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(false);
					g_pUpdators[3]->key_arrow_left = false;
				}break;
			case osgGA::GUIEventAdapter::KEY_Right:
				std::cout << "body translated right...." << std::endl;
				if (g_pUpdators[3])
				{
					g_pUpdators[3]->translate(false);
					g_pUpdators[3]->key_arrow_right = false;
				}break;



			default: std::cout << ".." << std::endl;

			}
		}
		break;
		}
		return false;
	}
};

// function to run before rendering starts -> use this to identify nodes
void init()
{
	if (g_pModel)
	{
		/*Printer*/
		std::cout << "Loaded Model" << std::endl;
		nodePrinter printer;


		/*Call Finder and then add switch node for alternate traversal route , finally add callback to enact the modifications when an event occurs.*/
		for (int i = 0; i < v.size(); i++)
		{
			nodeFinder finder(v[i], "MatrixTransform");
			finder.traverse(*g_pModel);

			if (finder.get())
			{

				osg::Group *pG1 = new osg::Group();
				osg::Group *pG2 = new osg::Group();
				osg::Switch *pSwitch = new osg::Switch();
				pSwitch->addChild(pG1);
				pSwitch->addChild(pG2);
				pSwitch->setSingleChildOn(0);

				osg::MatrixTransform *pMT = finder.get();
				for (int i = 0; i < pMT->getNumChildren(); i++)
				{
					osg::Group *pG = dynamic_cast<osg::Group*>(pMT->getChild(i));
					if (pG && pG->getName() == "model")
					{

						pG1->addChild(pG);
						pG2->addChild(pG);
					}
				}
				for (int i = 0; pG1->getNumChildren(); i++)
				{

					pMT->removeChild(pG1->getChild(i)); break;
				}
				pMT->addChild(pSwitch);

				//render-highlight in line mode when in rotation mode
				osg::StateSet *pSS = pG2->getOrCreateStateSet();
				pSS->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE), osg::StateAttribute::ON || osg::StateAttribute::OVERRIDE);



				osg::Vec3f axis(0, 1, 0);

				finder.get()->addUpdateCallback(g_pUpdators[i] = new myNodeUpdator(finder.get(), axis, pSwitch));
			}
		}

		//for each of the table objects add switch to change rendering mode to line when in collision with hand
		for (int i = 0; i < g_vec_obj.size(); i++){
			nodeFinder finder(g_vec_obj[i], "MatrixTransform");
			finder.traverse(*g_pModel);


			if (finder.get()){

				std::cout << "Found : " << g_vec_obj[i] << std::endl;

				osg::Group *pG1 = new osg::Group();
				osg::Group *pG2 = new osg::Group();
				osg::Switch *pSwitch = new osg::Switch();
				pSwitch->addChild(pG1);
				pSwitch->addChild(pG2);
				pSwitch->setSingleChildOn(0);

				osg::MatrixTransform *pMT = finder.get();
				for (int i = 0; i < pMT->getNumChildren(); i++)
				{
					osg::Group *pG = dynamic_cast<osg::Group*>(pMT->getChild(i));
					if (pG && pG->getName() == "model")
					{

						pG1->addChild(pG);
						pG2->addChild(pG);
					}
				}
				for (int i = 0; pG1->getNumChildren(); i++)
				{

					pMT->removeChild(pG1->getChild(i)); break;
				}
				pMT->addChild(pSwitch);

				//render
				osg::StateSet *pSS = pG2->getOrCreateStateSet();
				pSS->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE), osg::StateAttribute::ON || osg::StateAttribute::OVERRIDE);


				finder.get()->addUpdateCallback(g_pUpdators_objects[i] = new myNodeUpdator_objects(finder.get(), pSwitch));
			}

		}

		/*Traverse the printer*/
		printer.traverse(*g_pModel);

	}
	else
	{
		std::cout << " Failed to load a Model" << std::endl;
	}
}

int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc, argv);

	// load model
	g_pModel = osgDB::readNodeFiles(arguments);
	g_pModel->ref();

	if (!g_pModel) return 0;

	init();

	// setup viewer
	osgViewer::Viewer viewer;

	osg::GraphicsContext::Traits *pTraits = new osg::GraphicsContext::Traits();
	pTraits->x = 20;
	pTraits->y = 20;
	pTraits->width = 600;
	pTraits->height = 480;
	pTraits->windowDecoration = true;
	pTraits->doubleBuffer = true;
	pTraits->sharedContext = 0;

	osg::GraphicsContext *pGC = osg::GraphicsContext::createGraphicsContext(pTraits);
	osgGA::KeySwitchMatrixManipulator* pKeyswitchManipulator = new osgGA::KeySwitchMatrixManipulator();
	pKeyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
	pKeyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
	pKeyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
	viewer.setCameraManipulator(pKeyswitchManipulator);
	osg::Camera *pCamera = viewer.getCamera();
	pCamera->setGraphicsContext(pGC);
	pCamera->setViewport(new osg::Viewport(0, 0, pTraits->width, pTraits->height));


	viewer.addEventHandler(new myEventHandler);

	// add the state manipulator
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	// add the thread model handler
	viewer.addEventHandler(new osgViewer::ThreadingHandler);

	// add the window size toggle handler
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	// add the stats handler
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// add the record camera path handler
	viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

	// add the LOD Scale handler
	viewer.addEventHandler(new osgViewer::LODScaleHandler);

	// add the screen capture handler
	viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

	// set the scene to render
	viewer.setSceneData(g_pModel);

	viewer.realize();

	return viewer.run();

	return 0;
}
