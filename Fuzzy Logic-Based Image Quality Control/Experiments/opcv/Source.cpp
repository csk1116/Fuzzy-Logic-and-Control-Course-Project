
//#define _SCL_SECURE_NO_WARNINGS

#include<iostream>
#include<string>
#include<sstream>
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp> 
#include<opencv2/opencv.hpp>
#include<time.h>
//#include<stdio.h>
//#include<stdlib.h>
#include"SerialPort.h"
#include"opencv2/videoio.hpp"





using namespace cv;
using namespace std;



const char*keys =
{
	"{help h usage ? || print this message}"
	"{@video || Video file, if not defined try to use webcamera}"
};

//fuzzy_control function
double fuzzy_control(double input1, double input2, double input3, double input4);

//arduino
//String for getting the output from arduino
//char output[MAX_DATA_LENGTH];

/*Portname must contain these backslashes, and remember to
replace the following com port*/
const char *port_name = "\\\\.\\COM5";

//String for incoming data
//char incomingData[MAX_DATA_LENGTH];

int main(int argc, const char**argv)
{
	CommandLineParser parser(argc, argv, keys);
	parser.about("Chaptr2");

	if (parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	String videoFile = parser.get<String>(0);

	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}

	//video cap

	VideoCapture cap;

	if (videoFile != "")
		cap.open(videoFile);
	else
		cap.open(1);
	if (!cap.isOpened())
		return -1;

	//output the video
	VideoWriter writer("FUZZY_CONTROL_8.MOV", CV_FOURCC('M', 'J', 'P', 'G'), 28.0, Size(640, 480),0);



	//show
	//namedWindow("LED Image Quality Fuzzy Control", 1);
	namedWindow("FUZZY_FINAL_PROJECT", 1);


	//timer
	int loop_time = 1;
	time_t start, end;
	double elapsed;
	start = time(NULL);

	//how many frames
	int num_frame = 0;
	//double count = 0;
	//double fps = 30;
	//double T = 0;
	double w = 0.75;
	double wp = 0.75;
	double wg = 10;
	

	// create array
	double *gmean_array = NULL;
	gmean_array = new double[600];

	double *frames_array = NULL;
	frames_array = new double[600];

	double *SNR_array = NULL;
	SNR_array = new double[600];

	double *entropy_array = NULL;
	entropy_array = new double[600];

	//pd fuzzy e and er
	double *E_array = NULL;
	E_array = new double[600];

	double *ER_array = NULL;
	ER_array = new double[600];

	double E;
	double ER;
	
	

	//fuzzy
	double PWM;
	//double PWMS;

	double *PWM_array = NULL;
	PWM_array = new double[600];

	//arduino
	SerialPort arduino(port_name);
	if (arduino.isConnected()) cout << "Connection Established" << endl;
	else cout << "ERROR, check port name";

	int PWMI;
	

	while (loop_time && arduino.isConnected())
	{

		//timer
		end = time(NULL);
		elapsed = difftime(end, start);
		if (elapsed >= 30.0) {
			loop_time = 0;
		}

	
		Mat grayscale;
		Mat frame;
		bool bSuccess = cap.read(frame);
		if (!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		//bgr to gray
		cv::cvtColor(frame, grayscale, COLOR_BGR2GRAY);
		
		//imshow("LED Image Quality Fuzzy Control", frame);
		imshow("FUZZY_FINAL_PROJECT", grayscale);

		//meangray
		Vec<double,1>  gmean, gstd;
		cv::meanStdDev(grayscale, gmean, gstd);

		//snr
		Vec<double,1> SNR;
		SNR = 20 * log10(gmean[0] / gstd[0]);

		//entropy
		int histSize = 256;
		float range[] = { 0 , 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		

		Mat hist ;
		cv::calcHist(&grayscale, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);
		//probibility
		Mat hist_double;
		hist.convertTo(hist_double, CV_64F);
		//hist_double /= grayscale.total();
		hist_double /= 307200;
		//double a = grayscale.total();
		hist_double.setTo(1, hist == 0);
		Mat logP;
		cv::log(hist_double, logP);
		Mat hist_log = hist_double.mul(logP);
        Vec<double,4> entropy = -1*sum(hist_log);

		
		*(gmean_array + num_frame) = (double)(gmean[0]);

		//ORDER TWO LOW PASS FILTER
		if (num_frame >= 2)
		{
			gmean[0] = ((wg*wg / (wg*wg + 2*wg + 1))*gmean[0] + ((2 * wg + 2) / (wg*wg + 2*wg + 1))*gmean_array[num_frame - 1] - gmean_array[num_frame - 2] / (wg*wg + 2*wg + 1));
		}
		else
		{
			gmean[0];
		}
		
		
		
		//calculate e and er
		
		E = 128.000 - gmean[0];
		
		//ORDER ONE LOW PASS FILTER (w=corner frequency)
		/*if (num_frame!=0)
		{

		E = (1 / (w + 1))*E_array[num_frame - 1] + (w / (w + 1))*(128.000-gmean[0]);
		}

		else
		{
		E = 128.000-gmean[0];
		}*/

		//ORDER TWO LOW PASS FILTER
		/*if (num_frame>=2)
		{
		E = ((w / (w*w + w + 1))*(128.000-gmean[0])+((2*w+2)/ (w*w + w + 1))*E_array[num_frame-1] - E_array[num_frame-2]/ (w*w + w + 1));
		}
		else
		{
		E = 128.000-gmean[0];
		}*/


		
		

		//cout << "mean grayvalue: "<<gmean[0]<<"  SNR: "<<SNR[0]<<"  Entropy: "<<entropy[0]<<"  Time:"<<elapsed<<"  #frame:"<<num_frame<<endl;
		
		//store Vec into array
		
		

		*(SNR_array + num_frame) = (double)(SNR[0]);

		*(entropy_array + num_frame) = (double)(entropy[0]);
		
		//*(frames_array + num_frame) = (double)(num_frame);

		*(E_array + num_frame) = (double)(E);

		
		/*if (num_frame!=0)
		{
			ER = -1 * (gmean[0] - gmean_array[num_frame - 1]);
		}
		
		else
		{
			ER = 0;
		}*/

		//ORDER ONE LOW PASS FILTER (w=corner frequency)
		/*if (num_frame!=0)
		{
			
			ER = ((1 / (w + 1))*ER_array[num_frame - 1] + (w / (w + 1))*(gmean_array[num_frame-1]-gmean[0]));
		}
	    
		else
		{
			ER = 0;
		}*/

		//ORDER TWO LOW PASS FILTER
		if (num_frame>=2)
		{
			ER = ((w*w / (w*w + 2*w + 1))*(gmean_array[num_frame - 1] - gmean[0])+((2*w+2)/ (w*w + 2*w + 1))*ER_array[num_frame-1] - ER_array[num_frame-2]/ (w*w + 2*w + 1));
		}
		else
		{
			ER = 0;
		}

		*(ER_array + num_frame) = (double)(ER);

		//fuzzy_output
		//PWM = fuzzy_control(E, ER, SNR[0], entropy[0]);

		//ORDER ONE LOW PASS FILTER (w=corner frequency)
		/*if (num_frame!=0)
		{

		PWM = (1 / (wp + 1))*PWM_array[num_frame - 1] + (wp / (wp + 1))*fuzzy_control(E, ER, SNR[0], entropy[0]);
		}

		else
		{
		PWM = 128.000;
		}
*/
		//ORDER TWO LOW PASS FILTER
		if (num_frame>=2)
		{
		PWM = ((w*w / (w*w + 2*w + 1))*fuzzy_control(E, ER, SNR[0],entropy[0])+((2*w+2)/ (w*w + 2*w + 1))*PWM_array[num_frame-1] - PWM_array[num_frame-2]/ (w*w + 2*w + 1));
		}
		else
		{
		PWM = 128.000;
		}
		
		
		*(PWM_array + num_frame) = (double)(PWM);

		PWMI = round(PWM);
			
	
		
		cout << "#Frames: " << num_frame << "  Mean_Grayvalue: " << gmean[0] << "  SNR: " << SNR[0] << "  Entropy: " << entropy[0] << "  PWM: " << PWM << /*"  PWMI: "<<PWMI<<*/"  E: "<<E<<"  ER: "<<ER<<endl;
		
		//send to arduino
		//cout << "Write something: \n";
		//std::string input_string;
		//Getting input
		//getline(cin, input_string);
		//Creating a c string
		//char *c_string = new char[input_string.size() + 1];
		//copying the std::string to c string
		//std::copy(input_string.begin(), input_string.end(), c_string);
		//Adding the delimiter
		//c_string[input_string.size()] = '\n';
		//Writing string to arduino
		arduino.writeSerialPort(PWMI);
		//Getting reply from arduino
		//arduino.readSerialPort(output, MAX_DATA_LENGTH);
		//printing the output
		//puts(output);
		//freeing c_string memory
		//delete[] c_string;

		num_frame = num_frame + 1;

		writer << grayscale;


		if (waitKey(30) >= 0)
		break;

	}

	

	
	int a = 0;

	delete PWM_array;
	delete gmean_array;
	delete E_array;
	delete ER_array;
	delete SNR_array;
	delete entropy_array;

	//system("pause");
	
	cap.release();
	return 0;



}