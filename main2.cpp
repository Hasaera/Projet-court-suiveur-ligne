#include "mbed.h"
#include <math.h>
 
PwmOut E1(p22);
PwmOut E2(p23);
PwmOut M1(p21);
PwmOut M2(p24);
InterruptIn button(D3);
DigitalOut led(LED1);
DigitalOut flash(LED4);

AnalogIn photoresistor1(p20);
AnalogIn photoresistor2(p19);
AnalogIn photoresistor3(p18);
AnalogIn photoresistor4(p17);
AnalogIn photoresistor5(p16);

Serial pc(USBTX, USBRX);

float max_vol[5];
float min_vol[5];

void update_min_max(float * value) {
	
	for (int i=0; i < 5; i++) {
		
		if (value[i] >= max_vol[i]) {
			max_vol[i] = value[i];
		} 
		
		if (value[i] <= min_vol[i]) {
			min_vol[i] = value[i];
		}
		
	}
	
}

void read_photoresistor(float * vol) {
		
  vol[0] = photoresistor1.read();
	vol[1] = photoresistor2.read();
	vol[2] = photoresistor3.read();
	vol[3] = photoresistor4.read();
	vol[4] = photoresistor5.read();

}

int read_line(float * vol) {

	read_photoresistor(vol);
	update_min_max(vol);
	
	int lumi[5];
	
	for (int i = 0; i < 5; i++) {
		
		lumi[i] = floor((vol[i] - min_vol[i]) / (max_vol[i] - min_vol[i]) * 1000);
		
	}
	
	int pond_sum = 0;
	int sum = 0;
	
	for (int i = 0; i < 5; i++) {
		
		sum += lumi[i];
		pond_sum += lumi[i] * i * 1000;
		
	}
	
	return pond_sum/sum;

}

void puissance_moteurs(float gauche, float droite) {
	
	E1 = gauche;
	E2 = droite;
	
}

 
int main() {
		
  float vol[5];
	int proport;
	int last_proport = 0;
	int derive;
	int integrale = 0;
	float max = 1.0f;
	//E1 = 0.2f;
	//E2 = 0.2f;
	M1 = 0;
	M2 = 1;
	
	while(1){
		
		int position = read_line(vol);
		
		last_proport = proport;
		
		derive = proport - last_proport;
		proport = position - 2000;
		integrale += proport;
		
		last_proport = proport;
		
		int diff_moteurs = (proport/500);			//+ integrale/10000 + derive*3/2);
		
		if (diff_moteurs > max) {
			diff_moteurs = max;
		} 
		if (diff_moteurs < -max) {
			diff_moteurs = -max;
		}

		if (diff_moteurs == 0) {
			puissance_moteurs(max, max);
		} else if (diff_moteurs < 0) {
			puissance_moteurs(max+diff_moteurs, max);
		} else {
			puissance_moteurs(max, max-diff_moteurs);
		}
		
		pc.printf("Position : %d\r\n", proport);
		
	}
	
}
