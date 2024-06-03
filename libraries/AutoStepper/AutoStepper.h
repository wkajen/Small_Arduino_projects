#ifndef AutoStepper_h
#define AutoStepper_h

class AutoStepper {
  public:
    // constructor
    AutoStepper(int motorPin1, int motorPin2, int motorPin3, int motorPin4);

    int motorSpinning(int dir, bool autoWork);

    int stepNoTotal;

  private:
    void oneStepMotor(int dir);

    int motorPin1;
    int motorPin2;
    int motorPin3;
    int motorPin4;

    int stepNo;
    
};

#endif