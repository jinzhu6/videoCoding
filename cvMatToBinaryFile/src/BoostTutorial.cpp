//============================================================================
// Name        : BoostTutorual.cpp
// Author      : johannes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// standard includes
#include <iostream>
#include <fstream>

// openCV includes
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// boost inlcudes
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "cvmat_serilization.h"

using namespace std;
namespace io = boost::iostreams;


int main(int argc, char** argv) {
	cout << "<<< boost & openCV testing >>>" << endl;


	// verify if there is any input
    if( argc != 3)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    // input images
    cv::Mat image01;
    cv::Mat image02;

    // load and convert image into cv::Mat format
    image01 = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    image02 = cv::imread(argv[2], CV_LOAD_IMAGE_COLOR);

    // verify if the loading of the img was correct
    if(! image01.data )
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    //    boost::archive::binary_oarchive oa(ofs);
    //    //boost::archive::text_oarchive oa(ofs);
    //    oa << image;
    //
    //    ofs.close();


    // WRITE images to binary file

    std::ofstream ofs("matrices.bin", std::ios::out | std::ios::binary);

    {
      io::filtering_streambuf<io::output> out;
      out.push(io::zlib_compressor(io::zlib::best_speed));
      out.push(ofs);

      boost::archive::binary_oarchive oa(out);

      oa << image01;
      oa << image02;

    }

    ofs.close();



    // LOAD binary image

    cv::Mat loadedImg;

    std::ifstream ifs("matrices.bin", std::ios::in | std::ios::binary);

    // use scope to ensure archive and filtering stream buffer go out of scope
    // before stream
    {
    	io::filtering_streambuf<io::input> in;
    	in.push(io::zlib_decompressor());
    	in.push(ifs);

    	boost::archive::binary_iarchive ia(in);

    	bool cont = true;
    	while (cont)
    	{
    		cont = boost::serialization::try_stream_next(ia, ifs, loadedImg);


    		if(cont){
    			cout << "loading image from binary file ... "<< endl;
        	    cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE );
        	    imshow( "Display window", loadedImg );

        	    cv::waitKey(0);
    		}
    	}
    }

    ifs.close();

	return 0;
}
