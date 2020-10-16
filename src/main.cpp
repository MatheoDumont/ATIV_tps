
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/video/background_segm.hpp>
#include <iostream>
#include "kernel.h"
#include <math.h>
#include "seuil.h"
#include "path_contour.h"
#include "contour.h"
#include <cassert>

#define IMAGE_NAME0 "datas/square_sample.png"
#define IMAGE_NAME1 "datas/Palpa1.jpg"
#define IMAGE_NAME2 "datas/Palpa2.jpg"
#define IMAGE_NAME3 "datas/mr_piuel.jpeg"
#define IMAGE_NAME4 "datas/square_sample_0.png"
/*
* Partie 1 Tp 1 Appliquer convolution sur Image
*/

int main(int argc, char *argv[])
{
  float f[3][3] = {1 / 3.f, 0.f, -1 / 3.f, 1 / 3.f, 0.f, -1 / 3.f, 1 / 3.f, 0.f, -1 / 3.f};

  cv::Mat filtre = cv::Mat(3, 3, CV_32F, &f);
  // std::cout << filtre << std::endl;
  std::vector<cv::Mat> filtres;
  filtres.push_back(filtre);

  // cv::Mat image = cv::imread("datas/square_sample.png", CV_LOAD_IMAGE_GRAYSCALE);
  cv::Mat image;
  if (argc > 1)
    image = cv::imread(argv[1]);
  else
    image = cv::imread(IMAGE_NAME2);
  if (!image.data)
  {
    std::cerr << "Error : image or video not found" << std::endl;
    exit(-1);
  }

  cv::Mat im,gs; // for greyscale image

  cv::cvtColor(image, im, cv::COLOR_BGR2GRAY);

  // std::cout << cv::typeToString(gs.type()) << std::endl;
  // std::cout << gs.type() << " " << CV_32F << std::endl;
  // std::cout << cv::typeToString(filtre.type()) << std::endl;

  // cv::imshow("Image_from_path", image);
  // cv::waitKey(0);
  //
  // cv::imshow("greyscale", gs);
  // cv::waitKey(0);
  //
  // // cv::Mat gs_float;
  im.convertTo(gs, CV_32F);
  // cv::Mat convol = Kernel::conv2(gs, filtres)[0];
  //
  // // cv::Mat convol_uchar;
  // convol.convertTo(convol_uchar, CV_8UC1);
  // cv::imshow("convoluted", convol);
  // cv::waitKey(0);

  // { // gradient tests with amplitude and angle
  //   std::vector<cv::Mat> gradient_filters;
  //   cv::Mat grad0 = (cv::Mat_<float>(3, 3) << 1.0/3, 0, -1.0/3, 1.0/3, 0, -1.0/3, 1.0/3, 0, -1.0/3);
  //   gradient_filters.push_back(grad0);
  //   cv::Mat grad2 = (cv::Mat_<float>(3, 3) << -1.0/3, -1.0/3, -1.0/3, 0, 0, 0, 1.0/3, 1.0/3, 1.0/3);
  //   gradient_filters.push_back(grad2);
  //
  //   std::vector<cv::Mat> gradient_convol = Kernel::conv2(gs, gradient_filters);
  //   cv::imshow("amplitude_0 grad", Kernel::amplitude_0(gradient_convol)*(1/255.0));
  //   cv::imshow("amplitude_1 grad", Kernel::amplitude_1(gradient_convol)*(1/255.0));
  //   cv::imshow("amplitude_2 grad", Kernel::amplitude_2(gradient_convol)*(1/255.0));
  //   cv::waitKey(0);
  //
  //   cv::Mat gradient_color = Kernel::color_gradient_im(Kernel::amplitude_0(gradient_convol),
  // 					       Kernel::angle_arctan(gradient_convol[0],
  // 							     gradient_convol[1]));
  //   cv::imshow("color gradient", gradient_color*(1/255.0));
  //   cv::waitKey(0);
  // }

  {
    // gradients 4 directions
    std::vector<cv::Mat> gradient_filters;
    cv::Mat horizontal = (cv::Mat_<float>(3, 3) << -1 / 3.f, 0.f, 1 / 3.f, -1 / 3.f, 0.f, 1 / 3.f, -1 / 3.f, 0.f, 1 / 3.f);
    cv::Mat vertical = (cv::Mat_<float>(3, 3) << 1 / 3.f, 1 / 3.f, 1 / 3.f, 0.f, 0.f, 0.f, -1 / 3.f, -1 / 3.f, -1 / 3.f);
    cv::Mat quart_plus = (cv::Mat_<float>(3, 3) << 1.f / 3.f, 1.f / 3.f, 0.f, 1.f / 3.f, 0.f, -1.f / 3.f, 0.f, -1.f / 3.f, -1.f / 3.f);
    cv::Mat quart_moins = (cv::Mat_<float>(3, 3) << 0.f, 1.f / 3.f, 1.f / 3.f, -1.f / 3.f, 0.f, 1.f / 3.f, -1.f / 3.f, -1.f / 3.f, 0.f);

    gradient_filters.push_back(horizontal);
    gradient_filters.push_back(quart_moins);
    gradient_filters.push_back(vertical);
    gradient_filters.push_back(quart_plus);

    std::vector<cv::Mat> gradient_convol = Kernel::conv2(gs, gradient_filters);
		cv::Mat amp0 = Kernel::amplitude_0(gradient_convol);
    cv::Mat dir = Kernel::angle(gradient_convol);
    cv::Mat gradient_color = Kernel::color_gradient_im(amp0, dir * (M_PI / 4));

    cv::imshow("color gradient", gradient_color * (1 / 255.0));
    cv::waitKey(0);

    // TEST SEUIL GLOBAL
    // cv::imshow("seuillage global 4 directions - 0.025", Seuil::seuil_global(amp0 * (1 / 255.0), 0.025));
    // cv::imshow("seuillage global 4 directions - 0.05", Seuil::seuil_global(amp0 * (1 / 255.0), 0.05));
    cv::imshow("seuillage global 4 directions - 0.1", Seuil::seuil_global(amp0 * (1 / 255.0), 0.1));
    cv::waitKey(0);
		// cv::destroyAllWindows();

		// TEST SEUIL LOCAL
    /*
		cv::imshow("seuillage local 4 directions", Seuil::seuil_local(amp0 * (1 / 255.0), 7, 1.2));
    cv::waitKey(0);
		*/

		// TEST SEUIL HYSTERESIS
    // cv::Mat seuille = Seuil::seuil_hysteresis(amp0 * (1 / 255.0), 0.07, 0.16,5);
    // cv::imshow("seuillage hysteresis 4 directions 0.07-0.16 - rad1", Seuil::seuil_hysteresis(amp0 * (1 / 255.0), 0.07, 0.16,1));
    // cv::imshow("seuillage hysteresis 4 directions 0.07-0.16 - rad3", Seuil::seuil_hysteresis(amp0 * (1 / 255.0), 0.07, 0.16,3));
    // cv::imshow("seuillage hysteresis 4 directions 0.07-0.16 - rad5", seuille);
    // cv::waitKey(0);


		// TEST SEUIL PATH_CONTOUR
    cv::imshow("seuillage path 4 directions 0.025-0.09 gradient", Path::path_contour(amp0 * (1 / 255.0), dir, 0.025, 0.10, true));
    cv::imshow("seuillage path 4 directions 0.025-0.09 classique", Path::path_contour(amp0 * (1 / 255.0), dir, 0.025, 0.10, false));
    cv::waitKey(0);
    cv::destroyAllWindows();

    // TEST AFFINAGE CONTOUR SIMPLE
    cv::Mat seuille = Seuil::seuil_global(amp0 * (1 / 255.0), 0.1);
		gradient_color = Kernel::color_gradient_im(seuille * 255.0, dir * (M_PI / 4));

    cv::imshow("color gradient", gradient_color* (1 / 255.0));
    assert(dir.type() == CV_32F && amp0.type() == CV_32F);
    cv::imshow("Affinage contour simple", Contour::affinage_max_loc(seuille, dir, amp0));
    cv::waitKey(0);
		// cv::destroyAllWindows();

		// TEST FERMETURE DES contours
		std::vector<std::pair<int, int>> mask0{{1, -1}, {1, 0}, {1, 1},
																					 {0, -1}, {0, 0}, {0, 1},
																				 	 {-1,-1}, {-1,0}, {-1,1}};
 		std::vector<std::pair<int, int>> mask1 {
				  	{2, -1},{2, 0},{2, 1},
		{1, -2},{1, -1},{1, 0},{1, 1},{1, 2},
 		{0, -2},{0, -1},{0, 0},{0, 1},{0, 2},
 		{-1,-2},{-1,-1},{-1,0},{-1,1},{-1,2},
					 	{-2,-1},{-2,0},{-2,1}
		};
		// cv::imshow("Fermeture dil_ero it:1,1 mask0", Contour::fermeture_dil_ero(seuille, mask1,mask0,1,1));
		// cv::imshow("Fermeture dil_ero it:2,2 mask0", Contour::fermeture_dil_ero(seuille, mask1,mask0,2,2));
		// cv::imshow("Fermeture dil_ero it:2,1 mask0", Contour::fermeture_dil_ero(seuille, mask1,mask0,2,1));
		// cv::imshow("Fermeture dil_ero it:1,0 mask0", Contour::fermeture_dil_ero(seuille, mask1,mask0,3,3));
		// cv::waitKey(0);
	  // cv::destroyAllWindows();

    // Dilatation locale
    cv::Mat dilate = Contour::dilatation_contour(seuille, dir, 2);
    cv::imshow("Dilatation locale", dilate);
    cv::waitKey(0);

    // avec affinage ensuite
    cv::imshow("Affinage dilation locale", Contour::affinage_max_loc(dilate, dir, amp0));
    cv::waitKey(0);
  }




  cv::destroyAllWindows();
}
