#ifndef MyStepper_h
#define MyStepper_h

class MyStepper {
  public:
    // constructor
    MyStepper(int motorPin1, int motorPin2, int motorPin3, int motorPin4, int stepsLimit);

    int motorSpinning(int dir, bool autoWork);

    int stepNoTotal;

  private:
    void oneStepMotor(int dir, bool autoWork);

    int motorPin1;
    int motorPin2;
    int motorPin3;
    int motorPin4;

    int stepsLimit;
    int stepNo;
    
};

#endif