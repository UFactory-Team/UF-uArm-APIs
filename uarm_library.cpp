/******************************************************************************
* File Name          : uArm_library.cpp
* Author             : Joey Song  
* Updated            : Joey Song 
* Email              : joey@ufactory.cc
* Version            : V1.2 
* Date               : 12 Dec, 2014
* Modified Date      : 17 Dec, 2015
* Description        : 
* License            : 
* Copyright(C) 2014 UFactory Team. All right reserved.
*******************************************************************************/

#include "uarm_library.h"

uArmClass uarm;
int angleR;
int angleL;
int angleBottom;
int l_movementTrigger = 0;

uArmClass::uArmClass()
{
  g_servo_offset = 0.0;
}

void uArmClass::alert(byte times, byte runTime, byte stopTime)
{
  for(int ct=0; ct < times; ct++)
  {
    delay(stopTime);
    digitalWrite(BUZZER, HIGH);
    delay(runTime);
    digitalWrite(BUZZER, LOW);
  }
}


/* The code below is written by jerry song */


void uArmClass::writeAngle(byte servo_rot_angle, byte servo_left_angle, byte servo_right_angle, byte servo_hand_rot_angle, byte trigger)
{
  if(l_movementTrigger==0)
  {
    attachAll();
  }

  g_servo_rot.write(round(servo_rot_angle));
  g_servo_left.write(round(servo_left_angle));
  g_servo_right.write(round(servo_right_angle));
  g_servo_hand_rot.write((round(servo_hand_rot_angle)));
  l_movementTrigger = 0;

}

void uArmClass::writeAngle(double servo_rot_angle, double servo_left_angle, double servo_right_angle, double servo_hand_rot_angle)
{
  if(l_movementTrigger==0)
  {
    attachAll();
  }
  
  if(servo_left_angle < 10) servo_left_angle = 10;
  if(servo_left_angle > 120) servo_left_angle = 120;
  if(servo_right_angle < 10) servo_right_angle = 10;
  if(servo_right_angle > 110) servo_right_angle = 110;

  if(servo_left_angle + servo_right_angle > 150) 
  {servo_right_angle = 150 - servo_left_angle;}

  int servo_rot_angle_execute = inputToReal(SERVO_ROT_NUM,servo_rot_angle);
  int servo_left_angle_execute = inputToReal(SERVO_LEFT_NUM,servo_left_angle);
  int servo_right_angle_execute = inputToReal(SERVO_RIGHT_NUM,servo_right_angle);
  int servo_hand_rot_angle_execute = inputToReal(SERVO_HAND_ROT_NUM,servo_hand_rot_angle);
  
  g_servo_rot.write(servo_rot_angle_execute);
  g_servo_left.write(servo_left_angle_execute);
  g_servo_right.write(servo_right_angle_execute);
  g_servo_hand_rot.write(servo_hand_rot_angle_execute);

  l_movementTrigger = 0;

}

void uArmClass::attachAll()
{
  

  g_servo_rot.attach(11);
  g_servo_left.attach(13);
  g_servo_right.attach(12);
  g_servo_hand_rot.attach(10);
}

void uArmClass::detachAll()
{
  
  g_servo_rot.detach();
  g_servo_left.detach();
  g_servo_right.detach();
  g_servo_hand_rot.detach();

}


byte uArmClass::inputToReal(byte servo_num, double input_angle)
{
  return (byte)round(constrain((input_angle + readServoOffset(servo_num)),0,180));
}

double uArmClass::readServoOffset(byte servo_num)
{


  if ((servo_num == 1)||(servo_num == 2)||(servo_num == 3))
  {
    g_servo_offset = (EEPROM.read(LINEAR_START_ADDRESS + (servo_num-1)*2 +1))/10.00;

    if (EEPROM.read(LINEAR_START_ADDRESS + (servo_num-1)*2 ) == 0)
      {g_servo_offset = - g_servo_offset;}

    return g_servo_offset;
  } 
  else if(servo_num == 4)
    return 0;
  else{
      Serial.println("Incorrect");
      
  }
}



void uArmClass::saveDataToRom(double data, int address)
{
  
  int dataWhole;
  
  byte Byte0;
  byte Byte1;
  byte Byte2;

  if(abs(data) < 1) {
    dataWhole = (int) (data*100);
  }
  else{
    dataWhole = (int) (data*10);
  }



  if (dataWhole > 0){ 
    Byte0 = 1;
  }
  else{ 
    Byte0 =0; 
  }

  dataWhole = abs(dataWhole);

  Byte1 = (( dataWhole >> 0) & 0xFF);
  Byte2 = (( dataWhole >> 8) & 0xFF);
  

  EEPROM.write( address, Byte0);
  EEPROM.write( address + 1, Byte1);
  EEPROM.write( address + 2, Byte2);


}

  
    
double uArmClass::readToAngle(double input_analog, byte servo_num, byte trigger)
{
  int address = OFFSET_START_ADDRESS +(servo_num-1)*6;

  for (int i = 0; i<6;i++){
      

    }

  double data_a = (EEPROM.read(address+1)+EEPROM.read(address+2)*256)/10.0;
  if (EEPROM.read(address)==0)
    {data_a = -data_a;}

  double data_b = (EEPROM.read(address+4)+EEPROM.read(address+5)*256)/100.0;
  if (EEPROM.read(address+3)==0)
    {data_b = -data_b;}


  if (trigger == 0){
    return (data_a + data_b*input_analog) - readServoOffset(servo_num);
  }
  if (trigger == 1){
    return (data_a + data_b*input_analog);
  }
}



double uArmClass::readAngle(byte servo_num)
{

  switch (servo_num)
  {
    case SERVO_ROT_NUM:
      
      return readToAngle(analogRead(SERVO_ROT_ANALOG_PIN),SERVO_ROT_NUM,0);
      break;

    case SERVO_LEFT_NUM:
      return readToAngle(analogRead(SERVO_LEFT_ANALOG_PIN),SERVO_LEFT_NUM,0);
      break;

    case SERVO_RIGHT_NUM:
      return readToAngle(analogRead(SERVO_RIGHT_ANALOG_PIN),SERVO_RIGHT_NUM,0);
      break;

    case SERVO_HAND_ROT_NUM:
      return readToAngle(analogRead(SERVO_HAND_ROT_ANALOG_PIN),SERVO_HAND_ROT_NUM,0);
      break;

    default:
      
      break;


  }

}

double uArmClass::readAngle(byte servo_num, byte trigger)
{

  switch (servo_num)
  {
    case SERVO_ROT_NUM:
      return readToAngle(analogRead(SERVO_ROT_ANALOG_PIN),SERVO_ROT_NUM,trigger);
      break;

    case SERVO_LEFT_NUM:
      return readToAngle(analogRead(SERVO_LEFT_ANALOG_PIN),SERVO_LEFT_NUM,trigger);
      break;

    case SERVO_RIGHT_NUM:
      return readToAngle(analogRead(SERVO_RIGHT_ANALOG_PIN),SERVO_RIGHT_NUM,trigger);
      break;

    case SERVO_HAND_ROT_NUM:
      return readToAngle(analogRead(SERVO_HAND_ROT_ANALOG_PIN),SERVO_HAND_ROT_NUM,trigger);
      break;

    default:
      
      break;


  }

}

/*Action control */

void uArmClass::calAngles(double x, double y, double z)
{

   if (z > (MATH_L1 + MATH_L3 + TopOffset))
    {
        z = MATH_L1 + MATH_L3 + TopOffset;
    }

    if (z < (MATH_L1 - MATH_L4 + BottomOffset))
    {
        z = MATH_L1 - MATH_L4 + BottomOffset;
    }

  double x_in = 0.0;
  double y_in = 0.0;
  double z_in = 0.0;
  double right_all = 0.0;
  double right_all_2 = 0.0;
  double sqrt_z_x = 0.0;
  double sqrt_z_y = 0.0;
  double phi = 0.0;
  
  y_in = (-y-MATH_L2)/MATH_L3;
  z_in = (z - MATH_L1) / MATH_L3;
  right_all = (1 - y_in*y_in - z_in*z_in - MATH_L43*MATH_L43) / (2 * MATH_L43);
  sqrt_z_y = sqrt(z_in*z_in + y_in*y_in);
  

  if (x == 0)
  {
    // Calculate value of theta 1
    g_theta_1 = 90;

    // Calculate value of theta 3
    if (z_in == 0) {
      phi = 90;
    }

    else {
    phi = atan(-y_in / z_in)*MATH_TRANS;
    }

    if (phi > 0) phi = phi - 180;

    g_theta_3 = asin(right_all / sqrt_z_y)*MATH_TRANS - phi;
    if(g_theta_3<0)
      {
        g_theta_3 = 0;
      }

    // Calculate value of theta 2
    
    g_theta_2 = asin((z + MATH_L4*sin(g_theta_3 / MATH_TRANS) - MATH_L1) / MATH_L3)*MATH_TRANS;
  }

  else
  {
    // Calculate value of theta 1

    g_theta_1 = atan(y / x)*MATH_TRANS;

    if (y / x > 0) {
      g_theta_1 = g_theta_1;
    }
    if (y / x < 0) {
      g_theta_1 = g_theta_1 + 180;
    }
    if (y == 0) {
      if (x > 0) g_theta_1 = 180;
      else g_theta_1 = 0;       
    }
    
    // Calculate value of theta 3

    x_in = (-x / cos(g_theta_1 / MATH_TRANS) - MATH_L2) / MATH_L3;

    if (z_in == 0){ phi = 90; }
      
    else{ phi = atan(-x_in / z_in)*MATH_TRANS; }
      
    if (phi > 0) {phi = phi - 180;}  
    
    sqrt_z_x = sqrt(z_in*z_in + x_in*x_in);

    right_all_2 = -1 * (z_in*z_in + x_in*x_in + MATH_L43*MATH_L43 - 1) / (2 * MATH_L43);
    g_theta_3 = asin(right_all_2 / sqrt_z_x)*MATH_TRANS;
    g_theta_3 = g_theta_3 - phi;

    if (g_theta_3 <0 ) {
      g_theta_3 = 0;
    }


    // Calculate value of theta 2

    g_theta_2 = asin(z_in + MATH_L43*sin(abs(g_theta_3 / MATH_TRANS)))*MATH_TRANS;

  }
  
    
  g_theta_1 = abs(g_theta_1);
  g_theta_2 = abs(g_theta_2);
  
  
  if (g_theta_3 < 0 ){}
  else{
    if ((calYonly(g_theta_1,g_theta_2, g_theta_3)>y+0.1)||(calYonly(g_theta_1,g_theta_2, g_theta_3)<y-0.1))
    {
      g_theta_2 = 180 - g_theta_2;
    }  
  }
  

  if(isnan(g_theta_1)||isinf(g_theta_1))
    {g_theta_1 = uarm.readAngle(1);}
  if(isnan(g_theta_2)||isinf(g_theta_2))
    {g_theta_2 = uarm.readAngle(2);}
  if(isnan(g_theta_3)||isinf(g_theta_3)||(g_theta_3<0))
    {g_theta_3 = uarm.readAngle(3);}

}

void uArmClass::interpolation(double init_val, double final_val, double (&interpol_val_array)[INTERP_INTVL])
{
  // by using the formula theta_t = l_a_0 + l_a_1 * t + l_a_2 * t^2 + l_a_3 * t^3
  // theta(0) = init_val; theta(t_f) = final_val
  // theta_dot(0) = 0; theta_dot(t_f) = 0

  double l_a_0;
  double l_a_1;
  double l_a_2;
  double l_a_3;
  double l_t_step;
  
  byte l_time_total = 1;

  l_a_0 = init_val;
  l_a_1 = 0;
  l_a_2 = (3 * (final_val - init_val)) / (l_time_total*l_time_total);
  l_a_3 = (-2 * (final_val - init_val)) / (l_time_total*l_time_total*l_time_total);

  for (byte i = 0; i < INTERP_INTVL; i=i+1)
  {
    l_t_step = (l_time_total / (float)INTERP_INTVL) *i;
    interpol_val_array[i] = l_a_0 + l_a_1 * (l_t_step) + l_a_2 * (l_t_step *l_t_step ) + l_a_3 * (l_t_step *l_t_step *l_t_step);  
  }
}

void uArmClass::calXYZ(double theta_1, double theta_2, double theta_3)
{
  
  // g_l3_1 = MATH_L3 * cos(theta_2 / MATH_TRANS);
  // g_l4_1 = MATH_L4*cos(theta_3 / MATH_TRANS);
  double l5 = (MATH_L2 + MATH_L3*cos(theta_2 / MATH_TRANS) + MATH_L4*cos(theta_3 / MATH_TRANS));

  g_cal_x = -cos(abs(theta_1 / MATH_TRANS))*l5;
  g_cal_y = -sin(abs(theta_1 / MATH_TRANS))*l5;
  g_cal_z = MATH_L1 + MATH_L3*sin(abs(theta_2 / MATH_TRANS)) - MATH_L4*sin(abs(theta_3 / MATH_TRANS));

}

void uArmClass::calXYZ()
{
  
  calXYZ(
  uarm.readToAngle(analogRead(2),1,0),
  uarm.readToAngle(analogRead(0),2,0),
  uarm.readToAngle(analogRead(1),3,0));

}

void uArmClass::moveTo(double x, double y, double z)
{
  uarm.attachAll();
  
  double l_current_x;
  double l_current_y;
  double l_current_z;
  double l_current_hand;

  double x_arr[INTERP_INTVL];
  double y_arr[INTERP_INTVL];
  double z_arr[INTERP_INTVL];

  calXYZ();
  l_current_x = g_cal_x;
  l_current_y = g_cal_y;
  l_current_z = g_cal_z;
  l_current_hand = readAngle(SERVO_HAND_ROT_NUM);
  if (l_current_hand < 0) l_current_hand = 0;

  interpolation(l_current_x, x, x_arr);

  interpolation(l_current_y, y, y_arr);

  interpolation(l_current_z, z, z_arr); 
    
  for (byte i = 0; i < INTERP_INTVL; i++)
  {
    calAngles(x_arr[i],y_arr[i],z_arr[i]);

    l_movementTrigger = 1;
    uarm.writeAngle(g_theta_1, g_theta_2, g_theta_3, l_current_hand);

    delay(40);

  }

}

void uArmClass::moveTo(double x, double y, double z, int relative, double time_spend)
{
  uarm.attachAll();

  double x_arr[INTERP_INTVL];
  double y_arr[INTERP_INTVL];
  double z_arr[INTERP_INTVL];

  calXYZ();
  double current_x = g_cal_x;
  double current_y = g_cal_y;
  double current_z = g_cal_z;
  double current_hand = readAngle(SERVO_HAND_ROT_NUM);
  if (current_hand < 0) current_hand = 0;

  if ((relative !=0)&&(relative != 1))
  { 
    relative = 0;

  }

  // if (time_spend <0)
  // {  
  //  time_spend = abs(time_spend);
    
  // }

  interpolation(current_x, current_x*relative+x, x_arr);

  interpolation(current_y, current_y*relative+y, y_arr);

  interpolation(current_z, current_z*relative+z, z_arr); 

  for (byte i = 0; i < INTERP_INTVL; i++)
  {
    calAngles(x_arr[i],y_arr[i],z_arr[i]);
    l_movementTrigger = 1;
    uarm.writeAngle(g_theta_1, g_theta_2, g_theta_3, current_hand);

    delay(time_spend*1000/INTERP_INTVL);

  }

}


void uArmClass::moveToAtOnce(double x, double y, double z, int relative, double servo_4_angle)
{
  uarm.attachAll();
  
  double x_arr[INTERP_INTVL];
  double y_arr[INTERP_INTVL];
  double z_arr[INTERP_INTVL];
  

  calXYZ();
  double current_x = g_cal_x;
  double current_y = g_cal_y;
  double current_z = g_cal_z;

  if ((relative !=0)&&(relative != 1))
  { 
    relative = 0;
  }

  calAngles(current_x*relative+x,current_y*relative+y,current_z*relative+z);
  l_movementTrigger = 1;
  uarm.writeAngle(g_theta_1, g_theta_2, g_theta_3, servo_4_angle);


}

void uArmClass::moveTo(double x, double y, double z, int relative, double time_spend, int servo_4_relative, double servo_4_angle)
{
  uarm.attachAll();

  double x_arr[INTERP_INTVL];
  double y_arr[INTERP_INTVL];
  double z_arr[INTERP_INTVL];
  double h_arr[INTERP_INTVL];

  calXYZ();
  double current_x = g_cal_x;
  double current_y = g_cal_y;
  double current_z = g_cal_z;
  double current_hand = readAngle(SERVO_HAND_ROT_NUM);
  if (current_hand < 0) current_hand = 0;

  if ((relative !=0)&&(relative != 1))
  { 
    relative = 0;
  }

  if ((servo_4_relative !=0)&&(servo_4_relative != 1))
  { 
    servo_4_relative = 0;
  }

  if (time_spend <0)
  { 
    time_spend = abs(time_spend);
  }

  interpolation(current_x, current_x*relative+x, x_arr);

  interpolation(current_y, current_y*relative+y, y_arr);

  if ( current_z*relative+z>25)
    { interpolation(current_z, 25, z_arr); }
  else
    { interpolation(current_z, current_z*relative+z, z_arr); }

  interpolation(current_hand, current_hand*servo_4_relative+servo_4_angle, h_arr);
    
  for (byte i = 0; i < INTERP_INTVL; i++)
  {
    calAngles(x_arr[i],y_arr[i],z_arr[i]);
    l_movementTrigger = 1;
    uarm.writeAngle(g_theta_1, g_theta_2, g_theta_3, h_arr[i]);

    delay(time_spend*1000/INTERP_INTVL);
  }

}



void uArmClass::drawRec(double length_1, double length_2, double time_spend_per_length)
{

  moveTo(length_1,0,0,1,time_spend_per_length);
  moveTo(0,length_2,0,1,time_spend_per_length);
  moveTo(-length_1,0,0,1,time_spend_per_length);
  moveTo(0,-length_2,0,1,time_spend_per_length);
  
}

void uArmClass::drawCur(double length_1, double length_2, int angle, double time_spend)
{
  uarm.attachAll();
  double l_xp;
  double l_yp;
  
  calXYZ();
  double current_x = g_cal_x;
  double current_y = g_cal_y;
  double current_z = g_cal_z;
  double interp_arr[INTERP_INTVL];
  
  interpolation(0, angle/MATH_TRANS, interp_arr); 

  for (byte i = 0; i < INTERP_INTVL; i++){

    l_xp = length_1 * cos(interp_arr[i]);
    l_yp = length_2 * sin(interp_arr[i]);

    calAngles( l_xp + current_x - length_1 , l_yp+ current_y , current_z);
    l_movementTrigger = 1;
    uarm.writeAngle(g_theta_1, g_theta_2, g_theta_3,0);

    delay(time_spend*1000/INTERP_INTVL);
  
  }

}

double uArmClass::calYonly(double theta_1, double theta_2, double theta_3)

{

    //g_l3_1_2 = MATH_L3 * cos(theta_2 / MATH_TRANS);
    //g_l4_1_2 = MATH_L4*cos(theta_3 / MATH_TRANS);
    double l5_2 = (MATH_L2 + MATH_L3*cos(theta_2 / MATH_TRANS) + MATH_L4*cos(theta_3 / MATH_TRANS));

    return -sin(abs(theta_1 / MATH_TRANS))*l5_2;

}

void uArmClass::gripperCatch()
{
  g_servo_hand.attach(SERVO_HAND);
  g_servo_hand.write(HAND_ANGLE_CLOSE);
  digitalWrite(VALVE_EN, LOW); // valve disnable
  digitalWrite(PUMP_EN, HIGH); // pump enable
  g_gripper_reset = true;
}

void uArmClass::gripperRelease()
{
  if(g_gripper_reset)
  {
    g_servo_hand.attach(SERVO_HAND);
    g_servo_hand.write(HAND_ANGLE_OPEN);
    digitalWrite(VALVE_EN, HIGH); // valve enable, decompression
    digitalWrite(PUMP_EN, LOW);   // pump disnable
    g_gripper_reset= false;
  }
}
/* Action Control */
void uArmClass::pumpOn()
{

   pinMode(PUMP_EN, OUTPUT);
   pinMode(VALVE_EN, OUTPUT);
   digitalWrite(VALVE_EN, LOW);
   digitalWrite(PUMP_EN, HIGH);
}

void uArmClass::pumpOff()
{
   pinMode(PUMP_EN, OUTPUT);
   pinMode(VALVE_EN, OUTPUT);
   digitalWrite(VALVE_EN, HIGH);
   digitalWrite(PUMP_EN, LOW);
   delay(50);
   digitalWrite(VALVE_EN,LOW);
}
