#include <AFMotor.h>
#include <Arduino.h>
#define BTSerial Serial3


//FSR 센서 pin
const int pressureSensor = A8;     //FSR 센서 입력


//DC 모터 pin
AF_DCMotor motor0(2);
AF_DCMotor motor1(3);
AF_DCMotor motor2(4);


//초음파 센서 핀
const int trigPin2 = 30; // rail, forward
const int echoPin2 = 31;


const int trigPin3 = 28; // rail, backward
const int echoPin3 = 29;




const int trigPin1 = 32; // updown
const int echoPin1 = 33;


//초음파 센서 변수
float length_rail_fw = 0;
float distance_rail_fw = 0;
float length_rail_bw = 0;
float distance_rail_bw = 0;








//Bluetooth--start
int start_button = 0;
char BTvalue;


int pressureDefault = 0;
int sensorValue;
int pressureValue = 0;


int target_dist = 100;    //DC 모터가 내려오고 올라갈 거리 (초음파 센서로 할당)
int target_time = 100;    //목표 거리를 위해 굴러가야하는 거리


int flag_dc_down = 0;
int wheel_direction = 0;     //1.high 2.low
int temp_count = 10;
//거리 측정 관련 변수
float stopLength_prev, stopdistance_prev;
float stopLength_new, stopdistance_new;


float targetdistance = 0;
int convert_flag=0;
float finishdistance = 0;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600);
  //fsr 센서
  pinMode(pressureSensor,INPUT);
  //DC 모터
  motor0.setSpeed(200);
  motor1.setSpeed(250);
  motor2.setSpeed(250);


  //초음파 센서
  pinMode(trigPin1, OUTPUT); // trigPin을 출력으로  tirgPin1: updown US
  pinMode(echoPin1, INPUT); // echoPin을 입력이다.
  pinMode(trigPin2, OUTPUT); // trigPin을 출력으로 tirgPin2 : rail US_fw
  pinMode(echoPin2, INPUT); // echoPin을 입력이다.
  pinMode(trigPin3, OUTPUT); // trigPin을 출력으로 tirgPin3 : rail US_bw
  pinMode(echoPin3, INPUT); // echoPin을 입력이다.


}


//**** 여기 추가됐어 !! ************************************************************************8
int convert_direction(const int trigPin, const int echoPin)     //10cm 방향 확인 파트
{
  float length_rail;
  float distance_rail;
 
  /*digitalWrite(trigPin, LOW); //초음파 센서를 초기화 하는 과정입니다.
  delay(2);
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);


  length_rail = pulseIn(echoPin, HIGH); // 트리거 핀에서 나온 펄스를 받아서
  distance_rail = ((float)(340 * length_rail) / 10000) / 2;*/
  distance_rail = measure_value(trigPin,echoPin);


  if(distance_rail < 12)
  {
    return 1;   //변환 해줘야 함
  }
  else
    return 0;  //변환하지 말아야 함
}








float measure_value(const int trigPin, const int echoPin)
{
  int count;
  float prev;
  float length_rail;
  float distance_rail;
  count = 0;


  while(count<3)
  {
    digitalWrite(trigPin, LOW); //초음파 센서를 초기화 하는 과정입니다.
    delay(2);
    digitalWrite(trigPin, HIGH);
    delay(10);
    digitalWrite(trigPin, LOW);


    length_rail = pulseIn(echoPin, HIGH); // 트리거 핀에서 나온 펄스를 받아서
    distance_rail = ((float)(340 * length_rail) / 10000) / 2;


    Serial.println(distance_rail);
    if(distance_rail >200)
    {
      count = 0;
    }
    else if(count == 0 || ((prev -distance_rail >= 0) && (prev - distance_rail <= 3)) ||((distance_rail -prev >= 0) && (distance_rail - prev <= 3)))
    {
      prev = distance_rail;
      count++;
    }
    else
      count = 0;




  }
  return distance_rail;
}


int onrail(){
 
  //측정
  //delay(1000);
  distance_rail_bw = measure_value(trigPin2,echoPin2);
  /*
  digitalWrite(trigPin2, LOW); //초음파 센서를 초기화 하는 과정입니다.
  delay(2);
  digitalWrite(trigPin2, HIGH);
  delay(10);
  digitalWrite(trigPin2, LOW);


  length_rail_bw = pulseIn(echoPin2, HIGH); // 트리거 핀에서 나온 펄스를 받아서
  distance_rail_bw = ((float)(340 * length_rail_bw) / 10000) / 2; // 거리를 측정합니다.
  */
  Serial.print ("Ultra Sensor2 : ");
  Serial.print(distance_rail_bw);
  Serial.print("cm, ");
  //delay(1000); //1초마다 , 그리고 아래의 과정은 모두 동일합니다.
  distance_rail_fw = measure_value(trigPin3,echoPin3);
  /*
  digitalWrite(trigPin3, LOW); //초음파 센서를 초기화 하는 과정입니다.
  delay(2);
  digitalWrite(trigPin3, HIGH);
  delay(10);
  digitalWrite(trigPin3, LOW);


  length_rail_fw = pulseIn(echoPin3, HIGH); // 트리거 핀에서 나온 펄스를 받아서
  distance_rail_fw = ((float)(340 * length_rail_fw) / 10000) / 2; // 거리를 측정합니다.


  */
  Serial.print("Ultra Sensor3 : ");
  Serial.print(distance_rail_fw);
  Serial.println("cm");// 거리 값


  if (distance_rail_bw > distance_rail_fw) {
    return 1;
   
  } else {
    return 0;
  }
}




int check_updown()
{
  int change_flag;
  stopdistance_new = measure_value(trigPin1, echoPin1);
  /*
  digitalWrite(trigPin1, LOW); //초음파 센서를 초기화 하는 과정입니다.
  delay(2);
  digitalWrite(trigPin1, HIGH);
  delay(10);
  digitalWrite(trigPin1, LOW);




  //변화된 거리를 측정
  stopLength_new = pulseIn(echoPin1, HIGH); // 트리거 핀에서 나온 펄스를 받아서
  stopdistance_new= ((float)(340 * stopLength_new) / 10000) / 2; // 거리를 측정합니다.
  */


  Serial.print ("Ultra Sensor1 : "); //센서 1에
  Serial.print (stopdistance_new) ;// 거리 값
  Serial.println("cm"); // cm를 출력합니다.
   
  //delay(2000); //2초마다 , 그리고 아래의 과정은 모두 동일합니다.
 
  /// US part, updown
  if ((stopdistance_prev - stopdistance_new) > 10 && (stopdistance_prev > stopdistance_new) ) { // new 값이 전 값보다 이하의 차이가 난다면


    targetdistance = stopdistance_new;


    // DC 모터 멈춤 (레일 위)
    Serial.print(targetdistance);
    Serial.println("cm");




    //delay(2000);
    change_flag = 1;
    // 실로 dc모터(도르래)가 몇 초만큼 회전할 지 거리 - > 시간으로 환산


  } else {
    change_flag = 0;
    Serial.println("not change");
    //delay(2000);
  }


  stopdistance_prev = stopdistance_new;
  stopdistance_new = 0;
  stopLength_new = 0;
  return change_flag;








}




void loop() {
  // put your main code here, to run repeatedly:
  //0. 초기 디폴트 값 측정
    //0.1 디폴트 거리 값 측정 ***************


  float length_rail_fw = 0;
  float distance_rail_fw = 0;
  float length_rail_bw = 0;
  float distance_rail_bw = 0;


  wheel_direction = onrail();


    //0.1 updown default 값 측정
  stopdistance_prev = measure_value(trigPin1,echoPin1);
  /*
  digitalWrite(trigPin1, LOW); //초음파 센서를 초기화 하는 과정입니다.
  delay(2);
  digitalWrite(trigPin1, HIGH);
  delay(10);
  digitalWrite(trigPin1, LOW);




  stopLength_prev = pulseIn(echoPin1, HIGH); // 트리거 핀에서 나온 펄스를 받아서
  stopdistance_prev= ((float)(340 * stopLength_prev) / 10000) / 2; // 거리를 측정합니다.
  */
  Serial.print ("Ultra Sensor : "); //센서 1에
  Serial.print (stopdistance_prev) ;// 거리 값
  Serial.println("cm"); // cm를 출력합니다.
   
  delay(2000); //2초마다 , 그리고 아래의 과정은 모두 동일합니다.
    //0.2 디폴트 압력 값 측정
  sensorValue = analogRead(pressureSensor);
  pressureDefault = map(sensorValue,0,1023,0,100);


  Serial.print("right default ");
  Serial.println(pressureDefault);
  delay(4800);








  //1. Start(휴대폰을 통한 블루투스 연결)
  //1.1 휴대폰으로 통해 통신
  if(BTSerial.available())
  {
    if(BTSerial.available())
    {
      BTvalue = BTSerial.read();
      start_button = BTvalue - 48;
    }
  }
  Serial.println(start_button);
  //1.2 무한 회전 DC 모터가 돌아감
  if(start_button == 1)
  {
    //DC모터가 돌아감
    while(flag_dc_down == 0)
    {
      if(wheel_direction == 1)
      {
        motor1.run(BACKWARD);
        motor2.run(FORWARD);
        //delay(1000);
      }
      //digitalWrite(wheelApin1,LOW);
      //digitalWrite(wheelBpin1,LOW);
      //delay(100);
      else
      {
        motor1.run(FORWARD);
        motor2.run(BACKWARD);
        //delay(1000);
      }
      //wheel_direction = onrail();
      //**************10cm 변경된 부분 ********************************************
      //2. 서보모터 움직임 제어(초음파 센서)*************************
      //:앞에 다른 장애물이 감지되면 이동 정지*************************
      if(wheel_direction == 1)
      {
        convert_flag = convert_direction(trigPin2,echoPin2);   //모터 진행하는 쪽에 있는 센서를 함수에 보냄
      }
      else
      {
        convert_flag = convert_direction(trigPin3,echoPin3);
      }
      if (convert_flag == 1)
      {
        if(wheel_direction == 1)
         wheel_direction = 0;
        else
          wheel_direction = 1;
      }
      else
        wheel_direction = wheel_direction;
    //3. DC 모터 내려옴 : 초음파 센서
    //3.1 조건: 디폴트 값보다 초음파 센서 측정 값이 작으면*********
      flag_dc_down =check_updown();
    }
    //3.2 서보모터 정지**************
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    //3.3 목표물까지 거리 측정 -> 내려오는 거리 환산***************
    target_dist = targetdistance;
    Serial.println("stop");
    Serial.println("flag_dc_down:");
    Serial.println(flag_dc_down);
    if(flag_dc_down == 1)  //후에 변경
    {
      //3.4 DC 모터가 내려옴
      //내려오는 거
      motor0.run(BACKWARD);
      target_time = 200 * target_dist;
      delay(target_time);   //해당 시간만큼 내려옴 ***
      //3.5 정지
      //digitalWrite(motorpin1,HIGH);
      //digitalWrite(motorpin2,HIGH);
      Serial.println("second motor");
    }
    motor0.run(RELEASE);
    //4 DC 모터 올라감:FSR 센서
    //디폴트 값보다 압력센서 값이 클 떄까지 대기하면서 계속 측정
    while(pressureValue <= pressureDefault + 10)   //어느정도 값을 기준으로 해야할지 조정 필요 ***
    {
      sensorValue = analogRead(pressureSensor);
      pressureValue = map(sensorValue,0,1023,0,100);
      delay(1000);
      Serial.println("wait: pressure");
    }
    //4.1 디폴트 값보다 압력 센서 측정 값이 크면
    //4.2 앞에서 측정된 거리만큼 DC 모터가 올라감
    motor0.run(FORWARD);
    delay(target_time);   //1000ms 당 5cm


    //4.3 다 올라가면 정지
    motor0.run(RELEASE);
   
    target_dist = 0;
    target_time = 0;
    pressureValue = 0;
    while(start_button == 1)
    {
      if(BTSerial.available())
      {


        BTvalue = BTSerial.read();
        start_button = BTvalue - 48;
      }
      //Serial.println("button");
      Serial.println(start_button);
      delay(2000);
    }
    //delay(2000);
    Serial.println("finish");
    //stop code
    onrail();
    if(wheel_direction == 1)
    {
      motor1.run(FORWARD);
      motor2.run(BACKWARD);
        //delay(1000);
    }
      //digitalWrite(wheelApin1,LOW);
      //digitalWrite(wheelBpin1,LOW);
      //delay(100);
    else
    {
      motor1.run(BACKWARD);
      motor2.run(FORWARD);
      //delay(1000);
    }
    if(wheel_direction == 1)
    {
      finishdistance = measure_value(trigPin3,echoPin3);
    }
    else
      finishdistance = measure_value(trigPin2,echoPin2);


    while(finishdistance > 12)
    {
      if(wheel_direction == 1)
      {
        finishdistance = measure_value(trigPin3,echoPin3);
      }
      else
        finishdistance = measure_value(trigPin2,echoPin2);
      delay(1000);
    }
    motor1.run(RELEASE);
    motor2.run(RELEASE);
  }
}
