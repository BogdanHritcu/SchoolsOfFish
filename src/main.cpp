#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <cmath>
#include <iostream>

#include "utils/utils.h"
#include "entities/boid.h"
#include "interface/interface.h"

int WIDTH = 1080;
int HEIGHT = 720;
MouseStats mouseStats;

float old_time;
float current_time;
float delta_time;

BoidSystem boidSystem;

UserInterface userInterface(&mouseStats);

void init()
{
	srand((unsigned int)time(nullptr));

	Vec4f clearColor = color256to1(Vec4f(150, 158, 224));

	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, WIDTH, HEIGHT, 0.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// models
	BoidGroup::initModels();
	UserInterface::initModels();

	boidSystem.setBoidBoundary(Boundary2f(0.0f, 0.0f, static_cast<float>(WIDTH), static_cast<float>(HEIGHT)));
	boidSystem.setBoidBoundaryRepel(Vec2f(15.0f, 15.0f));

	BoidGroup* boidGroup;
	boidGroup = &boidSystem.addGroup(50);
	boidGroup->setBoidSize(Vec2f(15.0f, 5.0f));
	boidGroup->setBoidFriendliness(0.0f);
	boidGroup->setBoidViewDistance(60.0f);
	boidGroup->setBoidMinSeparationDistance(15.0f);
	boidGroup->setBoidMaxSpeed(100.0f);
	boidGroup->setBoidColor(Vec4f(0.0f, 1.0f, 0.0f));

	boidGroup = &boidSystem.addGroup(300);
	boidGroup->setBoidSize(Vec2f(15.0f, 5.0f));
	boidGroup->setBoidFriendliness(0.1f);
	boidGroup->setBoidViewDistance(60.0f);
	boidGroup->setBoidMinSeparationDistance(15.0f);
	boidGroup->setBoidMaxSpeed(100.0f);
	boidGroup->setBoidColor(Vec4f(0.0f, 0.0f, 1.0f));

	boidGroup = &boidSystem.addGroup(300);
	boidGroup->setBoidSize(Vec2f(15.0f, 5.0f));
	boidGroup->setBoidFriendliness(0.1f);
	boidGroup->setBoidViewDistance(60.0f);
	boidGroup->setBoidMinSeparationDistance(15.0f);
	boidGroup->setBoidMaxSpeed(100.0f);
	boidGroup->setBoidColor(Vec4f(1.0f, 0.0f, 0.0f));

	//UI
	userInterface.setPosition(Vec2f(10.0f, 10.0f));
	userInterface.setPadding(Vec2f(10.0f, 10.0f));
	userInterface.setColor(Vec4f(0.4f, 0.3f, 0.4f));
	//0.2f, 0.2f, 0.2f, 0.0f
	userInterface.setBoidSystemRef(boidSystem);

	Slider* slider;
	const size_t sliderCount = 10;
	
	Vec2f sliderRanges[sliderCount] =
	{
		Vec2f(0.0f, 1.0f), // cohesion
		Vec2f(0.0f, 1.0f), // separation
		Vec2f(0.0f, 1.0f), // alignment
		Vec2f(0.0f, 1.0f), // friendliness
		Vec2f(10.0f, 30.0f), // size.x
		Vec2f(3.0f, 15.0f), // size.y
		Vec2f(20.0f, 3000.0f), // count
		Vec2f(0.0f, 1.0f), // R
		Vec2f(0.0f, 1.0f), // G
		Vec2f(0.0f, 1.0f), // B
	};
	float sliderPercents[sliderCount] =
	{
		0.5f,
		0.5f,
		0.5f,
		0.8f,
		0.5f,
		0.5f,
		0.05f,
		rand_float(),
		rand_float(),
		rand_float()
	};

	TextBox* textBox;
	Vec2f sliderOff(0.0f, 5.0f);
	Vec2f textBoxOff(0.0f, 40.0f);
	Vec2f textLabelPosition(0.0f, 14.0f);
	Vec2f textLabelSize(96.0f, 14.0f);

	Vec2f sliderPosition = textLabelPosition + Vec2f(textLabelSize.x, 0.0f) + Vec2f(20.0f, 0.0f);
	Vec2f sliderSize(400.0f, 10.0f);

	Vec2f textValuePosition = sliderPosition + Vec2f(sliderSize.x, 0.0f) + Vec2f(10.0f, 0.0f);

	Vec2f textValueSize = Vec2f(32.0f, 14.0f);

	for (size_t i = 0; i < sliderCount - 3; i++)
	{
		// sliders
		slider = &userInterface.addSlider();
		slider->setPosition(sliderPosition + sliderOff + textBoxOff * static_cast<float>(i));
		slider->setPercent(sliderPercents[i]);
		slider->setRange(sliderRanges[i].x, sliderRanges[i].y);

		//slider->setSliderColor(Vec4f(0.1f, 0.3f, 0.5f));
		slider->setSliderColor(color256to1(Vec4f(208, 211, 143)));
		slider->setButtonColor(Vec4f(0.8f, 0.1f, 0.2f));
		slider->setSize(sliderSize);
		slider->setButtonDiameterPercent(1.5f);

		// value text boxes
		textBox = &userInterface.addTextBox();
		textBox->setPosition(textValuePosition + textBoxOff * static_cast<float>(i));

		textBox->setSize(textValueSize);
		textBox->setAutoSize(false);
		textBox->setPadding(Vec2f(6.0f, 6.0f));
		textBox->setBoxColor(color256to1(Vec4f(208, 211, 143)));
		textBox->setTextColor(Vec4f(0.2f, 0.2f, 0.2f));
	}

	Vec2f textLabelSizeCopy = Vec2f(8.0f, 14.0f);
	Vec2f sliderPositionCopy = textLabelPosition + Vec2f(textLabelSizeCopy.x, 0.0f) + Vec2f(20.0f, 0.0f);
	Vec2f sliderSizeCopy = Vec2f(200.0f, 10.0f);
	textValuePosition = sliderPositionCopy + Vec2f(sliderSizeCopy.x, 0.0f) + Vec2f(10.0f, 0.0f);
	Vec4f colors[3] =
	{
		Vec4f(0.8f, 0.2f, 0.1f),
		Vec4f(0.1f, 0.8f, 0.2f),
		Vec4f(0.2f, 0.1f, 0.8f)
	};
	size_t k = 0;

	for (size_t i = sliderCount - 3; i < sliderCount; i++, k++)
	{
		// sliders
		slider = &userInterface.addSlider();
		slider->setPosition(sliderPositionCopy + sliderOff + textBoxOff * static_cast<float>(i));
		slider->setPercent(sliderPercents[i]);
		slider->setRange(sliderRanges[i].x, sliderRanges[i].y);

		slider->setSliderColor(color256to1(Vec4f(208, 211, 143)));
		slider->setButtonColor(colors[k]);
		slider->setSize(sliderSizeCopy);
		slider->setButtonDiameterPercent(1.5f);

		// value text boxes
		textBox = &userInterface.addTextBox();
		textBox->setPosition(textValuePosition + textBoxOff * static_cast<float>(i));

		textBox->setSize(textValueSize);
		textBox->setAutoSize(false);
		textBox->setPadding(Vec2f(6.0f, 6.0f));
		textBox->setBoxColor(Vec4f(0.1f, 0.3f, 0.5f));
		textBox->setBoxColor(color256to1(Vec4f(208, 211, 143)));
		textBox->setTextColor(Vec4f(0.2f, 0.2f, 0.2f));
	}

	std::string propertiesName[sliderCount] = { "Cohesion", "Separation", "Alignment", "Friendliness", "Width", "Height", "Count", "R", "G", "B" };
	for (size_t i = 0; i < sliderCount - 3; i++)
	{
		// value text boxes
		textBox = &userInterface.addTextBox();
		textBox->setPosition(textLabelPosition + textBoxOff * static_cast<float>(i));

		textBox->setText(propertiesName[i]);
		textBox->setSize(textLabelSize);
		textBox->setAutoSize(false);
		textBox->setPadding(Vec2f(6.0f, 6.0f));
		textBox->setBoxColor(color256to1(Vec4f(208, 211, 143)));
		textBox->setTextColor(Vec4f(0.2f, 0.2f, 0.2f));
	}


	for (size_t i = sliderCount - 3; i < sliderCount; i++)
	{
		// value text boxes
		textBox = &userInterface.addTextBox();
		textBox->setPosition(textLabelPosition + textBoxOff * static_cast<float>(i));

		textBox->setText(propertiesName[i]);
		textBox->setSize(textLabelSizeCopy);
		textBox->setAutoSize(false);
		textBox->setPadding(Vec2f(6.0f, 6.0f));
		textBox->setBoxColor(color256to1(Vec4f(208, 211, 143)));
		textBox->setTextColor(Vec4f(0.2f, 0.2f, 0.2f));
	}


	////////////////////////////////////////////////////
	
	old_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}


void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	boidSystem.draw();

	userInterface.draw();

	glutSwapBuffers();
}

void idle()
{
	current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	delta_time = current_time - old_time;
	old_time = current_time;

	//printf("fps: %.2f\n", 1.0f / delta_time);
	userInterface.update();

	boidSystem.update(delta_time);

	glutPostRedisplay();
}

void click_callback(int button, int state, int x, int y)
{
	mouseStats.update(Vec2f(static_cast<float>(x), static_cast<float>(y)), button, state);

	userInterface.check();
}

void keyboard_callback(unsigned char key, int x, int y)
{

}

void mouse_position_callback(int x, int y)
{
	mouseStats.position = Vec2f(static_cast<float>(x), static_cast<float>(y));
}

void resize_callback(int width, int height)
{
	WIDTH = width;
	HEIGHT = height;

	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, WIDTH, HEIGHT, 0.0, -1.0, 1.0);

	boidSystem.setBoidBoundary(Boundary2f(Vec2f(0.0f, 0.0f), Vec2f(static_cast<float>(width), static_cast<float>(height))));
	
}

int main(int argc, char** argv)
{
	// init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Schools Of Fish");
	init();
	//glutFullScreen();

	// mouse callbacks
	glutMouseFunc(click_callback);
	glutPassiveMotionFunc(mouse_position_callback);
	glutMotionFunc(mouse_position_callback);
	glutReshapeFunc(resize_callback);

	//keyboard callbacks
	glutKeyboardFunc(keyboard_callback);

	// draw function
	glutDisplayFunc(draw);

	//idle function
	glutIdleFunc(idle);

	// main loop
	glutMainLoop();

	return 0;
}