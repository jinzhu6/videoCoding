#include <stdio.h>
#include <iostream>

/**
 * Optris interface
 */
#include "PIImager.h"

/**
 * Visualization
 */
#include "Obvious2D.h"
#include "ImageBuilder.h"

#include <string>
#include <stdlib.h>
#include "cv.h"

using namespace std;
using namespace optris;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define WIDTH    1024
#define HEIGHT   960
//#define WIDTH    960
//#define HEIGHT   600

obvious::Obvious2D _viewer(WIDTH, HEIGHT, "Optris Imager example");
ImageBuilder _iBuilder;

// by jo: buffershow stores the rgb image of the head image
unsigned char* _bufferShow = NULL;
string outputFile = "/tmp/output1.png";
optris::PIImager* _imager;

void measurePoint(unsigned short* image, unsigned int w, unsigned int h, unsigned int x, unsigned int y)
{	
	cout << "x: " << x << " y: " << y << " w: " << w << " h: " << h << endl;
	char text[20];
	_iBuilder.drawCrosshair(_bufferShow, x, y, 3);
	sprintf(text, "%2.1f", 0.1f*((float)(image[x + y*w] - 1000)));
	//printf(text, "%2.1f \n\n", 0.1f*((float)(image[x + y*w] - 1000)));

	x -= w/20;
	y += h/20;
	_viewer.addText(text, WIDTH * x/w, HEIGHT - HEIGHT * y/h);
}

void cbOnFrame(unsigned short* image, unsigned int w, unsigned int h)
{	
	// by jo: converts the heat values to color values. 
	// by jo: @_bufferShow: size = hight*width+3 (3 = rgb)
	_iBuilder.convertTemperatureToPaletteImage(image, _bufferShow);


	cout << "bufferShow[0]: " << (int)_bufferShow[w*h] << endl;
	_viewer.clearText();

	// by jo: heat information per pixel stored in an 1-dim array
	float tmp = 0.1*(float)((image[100 + 100*w]) - 1000);	
	cout << "imageValue: " <<  tmp << endl;

	measurePoint(image, w, h, w/2,   h/2  );
	measurePoint(image, w, h, w/4,   h/4  );
	measurePoint(image, w, h, w/4*3, h/4  );
	measurePoint(image, w, h, w/4,   h/4*3);
	measurePoint(image, w, h, w/4*3, h/4*3);
	
	cout << "R: "<< (int)_bufferShow[0] << endl;
	cout << "G: "<< (int)_bufferShow[1] << endl;
	cout << "B: "<< (int)_bufferShow[2] << endl;
	cout << "R: "<< (int)_bufferShow[3] << endl;
	cout << "G: "<< (int)_bufferShow[4] << endl;
	cout << "B: "<< (int)_bufferShow[5] << endl;

	// by jo: create and visualize the rgb-image
	// by jo: @param channel: selection of the visualisation mode (rgb(3), lumnicance(1))
	_viewer.draw(_bufferShow, _iBuilder.getStride(), h, 3);
}

void cbPalette()
{
	EnumOptrisColoringPalette palette = _iBuilder.getPalette();
	unsigned int val = (unsigned int)palette;
	val++;
	if(val>eAlarmRed) val = 1;
	_iBuilder.setPalette((EnumOptrisColoringPalette) val);
}

int main (int argc, char* argv[])
{
	if(argc!=2)
	{
		cout << "usage: " << argv[0] << " <xml configuration file>" << endl;
		return -1;
	}

	/**
	 * Initialize Optris image processing chain
	 */
	_imager = new PIImager(argv[1]);
	_iBuilder.setSize(_imager->getWidth(), _imager->getHeight());


	cout << "Stride: " << _iBuilder.getStride() << " Height " <<  _imager->getHeight() << endl;
	cout << "row buffer size" << _imager->getRawBufferSize() << endl;
	cout << "Max. Frame rate of camera: " << _imager->getFramerate() << endl;
	cout << "width: " << _imager->getWidth() << "height: " << _imager->getHeight() << endl;

	unsigned char* bufferRaw = new unsigned char[_imager->getRawBufferSize()];
	_bufferShow              = new unsigned char[_iBuilder.getStride() * _imager->getHeight() * 3];

	_imager->setFrameCallback(cbOnFrame);
	_viewer.registerKeyboardCallback('p', cbPalette);
	_iBuilder.setManualTemperatureRange(25.0f, 40.0f);
	_iBuilder.setPaletteScalingMethod(eMinMax);

	/**
	 * Enter endless loop in order to pass raw data to Optris image processing library.
	 * Processed data are supported by the frame callback function.
	 */
	_imager->startStreaming();
	//_imager->startRecording((char*)outputFile.c_str());

	while(_viewer.isAlive())
	{
		// by jo: getFrame set the bufferRaw pointer to the current raw frame
		_imager->getFrame(bufferRaw);
		//cout << "bufferRaw1: " << bufferRaw[1] << endl;
		// by jo: process the raw frame date which the bufferRaw pointer points to
		_imager->process(bufferRaw);
		
		_imager->releaseFrame();
		//cout << "Elapsed: " << im.elapsed() << " ms" << endl;
	}
	//_imager->stopRecording();
	delete [] bufferRaw;
	delete [] _bufferShow;
	delete _imager;

	cout << "Exiting application" << endl;
}
