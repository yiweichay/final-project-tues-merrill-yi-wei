#include <xc.h>
#include <math.h>
#include "color.h"
#include "i2c.h"

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Clear(void){
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at Clear low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop();          //Stop condition
    return tmp;
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void){
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at GREEN low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop(); 
    return tmp;
}

unsigned int color_read_Blue(void){
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at BLUE low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop();          //Stop condition
    return tmp;
}

void read_colours(struct RGB_val *m){
    (m->C) = color_read_Clear();
    __delay_ms(1);
    (m->R) = color_read_Red();
    __delay_ms(1);
    (m->G) = color_read_Green();
    __delay_ms(1);
    (m->B) = color_read_Blue();
    __delay_ms(1);
    return;
}

void read_color_sensor(struct RGB_val *m) // Redundant 
{
	unsigned int tmp;
    // Red
    I2C_2_Master_Start();
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();
    (m->R) = tmp;
    
    // Green
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at GREEN low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop(); 
    (m->G) = tmp;
    
    // Blue
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at BLUE low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop();          //Stop condition
    (m->B) = tmp;
    
    return;
}

//unsigned int determine_color(struct RGB_val *m){
//    unsigned int data[3][9][3] = {
//        {{ // White LED 
//        {3150,1390,775}, // 0: Red - problem
//        {2385,1660,795}, // 1: Green
//        {1670,1162,599}, // 2: Blue
//        {2671,1473,737}, // 3: Yellow - problem
//        {2853,1391,787}, // 4: Pink
//        {2422,991,564}, // 5: Orange - problem
//        {2225,1507,788}, // 6: Light Blue
//        {2702,1654,888}, // 7: White
//        {1658,951,467}   // 8: Black
//         }},
//         {{ // Red LED 
//        {2449,158,300}, // 0: Red - problem
//        {1737,168,235}, // 1: Green
//        {1692,147,239}, // 2: Blue
//        {2544,250,334}, // 3: Yellow - problem
//        {2414,237,248}, // 4: Pink
//        {2553,177,316}, // 5: Orange - problem
//        {2221,233,335}, // 6: Light Blue
//        {2576,278,391}, // 7: White
//        {1610,104,190}   // 8: Black
//         }},
//         {{ // Green LED 
//        {415,901,344}, // 0: Red - problem
//        {299,1129,426}, // 1: Green
//        {256,1107,430}, // 2: Blue
//        {515,1383,501}, // 3: Yellow - problem
//        {475,1226,481}, // 4: Pink
//        {445,997,377}, // 5: Orange - problem
//        {415,1417,559}, // 6: Light Blue
//        {535,1580,633}, // 7: White
//        {213,938,329}   // 8: Black
//         }}
//    }; // Blue LED does not work 
//         
//    unsigned int temp = 1000;
//    unsigned int out;
//    unsigned int testresults[3] = {0,0,0};
//    
//    for (unsigned int i=0; i<3; i++){
//        if (i==0) {LATGbits.LATG1=1;LATAbits.LATA4=1;LATFbits.LATF7=1;} // White
//        if (i==1) {LATGbits.LATG1=1;LATAbits.LATA4=0;LATFbits.LATF7=0;} // Red 
//        if (i==2) {LATGbits.LATG1=0;LATAbits.LATA4=1;LATFbits.LATF7=0;} // Green
//        for (unsigned int j=0; j<9; j++) {
//            unsigned int rr = (data[i][j][0]-(m->R))*(data[i][j][0]-(m->R));
//            unsigned int gg = (data[i][j][1]-(m->G))*(data[i][j][1]-(m->G));
//            unsigned int bb = (data[i][j][2]-(m->B))*(data[i][j][2]-(m->B));
//            unsigned int calc = sqrt(rr + gg + bb);
//            if (calc < temp) { // get the closest value
//                temp = calc; 
//                if (calc < 20){testresults[i] = j;} // threshold similarity
//                else {testresults[i] = 9;} // means ambient 
//            }
//        }
//        __delay_ms(50);
//    }
//    
//   // test condition: if more than 2 same number, return number else inconclusive 
//    int count = sizeof(testresults) / sizeof(testresults[0]);
//    for (int i = 0; i < count - 1; i++) { // read comment by @nbro
//        for (int j = i + 1; j < count; j++) {
//            if (testresults[i] == testresults[j]) {
//                out = testresults[i]; // 2 tests same 
//            }
//        }
//    }
//    return out;
//}

unsigned int determine_color1(struct RGB_val *m){
    unsigned int data[9][3] = { // White test
        {47,1390,775}, // 0: Red - problem
        {2385,1660,795}, // 1: Green
        {1670,1162,599}, // 2: Blue
        {2671,1473,737}, // 3: Yellow - problem
        {2853,1391,787}, // 4: Pink
        {2422,991,564}, // 5: Orange - problem
        {2225,1507,788}, // 6: Light Blue
        {2702,1654,888}, // 7: White
        {47,23,18}   // 8: Black
    };
         
    unsigned int temp = 1000;
    unsigned int out;
    
    for (unsigned int j=0; j<9; j++) {
        unsigned int rr = (data[j][0]-(m->R))*(data[j][0]-(m->R));
        unsigned int gg = (data[j][1]-(m->G))*(data[j][1]-(m->G));
        unsigned int bb = (data[j][2]-(m->B))*(data[j][2]-(m->B));
        unsigned int calc = sqrt(rr + gg + bb);
        if (calc < temp) { // get the closest value
            temp = calc; 
            if (calc < 30){out = j;} // threshold similarity
            else {out = 9;} // means ambient 
        }
    }
    return out;
}

unsigned int determine_color2(struct RGB_val *m){
    unsigned int data[9][3] = { // Red test
        {2449,158,300}, // 0: Red - problem
        {1737,168,235}, // 1: Green
        {1692,147,239}, // 2: Blue
        {2544,250,334}, // 3: Yellow - problem
        {2414,237,248}, // 4: Pink
        {2553,177,316}, // 5: Orange - problem
        {2221,233,335}, // 6: Light Blue
        {2576,278,391}, // 7: White
        {818,382,193}   // 8: Black
    };
         
    unsigned int temp = 1000;
    unsigned int out;
    
    for (unsigned int j=0; j<9; j++) {
        unsigned int rr = (data[j][0]-(m->R))*(data[j][0]-(m->R));
        unsigned int gg = (data[j][1]-(m->G))*(data[j][1]-(m->G));
        unsigned int bb = (data[j][2]-(m->B))*(data[j][2]-(m->B));
        unsigned int calc = sqrt(rr + gg + bb);
        if (calc < temp) { // get the closest value
            temp = calc; 
            if (calc < 30){out = j;} // threshold similarity
            else {out = 9;} // means ambient 
        }
    }
    return out;
}

unsigned int determine_color3(struct RGB_val *m){
    unsigned int data[9][3] = { // Red test
        {415,901,344}, // 0: Red - problem
        {299,1129,426}, // 1: Green
        {256,1107,430}, // 2: Blue
        {515,1383,501}, // 3: Yellow - problem
        {475,1226,481}, // 4: Pink
        {445,997,377}, // 5: Orange - problem
        {415,1417,559}, // 6: Light Blue
        {535,1580,633}, // 7: White
        {1177,244,183}   // 8: Black
    };
         
    unsigned int temp = 1000;
    unsigned int out;
    
    for (unsigned int j=0; j<9; j++) {
        unsigned int rr = (data[j][0]-(m->R))*(data[j][0]-(m->R));
        unsigned int gg = (data[j][1]-(m->G))*(data[j][1]-(m->G));
        unsigned int bb = (data[j][2]-(m->B))*(data[j][2]-(m->B));
        unsigned int calc = sqrt(rr + gg + bb);
        if (calc < temp) { // get the closest value
            temp = calc; 
            if (calc < 30){out = j;} // threshold similarity
            else {out = 9;} // means ambient 
        }
    }
    return out;
}

float determine_color_new(struct RGB_val *m){         
    unsigned int temp = 1000;
    unsigned int out;
    float RedRatio, GreenRatio, BlueRatio, Hue;
    float volatile RatioMax, RatioMin, Saturation;
    
    RedRatio = ((float)(m->R) / (float)(m->C));
    GreenRatio = ((float)(m->G) / (float)(m->C));
    BlueRatio = ((float)(m->B) / (float)(m->C));
    
    RatioMax = MAX_FLOAT(MAX_FLOAT(RedRatio, GreenRatio), BlueRatio);
    RatioMin = MIN_FLOAT(MIN_FLOAT(RedRatio, GreenRatio), BlueRatio);
    
    if (RatioMax > 0)
        Saturation = (RatioMax - RatioMin) / RatioMax;
    else
        Saturation = 0;

    if (Saturation == 0)
        Hue = 0;
    else 
    {
        if (RatioMax == RedRatio)
            Hue = (GreenRatio - BlueRatio) / (RatioMax - RatioMin);
        else if (RatioMax == GreenRatio)
            Hue = 2 + (BlueRatio - RedRatio) / (RatioMax - RatioMin);
        else
            Hue = 4 + (RedRatio - GreenRatio) / (RatioMax - RatioMin);

        Hue = Hue / 6;

        if (Hue < 0)
            Hue += 1;
    }
    
    return Hue; 
}