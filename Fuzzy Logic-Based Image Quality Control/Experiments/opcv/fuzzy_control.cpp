#include"fl/Headers.h"

using namespace fl;

//fuzzy_control



double fuzzy_control(double input1, double input2, double input3, double input4)
{
	

	Engine* engine = new Engine;
	engine->setName("light_LED");
	engine->setDescription("(gmean, SNR and entropy) -> (PWM)");

	InputVariable* E = new InputVariable;
	E->setName("E");
	E->setDescription("Image grayvalue error");
	E->setEnabled(true);
	E->setRange(-15.000, 35.000);
	E->setLockValueInRange(true);
	E->addTerm(new Trapezoid("NL", -15.000, -15.000, -10.000, -5.000));
	E->addTerm(new Triangle("NS", -10.000, -5.000, 0.000));
	E->addTerm(new Triangle("Z", -2.000, 0.000, 12.000));
	E->addTerm(new Triangle("PS", 0.000, 12.000, 24.000));
	E->addTerm(new Trapezoid("PL", 12.000, 24.000, 35.000, 35.000));
	engine->addInputVariable(E);

	InputVariable* ER = new InputVariable;
	ER->setName("ER");
	ER->setDescription("Image grayvalue error rate");
	ER->setEnabled(true);
	ER->setRange(-15.000, 15.000);
	ER->setLockValueInRange(true);
	ER->addTerm(new Trapezoid("NL", -15.000, -15.000, -10.000, -5.000));
	ER->addTerm(new Triangle("NS", -10.000, -5.000, 0.000));
	ER->addTerm(new Triangle("Z", -5.000, 0.000, 5.000));
	ER->addTerm(new Triangle("PS", 0.000, 5.000, 10.000));
	ER->addTerm(new Trapezoid("PL", 5.000, 10.000, 15.000, 15.000));
	engine->addInputVariable(ER);

	/*InputVariable* ER = new InputVariable;
	ER->setName("ER");
	ER->setDescription("Image grayvalue error rate");
	ER->setEnabled(true);
	ER->setRange(-1.000, 1.000);
	ER->setLockValueInRange(true);
	ER->addTerm(new Trapezoid("NL", -1.000, -1.000, -0.660, -0.320));
	ER->addTerm(new Triangle("NS", -0.660, -0.320, 0.000));
	ER->addTerm(new Triangle("Z", -0.320, 0.000, 0.320));
	ER->addTerm(new Triangle("PS", 0.000, 0.320, 0.660));
	ER->addTerm(new Trapezoid("PL", 0.320, 0.660, 1.000, 1.000));
	engine->addInputVariable(ER);*/

	InputVariable* SNR = new InputVariable;
	SNR->setName("SNR");
	SNR->setDescription("Image SNR");
	SNR->setEnabled(true);
	SNR->setRange(-50.000, 7.000);
	SNR->setLockValueInRange(true);
	SNR->addTerm(new Trapezoid("poor", -50.000, -50.000, 0.000, 3.000));
	SNR->addTerm(new Triangle("good", 1.000, 3.000, 5.000));
	SNR->addTerm(new Trapezoid("excellent", 3.000, 5.500, 7.000, 7.000));
	engine->addInputVariable(SNR);

	InputVariable* entropy = new InputVariable;
	entropy->setName("entropy");
	entropy->setDescription("Image entropy");
	entropy->setEnabled(true);
	entropy->setRange(0.000, 6.000);
	entropy->setLockValueInRange(true);
	entropy->addTerm(new Trapezoid("poor", 0.000, 0.000, 1.000, 3.000));
	entropy->addTerm(new Triangle("good", 1.000, 3.000, 5.000));
	entropy->addTerm(new Trapezoid("excellent", 3.000, 5.000, 6.000, 6.000));
	engine->addInputVariable(entropy);

	OutputVariable* mPWM = new OutputVariable;
	mPWM->setName("mPWM");
	mPWM->setDescription("PWM based on Mamdani inference");
	mPWM->setEnabled(true);
	mPWM->setRange(0.000, 255.000);
	mPWM->setLockValueInRange(false);
	mPWM->setAggregation(new Maximum);
	mPWM->setDefuzzifier(new Centroid(1000));
	mPWM->setDefaultValue(fl::nan);
	mPWM->setLockPreviousValue(false);
	mPWM->addTerm(new Trapezoid("NL", 0.000, 0.000, 31.875, 63.750));
	mPWM->addTerm(new Triangle("NM", 31.875, 63.750, 95.625));
	mPWM->addTerm(new Triangle("NS", 63.750, 95.625, 127.500));
	mPWM->addTerm(new Triangle("Z", 95.625, 127.500, 159.375));
	mPWM->addTerm(new Triangle("PS", 127.500, 159.375, 191.250));
	mPWM->addTerm(new Triangle("PM", 159.375, 191.250, 223.125));
	mPWM->addTerm(new Trapezoid("PL", 191.250, 223.125, 255.000, 255.000));
	engine->addOutputVariable(mPWM);

	

	RuleBlock* mamdani = new RuleBlock;
	mamdani->setName("mamdani");
	mamdani->setDescription("Mamdani inference");
	mamdani->setEnabled(true);
	mamdani->setConjunction(new Minimum);
	mamdani->setDisjunction(new Maximum);
	mamdani->setImplication(new Minimum);
	mamdani->setActivation(new General);
	mamdani->addRule(Rule::parse("if SNR is poor or entropy is poor then mPWM is PL", engine));
	//mamdani->addRule(Rule::parse("if SNR is good and entropy is good then mPWM is PM with 0.5", engine));
	//mamdani->addRule(Rule::parse("if SNR is excellent or entropy is excellent then mPWM is PS with 0.3", engine));

	//PD LIKE FUZZY
	mamdani->addRule(Rule::parse("if E is PL and ER is PL then mPWM is PL", engine));
	mamdani->addRule(Rule::parse("if E is PL and ER is PS then mPWM is PL", engine));
	mamdani->addRule(Rule::parse("if E is PS and ER is PL then mPWM is PL", engine));

	mamdani->addRule(Rule::parse("if E is Z and ER is PL then mPWM is PM", engine));
	mamdani->addRule(Rule::parse("if E is PS and ER is PS then mPWM is PM", engine));
	mamdani->addRule(Rule::parse("if E is PL and ER is Z then mPWM is PM", engine));

	mamdani->addRule(Rule::parse("if E is NS and ER is PL then mPWM is PS", engine));
	mamdani->addRule(Rule::parse("if E is Z and ER is PS then mPWM is PS", engine));
	mamdani->addRule(Rule::parse("if E is PS and ER is Z then mPWM is PS", engine));
	mamdani->addRule(Rule::parse("if E is PL and ER is NL then mPWM is PS", engine));

	mamdani->addRule(Rule::parse("if E is PL and ER is NL then mPWM is Z ", engine));
	mamdani->addRule(Rule::parse("if E is PS and ER is NS then mPWM is Z ", engine));
	mamdani->addRule(Rule::parse("if E is Z and ER is Z then mPWM is Z ", engine));
	mamdani->addRule(Rule::parse("if E is NS and ER is PS then mPWM is Z ", engine));
	mamdani->addRule(Rule::parse("if E is NL and ER is PL then mPWM is Z ", engine));
	

	mamdani->addRule(Rule::parse("if E is NL and ER is PS then mPWM is NS", engine));
	mamdani->addRule(Rule::parse("if E is NS and ER is Z then mPWM is NS", engine));
	mamdani->addRule(Rule::parse("if E is Z and ER is NS then mPWM is NS", engine));
	mamdani->addRule(Rule::parse("if E is PS and ER is NL then mPWM is NS", engine));

	mamdani->addRule(Rule::parse("if E is NL and ER is Z then mPWM is NM", engine));
	mamdani->addRule(Rule::parse("if E is NS and ER is NS then mPWM is NM", engine));
	mamdani->addRule(Rule::parse("if E is Z and ER is NL then mPWM is NM", engine));

	mamdani->addRule(Rule::parse("if E is NS and ER is NL then mPWM is NL", engine));
	mamdani->addRule(Rule::parse("if E is NL and ER is NS then mPWM is NL", engine));
	mamdani->addRule(Rule::parse("if E is NL and ER is NL then mPWM is NL", engine));


	engine->addRuleBlock(mamdani);

	scalar inputVariable1=input1;
	E->setValue(inputVariable1);

	scalar inputVariable2 = input2;
	ER->setValue(inputVariable2);

	scalar inputVariable3 = input3;
	SNR->setValue(inputVariable3);

	scalar inputVariable4 = input4;
	entropy->setValue(inputVariable4);

	engine->process();

	scalar result ;
	result = mPWM->getValue();
	return result;

	

	

}

