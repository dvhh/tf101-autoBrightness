#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
//#include <time.h>


void msleep(const int millisecs) {
	usleep(millisecs*1000);
	/*
	struct timespec req;
	time_t sec=millisecs/1000;
	req.tv_sec = sec;
	req.tv_nsec = millisecs % 1000 * 1000000L;
	while( nanosleep( &req, &req ) == -1 ) {
		continue;
	}
	*/
}


double calcBrighness( const double lux, const double min, const double max ) {
	double result = min;
	if( lux == 0 ) {
		return result;
	}
	result = ( max - min ) * ( log(lux) / log(10000) ) + min;
	if( result < min ) {
		result = min;
	}
	return result;
}

int readLuminosity() {
	FILE* input = fopen( SENSOR_PATH, "r" );
	int result = -1;
	if( input != NULL ) {
		if( fscanf( input, "%i", &result ) != 1 ) {
			perror( "Error reading sensor value" );
			result = -1;
		}
		fclose( input );
	}else{
		perror( "Error reading sensor value" );
		return -1;
	}
	return result;
}

int readBrightness() {
	FILE* input = fopen( BRIGHTNESS_CONTROL, "r" );
	int result = -1;
	if( input != NULL ) {
		if( fscanf( input, "%i", &result ) != 1 ) {
			perror( "Error reading brightness value" );
			result = -1;
		}
		fclose( input );
	}else{
		perror( "Error reading brightness value" );
		return -1;
	}
	return result;
}

bool writeBrightness( const int value ) {
	FILE* output = fopen( BRIGHTNESS_CONTROL, "w" );
	//fprintf(stderr,"writing %i\n",value);
	bool result = false;
	if( output != NULL ) {
		if( fprintf( output, "%i", value ) < 0 ) {
			perror( "Error setting brightness value" );
		}else{
			result = true;
		}
		fclose(output);
	}else{
		perror( "Error setting brightness value" );
		return false;
	}
	return result;
}
/*
bool writeBrightnessFd(const int value) {
	int fd=open( BRIGHTNESS_CONTROL, O_WRONLY );
	bool result=false;
	if(fd != -1) {
	}else{
	}
	return result;
}
*/
double transitionCurve(const double v1, const double v2, const double steps, const double position) {
	// TODO : Research about brighness transition curve recommendation
	const double delta = ( v2 - v1 ) / steps;
	return v1 + delta * position; 
}

int main( int argc, char** argv ) {
	double values[HISTORY_DEPTH];
	int index=0;
	bool changed;
	int currentBrightness = readBrightness();
	for(int i=0;i<HISTORY_DEPTH;i++) {
		values[i]=currentBrightness;
	}
	double oldLuminosity=0;
	double targetBrightness=currentBrightness;
	while( true ) {
		changed=false;
		int luminosity = readLuminosity();
		if( luminosity != -1 ) {
			if(luminosity!=oldLuminosity) {
				oldLuminosity=luminosity;
				targetBrightness=calcBrighness(luminosity, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
				//fprintf(stderr,"%lf\n",targetBrightness);
			}
			values[index++]=targetBrightness;
			index %= HISTORY_DEPTH;
			double mean = 0.0L;
			for(int i=0;i<HISTORY_DEPTH;i++) {
				mean += values[i];
				//fprintf(stderr,"%i\t%lf\t%lf\n",i,values[i],mean);
			}
			mean /= HISTORY_DEPTH;
			if(fabs(currentBrightness-mean)>1.0) {
				changed=true;
				//fprintf(stderr,"%lf\t%lf\n",currentBrightness,mean);
				currentBrightness=mean;
				writeBrightness(mean);
			}
		}
		if(!changed) {
			//fprintf(stderr,"not changed\n");
			msleep(1000);
		}else{
			//fprintf(stderr,"changed\n");
			msleep(100);
		}
	}
}

